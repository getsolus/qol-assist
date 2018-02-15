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

#include <stdio.h>
#include <stdlib.h>

#include "migrate.h"

QolContext *qol_context_new(void)
{
        QolContext *ret = NULL;

        ret = calloc(1, sizeof(QolContext));
        if (!ret) {
                fputs("OOM\n", stderr);
                return NULL;
        }

        ret->user_manager = qol_user_manager_new();
        if (!ret->user_manager) {
                goto failed;
        }

        return ret;

failed:
        qol_context_free(ret);
        return NULL;
}

void qol_context_free(QolContext *self)
{
        if (!self) {
                return;
        }
        qol_user_manager_free(self->user_manager);
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
