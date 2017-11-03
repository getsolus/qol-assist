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
#include <string.h>
#include <unistd.h>

#include "util.h"

/**
 * This is a bit gross, but basically we read the file twice..
 * Once to get the size, and a second time to copy.
 *
 * This ensures we still use glibc getusershell but is much cheaper than
 * trying to get the shells for every single user.
 */
char **qol_get_shells(size_t *n_shells)
{
        char **ret = NULL;
        size_t shells = 0;
        size_t i = 0;
        char *s = NULL;

        *n_shells = 0;

        setusershell();
        while ((s = getusershell())) {
                ++shells;
        }
        s = NULL;
        endusershell();

        /* Allocate root array */
        ret = calloc(shells, sizeof(char *));
        if (!ret) {
                return NULL;
        }

        setusershell();
        while ((s = getusershell())) {
                char *t = strdup(s);
                if (!t) {
                        shells = i;
                        goto failed;
                }
                ret[i] = t;
                ++i;
        }
        endusershell();

        *n_shells = shells;
        return ret;

failed:
        qol_free_stringv(ret, shells);
        return NULL;
}

void qol_free_stringv(char **strv, size_t n_str)
{
        if (!strv) {
                return;
        }
        for (size_t i = 0; i < n_str; i++) {
                if (strv[i]) {
                        free(strv[i]);
                }
        }
        free(strv);
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
