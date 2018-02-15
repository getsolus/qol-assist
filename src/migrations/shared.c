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
#include <stdio.h>
#include <string.h>

#include "declared.h"

static bool qol_migration_push(QolContext *context, const char *group, bool require_admin)
{
        for (QolUser *user = context->user_manager->users; user; user = user->next) {
                if (!qol_user_is_active(user)) {
                        continue;
                }
                if (require_admin && !qol_user_is_admin(user)) {
                        continue;
                }
                if (qol_user_in_group(user, group)) {
                        continue;
                }
                fprintf(stderr, " -> Adding '%s' to '%s'\n", user->name, group);
                if (!qol_user_add_to_group(user, group)) {
                        fprintf(stderr,
                                "Failed to add user '%s' to group '%s'\n",
                                user->name,
                                group);
                        return false;
                }
        }
        return true;
}

/**
 * Shared code for our API..
 */
bool qol_migration_push_active_admin_group(QolContext *context, const char *group)
{
        return qol_migration_push(context, group, true);
}

bool qol_migration_push_active_group(QolContext *context, const char *group)
{
        return qol_migration_push(context, group, false);
}

bool qol_migration_update_group_id(QolContext *context, const char *group, int gid)
{
        int current_id = 1;

        current_id = qol_user_manager_get_group_id(context->user_manager, group);
        if (current_id < 0) {
                fprintf(stderr, "Failed to find '%s' group: %s\n", group, strerror(errno));
                return false;
        }

        /* No need to perform an update here, it's been done */
        if (current_id == gid) {
                return true;
        }

        fprintf(stderr, "Changing group '%s' ID from %d to %d\n", group, current_id, gid);
        if (!qol_user_manager_change_group_id(context->user_manager, group, gid)) {
                fprintf(stderr, "Failed to change group id: %s\n", strerror(errno));
                return false;
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
