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

#include <stdio.h>
#include <stdlib.h>

#include "user-manager.h"

#define __qol_unused__ __attribute__((unused))

int main(__qol_unused__ int argc, __qol_unused__ char **argv)
{
        QolUserManager *manager = NULL;
        QolUser *user = NULL;

        manager = qol_user_manager_new();
        if (!manager) {
                fputs("Cannot get the system users!\n", stderr);
                return EXIT_FAILURE;
        }

        fprintf(stderr, "Not yet fully implemented\n");

        /* Wind the user list in reverse */
        for (user = manager->users; user; user = user->next) {
                if (!qol_user_is_admin(user) || !qol_user_is_active(user)) {
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

        qol_user_manager_free(manager);

        return EXIT_FAILURE;
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
