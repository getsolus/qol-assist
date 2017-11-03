/*
 * This file is part of qol-assist.
 *
 * Copyright © 2017 Solus Project
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

        /* TODO: Init the user manager */
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

        groups = calloc((size_t)n_groups, sizeof(gid_t));
        if (!groups) {
                fputs("OOM\n", stderr);
                return false;
        }

        /* Attempt to grab the initial group listing */
        r = getgrouplist(user->name, user->gid, groups, &n_groups);
        if (r < 0) {
                /* If the n_groups didn't change, something is bork. */
                if (n_groups == 5) {
                        goto failed;
                }
                /* Reallocate the buffer, try again */
                groups = realloc(groups, ((size_t)n_groups) * sizeof(gid_t));
                if (!groups) {
                        fputs("OOM\n", stderr);
                        goto failed;
                }
                r = getgrouplist(user->name, user->gid, groups, &n_groups);
                if (r < 0) {
                        goto failed;
                }
        }

        /* Set up storage for the groups */
        user->n_groups = (size_t)n_groups;
        user->groups = calloc(user->n_groups, sizeof(char *));

        if (!user->groups) {
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
                user->groups[i] = cp;
        }

        ret = true;

failed:
        if (groups) {
                free(groups);
        }
        return ret;
}

/**
 * Construct a new QolUser
 *
 * TODO: Actually construct this guy using pwent stuff
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

        /* TODO: Init user fully */
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
        if (user->groups) {
                for (size_t i = 0; i < user->n_groups; i++) {
                        if (user->groups[i]) {
                                free(user->groups[i]);
                        }
                }
                free(user->groups);
        }

        if (user->name) {
                free(user->name);
        }
        free(user);
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
