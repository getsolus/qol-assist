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

#include <stdbool.h>
#include <sys/types.h>

/**
 * QolUser represents a system user in an accessible fashion.
 */
typedef struct QolUser {
        struct QolUser *next; /**<Next user in the chain */

        char **groups;   /**<All groups the user belongs to */
        size_t n_groups; /**<Number of groups */

        char *name; /**<Username */
        uid_t uid;  /**<User ID */
        gid_t gid;  /**<Primary group ID */

        bool valid_shell; /**<Whether the shell is actually valid */
} QolUser;

/**
 * QolUserManager allows us to introspect the system users and modify them
 */
typedef struct QolUserManager {
        QolUser *users; /**<Known users */

        char **shells;
        size_t n_shells;
} QolUserManager;

/**
 * Create a new QolUserManager to handle system user management
 *
 * @returns Newly allocated QolUserManager
 */
QolUserManager *qol_user_manager_new(void);

/**
 * Attempt to refresh the manager state with the users
 *
 * @param manager Pointer to an allocated QolUserManager
 * @returns True if the operation succeeded
 */
bool qol_user_manager_refresh(QolUserManager *manager);

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
