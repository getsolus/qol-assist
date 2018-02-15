/*
 * This file is part of qol-assist.
 *
 * Copyright © 2017-2018 Solus Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "user-manager.h"
#include "util.h"

static void qol_user_free(QolUser *user);
static QolUser *qol_user_new(struct passwd *pwd);

QolUserManager *qol_user_manager_new(void)
{
        QolUserManager *ret = NULL;

        ret = calloc(1, sizeof(QolUserManager));
        if (!ret) {
                fputs("OOM\n", stderr);
                return NULL;
        }

        /* Attempt to load the manager for the first time */
        if (!qol_user_manager_refresh(ret)) {
                qol_user_manager_free(ret);
                return NULL;
        }

        return ret;
}

void qol_user_manager_free(QolUserManager *self)
{
        if (!self) {
                return;
        }
        qol_user_free(self->users);
        qol_free_stringv(self->shells, self->n_shells);
        free(self);
}

/**
 * Determine if the shell is valid by referencing our cached shells file
 */
static bool qol_user_manager_is_shell_valid(QolUserManager *self, const char *shell)
{
        if (!shell) {
                return false;
        }

        for (size_t i = 0; i < self->n_shells; i++) {
                if (strcmp(self->shells[i], shell) == 0) {
                        return true;
                }
        }
        return false;
}

bool qol_user_manager_refresh(QolUserManager *self)
{
        struct passwd *pwd = NULL;
        QolUser *root_user = NULL;
        bool ret = false;

        qol_free_stringv(self->shells, self->n_shells);
        self->n_shells = 0;

        /* Must be able to grab shells! */
        self->shells = qol_get_shells(&self->n_shells);
        if (!self->shells) {
                return false;
        }

        setpwent();

        while ((pwd = getpwent()) != NULL) {
                QolUser *user = NULL;

                user = qol_user_new(pwd);
                if (!user) {
                        goto end;
                }

                user->valid_shell = qol_user_manager_is_shell_valid(self, pwd->pw_shell);

                /* Merge list */
                user->next = root_user;
                root_user = user;
        }

        /* Ensure we don't have any existing users now */
        qol_user_free(self->users);
        self->users = root_user;
        ret = true;

end:
        endpwent();

        return ret;
}

/**
 * Attempt to assign all group names to the User object
 */
static bool qol_user_assign_groups(QolUser *user)
{
        gid_t *groups = NULL;
        int n_groups = 5; /* Try just 5 groups at first */
        int r = -1;
        bool ret = false;
        size_t out_n_groups = 0;
        char **out_groups = NULL;

        groups = calloc((size_t)n_groups, sizeof(gid_t));
        if (!groups) {
                fputs("OOM\n", stderr);
                return false;
        }

        /* Attempt to grab the initial group listing */
        r = getgrouplist(user->name, user->gid, groups, &n_groups);
        if (r < 0) {
                gid_t *r_groups = NULL;

                /* If the n_groups didn't change, something is bork. */
                if (n_groups == 5) {
                        goto failed;
                }

                /* Reallocate the buffer, try again */
                r_groups = realloc(groups, ((size_t)n_groups) * sizeof(gid_t));
                if (!r_groups) {
                        fputs("OOM\n", stderr);
                        goto failed;
                }

                groups = r_groups;

                r = getgrouplist(user->name, user->gid, groups, &n_groups);
                if (r < 0) {
                        goto failed;
                }
        }

        /* Set up storage for the groups */
        out_n_groups = (size_t)n_groups;
        out_groups = calloc(out_n_groups, sizeof(char *));

        if (!out_groups) {
                goto failed;
        }

        /* Now pop all the group names into the user's groups */
        for (int i = 0; i < n_groups; i++) {
                struct group *group = NULL;
                char *cp = NULL;

                group = getgrgid(groups[i]);
                if (!group) {
                        if (errno != 0) {
                                goto failed;
                        }
                        fprintf(stderr,
                                "Skipping invalid group '%d' for '%s'\n",
                                groups[i],
                                user->name);
                        continue;
                }

                cp = strdup(group->gr_name);
                if (!cp) {
                        fputs("OOM\n", stderr);
                        goto failed;
                }
                out_groups[i] = cp;
        }

        /* We may be called to refresh */
        qol_free_stringv(user->groups, user->n_groups);
        user->n_groups = out_n_groups;
        user->groups = out_groups;
        out_groups = NULL;

        ret = true;

failed:
        if (groups) {
                free(groups);
        }
        if (out_groups) {
                qol_free_stringv(out_groups, out_n_groups);
        }
        return ret;
}

/**
 * Construct a new QolUser
 */
QolUser *qol_user_new(struct passwd *pwd)
{
        QolUser *ret = NULL;

        ret = calloc(1, sizeof(QolUser));
        if (!ret) {
                fputs("OOM\n", stderr);
                return NULL;
        }

        ret->uid = pwd->pw_uid;
        ret->gid = pwd->pw_gid;
        ret->name = strdup(pwd->pw_name);
        if (!ret->name) {
                goto failed;
        }

        if (!qol_user_assign_groups(ret)) {
                goto failed;
        }

        return ret;

failed:
        qol_user_free(ret);
        return NULL;
}

/**
 * Delete a QolUser and forward it along the chain
 */
static void qol_user_free(QolUser *user)
{
        if (!user) {
                return;
        }

        /* Chain to the next guy */
        if (user->next) {
                qol_user_free(user->next);
        }

        /* Clear out the groups */
        qol_free_stringv(user->groups, user->n_groups);

        if (user->name) {
                free(user->name);
        }
        free(user);
}

/**
 * Simple detection: is the shell valid, and is the UID high enough.
 */
bool qol_user_is_active(QolUser *self)
{
        if (!self) {
                return false;
        }
        if (self->uid >= QOL_MIN_UID && self->valid_shell) {
                return true;
        }
        return false;
}

bool qol_user_in_group(QolUser *self, const char *group)
{
        if (!self) {
                return false;
        }

        /* Walk groups and check membership */
        for (size_t i = 0; i < self->n_groups; i++) {
                if (strcmp(self->groups[i], group) == 0) {
                        return true;
                }
        }
        return false;
}

bool qol_user_is_admin(QolUser *self)
{
        if (!self) {
                return false;
        }

        /* Is this root? */
        if (self->uid == 0 && self->gid == 0) {
                return true;
        }

        /* Wheel (sudo) user? */
        if (qol_user_in_group(self, QOL_WHEEL_GROUP)) {
                return true;
        }

        /* Not admin. */
        return false;
}

bool qol_user_add_to_group(QolUser *self, const char *group)
{
        char *command[] = {
                "/usr/sbin/usermod", /** TODO: Perhaps configure option */
                "-a",
                "-G",
                (char *)group,
                self->name,
                NULL,
        };

        if (!qol_exec_command(command)) {
                return false;
        }

        /* Rebuild groups */
        if (!qol_user_assign_groups(self)) {
                return false;
        }

        /* Ensure we're really in the group now */
        if (!qol_user_in_group(self, group)) {
                return false;
        }

        return true;
}

int qol_user_manager_get_group_id(__qol_unused__ QolUserManager *self, const char *group)
{
        struct group *grp = NULL;

        grp = getgrnam(group);
        if (!grp) {
                return -1;
        }
        return (int)grp->gr_gid;
}

bool qol_user_manager_change_group_id(QolUserManager *self, const char *group, int gid)
{
        char *gid_str = NULL;
        bool ret = false;

        char *command[] = {
                "/usr/sbin/groupmod",
                "-g",
                NULL, /* gid */
                NULL, /* group */
                NULL,
        };

        if (asprintf(&gid_str, "%u", gid) < 0) {
                return false;
        }

        command[2] = gid_str;
        command[3] = (char *)group;

        if (!qol_exec_command(command)) {
                goto failed;
        }

        /* Check the updated gid is now valid */
        if (qol_user_manager_get_group_id(self, group) != gid) {
                goto failed;
        }

        ret = true;

failed:
        free(gid_str);
        return ret;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
