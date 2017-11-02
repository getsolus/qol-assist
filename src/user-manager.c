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

#include <stdlib.h>

#include "user-manager.h"

static void qol_user_free(QolUser *user);
static QolUser *qol_user_new(void);

QolUserManager *qol_user_manager_new(void)
{
        QolUserManager *ret = NULL;

        ret = calloc(1, sizeof(QolUserManager));
        if (!ret) {
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

/**
 * Construct a new QolUser
 *
 * TODO: Actually construct this guy using pwent stuff
 */
QolUser *qol_user_new()
{
        QolUser *ret = NULL;

        ret = calloc(1, sizeof(QolUser));
        if (!ret) {
                return NULL;
        }

        /* TODO: Init user */
        return ret;
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
