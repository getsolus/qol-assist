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

typedef struct QolUserManager {
        int __reserved1;
} QolUserManager;

/**
 * Create a new QolUserManager to handle system user management
 *
 * @returns Newly allocated QolUserManager
 */
QolUserManager *qol_user_manager_new(void);

/**
 * Free a previously allocated QolUserManager
 *
 * @param manager Pointer to an allocated QolUserManager
 */
void qol_user_manager_free(QolUserManager *manager);

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
