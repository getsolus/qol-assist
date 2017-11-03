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

#include <stdio.h>

#include "declared.h"

#define WANTED_GROUP "scanner"

/**
 * Add all active/admin users into the scanner group
 */
bool qol_migration_01_scanner_group(QolContext *context, int level)
{
        for (QolUser *user = context->user_manager->users; user; user = user->next) {
                if (!qol_user_is_active(user) || !qol_user_is_admin(user)) {
                        continue;
                }
                if (qol_user_in_group(user, WANTED_GROUP)) {
                        continue;
                }
                fprintf(stderr, " -> Adding '%s' to '%s'\n", user->name, WANTED_GROUP);
                if (!qol_user_add_to_group(user, WANTED_GROUP)) {
                        fprintf(stderr,
                                "Failed to add user '%s' to group '%s'\n",
                                user->name,
                                WANTED_GROUP);
                        return false;
                }
        }
        return true;
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
