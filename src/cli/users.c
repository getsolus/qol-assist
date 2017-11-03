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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "migrate.h"

static bool list_users_internal(bool require_admin, bool require_active, bool require_system)
{
        QolContext *context = NULL;
        QolUser *user = NULL;

        context = qol_context_new();
        if (!context) {
                fprintf(stderr, "Failed to construct QolContext: %s\n", strerror(errno));
                return false;
        }

        /* Wind the user list in reverse */
        for (user = context->user_manager->users; user; user = user->next) {
                if (require_admin && !qol_user_is_admin(user)) {
                        continue;
                }
                if (require_active && !qol_user_is_active(user)) {
                        continue;
                }
                if (require_system && qol_user_is_active(user)) {
                        continue;
                }
                fprintf(stdout, "User: %s (", user->name);
                for (size_t i = 0; i < user->n_groups; i++) {
                        fprintf(stdout,
                                "%s%s%s",
                                i != 0 ? ":" : "",
                                user->groups[i],
                                i == user->n_groups - 1 ? ")\n" : "");
                }
        }

        qol_context_free(context);
        return true;
}

static bool list_all_users(void)
{
        return list_users_internal(false, false, false);
}

static bool list_system_users(void)
{
        return list_users_internal(false, false, true);
}

/**
 * List all normal users on the system
 */
static bool list_users(void)
{
        return list_users_internal(false, true, false);
}

/**
 * List all admin users on the system
 */
static bool list_admins(void)
{
        return list_users_internal(true, true, false);
}

bool qol_cli_list_users(int argc, char **argv)
{
        const char *t = NULL;

        if (argc != 1) {
                fprintf(stderr, "usage: list-users [system|all|admin|active]\n");
                return false;
        }

        t = argv[0];

        if (strcmp(t, "system") == 0) {
                return list_system_users();
        } else if (strcmp(t, "all") == 0) {
                return list_all_users();
        } else if (strcmp(t, "admin") == 0) {
                return list_admins();
        } else if (strcmp(t, "active") == 0) {
                return list_users();
        }

        fprintf(stderr, "Unknown type: %s\n", t);
        return false;
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
