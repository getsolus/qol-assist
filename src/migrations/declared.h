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

#pragma once

#include "../migrate.h"

/**
 * Add any active admins to the specified group if they're not in it already
 *
 * @param context Pointer to a valid QolContext
 * @param group Name of the group for the users to join
 *
 * @returns True if the operation succeeded
 */
bool qol_migration_push_active_admin_group(QolContext *context, const char *group);

/* Migrations follow */

bool qol_migration_01_scanner_group(QolContext *context);

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
