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
#include <sys/stat.h>
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
        MIGRATION("Add users to plugdev group", 02_plugdev_group),
};

/**
 * Track the maximum number of migrations available
 */
static size_t n_migrations = sizeof(migration_table) / sizeof(migration_table[0]);

/**
 * Attempt to grab the current migration level
 *
 * If we don't find the file, we'll actually default to level 0
 * If we have parse issues, we'll default to 0.
 */
static bool qol_get_migration_level(int *level)
{
        const char *status_file = QOL_STATUS_FILE;
        FILE *f = NULL;
        int lvl = 0;

        if (!qol_file_exists(status_file)) {
                *level = 0;
                return true;
        }

        /* Something super janky. */
        f = fopen(status_file, "r");
        if (!f) {
                fprintf(stderr,
                        "Failed to read migration file %s: %s\n",
                        status_file,
                        strerror(errno));
                return false;
        }

        if (fscanf(f, "%d\n", &lvl) != 1) {
                fprintf(stderr, "Failed to parse migration level. Setting to 0\n");
                lvl = 0;
        } else {
                /* We have a valid migration level, so start 1 higher */
                ++lvl;
        }
        fclose(f);

        /* Make sure someone doesn't dick us over with a malformed file */
        if (lvl < 0) {
                lvl = 0;
        }

        *level = lvl;
        return true;
}

/**
 * Set the migration level on disk
 */
static bool qol_set_migration_level(int level)
{
        const char *status_file = QOL_STATUS_FILE;
        const char *trigger_dir = QOL_TRACK_DIR;
        FILE *f = NULL;
        bool ret = false;

        /* Make sure trigger directory exists */
        if (!qol_file_exists(trigger_dir)) {
                if (mkdir(trigger_dir, 00755) != 0) {
                        fprintf(stderr,
                                "Failed to construct directory %s: %s\n",
                                trigger_dir,
                                strerror(errno));
                        return false;
                }
        }

        if (qol_file_exists(status_file) && unlink(status_file) < 0) {
                fprintf(stderr,
                        "Failed to remove old trigger file %s: %s\n",
                        status_file,
                        strerror(errno));
                return false;
        }

        /* Something super janky. */
        f = fopen(status_file, "w");
        if (!f) {
                fprintf(stderr,
                        "Failed to open migration file %s: %s\n",
                        status_file,
                        strerror(errno));
                return false;
        }

        if (fprintf(f, "%d\n", level) < 0) {
                fprintf(stderr,
                        "Failed to write migration level to %s: %s\n",
                        status_file,
                        strerror(errno));
        } else {
                ret = true;
        }
        fflush(f);
        fclose(f);

        fprintf(stderr, "File written?! %s\n", status_file);
        return ret;
}

bool qol_cli_migrate(__qol_unused__ int argc, __qol_unused__ char **argv)
{
        QolContext *context = NULL;
        size_t migration_level_start = 0;
        int start_level = 0;
        bool ret = false;
        const char *trigger_file = QOL_TRIGGER_FILE;
        int n_migration_performed = 0;

        if (!qol_get_migration_level(&start_level)) {
                fprintf(stderr, "Cannot begin migration, file a bug to %s\n", PACKAGE_BUG_TRACKER);
                return false;
        } else {
                migration_level_start = (size_t)start_level;
        }

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
                if (!m->func(context)) {
                        fprintf(stderr, "Failed migration %lu: '%s'\n", i, m->name);
                        goto end;
                }
                fprintf(stdout, "Successful migration %lu: '%s'\n", i, m->name);

                /* Now update the migration level for each migration.. */
                if (!qol_set_migration_level((int)i)) {
                        fprintf(stderr,
                                "Cannot continue migration from level %lu, file a bug to %s\n",
                                i,
                                PACKAGE_BUG_TRACKER);
                        goto end;
                }
                ++n_migration_performed;
        }

        if (n_migration_performed > 0) {
                fprintf(stdout, "Performed %d migrations\n", n_migration_performed);
        } else {
                fputs("No migrations required\n", stdout);
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
