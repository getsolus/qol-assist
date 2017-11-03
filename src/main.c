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

/**
 * Basic CLI utilities
 */
typedef struct SubCommand {
        const char *name; /**<Name of this subcommand */
        bool (*execute)(void);
        const char *short_desc; /**<Short description to display */
} SubCommand;

/**
 * Dummy code to validate the user management API
 */
static bool list_users(void)
{
        QolContext *context = NULL;
        QolUser *user = NULL;

        context = qol_context_new();
        if (!context) {
                fprintf(stderr, "Failed to construct QolContext: %s\n", strerror(errno));
                return false;
        }

        /* Wind the user list in reverse */
        for (user = context->user_manager->users; user; user = user->next) {
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

        qol_context_free(context);
        return true;
}

static bool perform_migration(void)
{
        QolContext *context = NULL;
        size_t migration_level_start = 0;
        bool ret = false;

        /* Before we go anywhere, kill stdin */
        if (stdin && fileno(stdin) >= 0) {
                close(fileno(stdin));
                stdin = NULL;
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

        ret = true;

end:
        qol_context_free(context);
        return ret;
}

static bool print_version(void)
{
        fputs(PACKAGE_NAME " version " PACKAGE_VERSION "\n\n", stdout);
        fputs("Copyright © 2017 Solus Project\n\n", stdout);
        fputs(PACKAGE_NAME
              " "
              "is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n",
              stdout);

        return true;
}

/**
 * Our known commands
 */
SubCommand command_table[] = {
        { "list-admin", list_users, "List administrative users on the system" },
        { "migrate", perform_migration, "Perform migration functions" },
        { "help", NULL, "Print this help message" },
        { "version", print_version, "Print the program version and exit" },
};

static void print_usage(const char *binname)
{
        fprintf(stderr, "%s: usage: \n\n", binname);

        for (size_t i = 0; i < ARRAY_SIZE(command_table); i++) {
                SubCommand *command = &command_table[i];

                fprintf(stdout, "%*s - %s\n", 20, command->name, command->short_desc);
        }
}

int main(__qol_unused__ int argc, __qol_unused__ char **argv)
{
        const char *subcommand = NULL;
        SubCommand *command = NULL;

        if (argc != 2) {
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }

        subcommand = argv[1];
        for (size_t i = 0; i < ARRAY_SIZE(command_table); i++) {
                SubCommand *c = &command_table[i];
                if (strcmp(c->name, subcommand) == 0) {
                        command = c;
                        break;
                }
        }

        if (!command) {
                fprintf(stderr, "Unknown command '%s'\n", subcommand);
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }

        if (command->execute && command->execute()) {
                return EXIT_SUCCESS;
        }

        if (strcmp(command->name, "help") == 0 && !command->execute) {
                print_usage(argv[0]);
                return EXIT_SUCCESS;
        }

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
