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

/**
 * Fix the users group GID.
 *
 * Solus never used this group before, however by the time we needed it for
 * SAMBA, a systemd update had created a 'users' group with a bogus GID in
 * the 900 range, so we forcibly set it back to 100 here before using it
 * for anything.
 */
bool qol_migration_03_users_group_gid(QolContext *context)
{
        return qol_migration_update_group_id(context, "users", 100);
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
