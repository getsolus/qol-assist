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
#include <unistd.h>

#include "cli.h"
#include "config.h"
#include "migrate.h"
#include "user-manager.h"
#include "util.h"

#include "migrations/declared.h"

#define MIGRATION(x, y)                                                                            \
        {                                                                                          \
                x, qol_migration_##y                                                               \
        }

/**
 * Simple table describing all of our potential migrations
 */
static QolMigration migration_table[] = {
        MIGRATION("Add users to scanner group", 01_scanner_group),
};

/**
 * Track the maximum number of migrations available
 */
static size_t n_migrations = sizeof(migration_table) / sizeof(migration_table[0]);

bool qol_cli_migrate(__qol_unused__ int argc, __qol_unused__ char **argv)
{
        QolContext *context = NULL;
        size_t migration_level_start = 0;
        bool ret = false;
        const char *trigger_file = QOL_TRIGGER_FILE;

        if (geteuid() != 0 || getegid() != 0) {
                fprintf(stderr, "This command must be run with root privileges.\n");
                return false;
        }

        /* Make sure the trigger is actually there. */
        if (!qol_file_exists(trigger_file)) {
                fprintf(stderr, "Refusing to run migration without trigger file\n");
                return false;
        }

        context = qol_context_new();
        if (!context) {
                return false;
        }

        /* Emulate migration steps */
        for (size_t i = migration_level_start; i < n_migrations; i++) {
                QolMigration *m = &migration_table[i];

                fprintf(stdout, "Begin migration %lu: '%s'\n", i, m->name);
                if (!m->func(context, (int)i)) {
                        fprintf(stderr, "Failed migration %lu: '%s'\n", i, m->name);
                        goto end;
                }
                fprintf(stdout, "Successful migration %lu: '%s'\n", i, m->name);
        }

        /* Ensure the trigger file is actually there */
        if (unlink(trigger_file) != 0) {
                fprintf(stderr,
                        "WARNING: Failed to remove trigger file %s: %s\n",
                        trigger_file,
                        strerror(errno));
                goto end;
        }

        ret = true;

end:
        qol_context_free(context);
        return ret;
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
