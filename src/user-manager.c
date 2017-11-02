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
        free(self);
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
