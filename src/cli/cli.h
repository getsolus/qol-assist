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

/**
 * Basic CLI utilities
 */
typedef struct SubCommand {
        const char *name; /**<Name of this subcommand */
        bool (*execute)(int argc, char **argv);
        const char *short_desc; /**<Short description to display */
} SubCommand;

/**
 * List users on the system
 */
bool qol_cli_list_users(int argc, char **argv);

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
