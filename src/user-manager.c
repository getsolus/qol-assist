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

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user-manager.h"

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
        free(self);
}

bool qol_user_manager_refresh(QolUserManager *self)
{
        struct passwd *pwd = NULL;
        QolUser *root_user = NULL;
        bool ret = false;

        setpwent();

        while ((pwd = getpwent()) != NULL) {
                QolUser *user = NULL;

                user = qol_user_new(pwd);
                if (!user) {
                        goto end;
                }

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

        /* TODO: Init user fully */
        ret->name = strdup(pwd->pw_name);
        if (!ret->name) {
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
