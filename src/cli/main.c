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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cli.h"
#include "config.h"
#include "util.h"

static bool print_version(__qol_unused__ int argc, __qol_unused__ char **argv)
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
        { "list-users", qol_cli_list_users, "List users on the system" },
        { "migrate", qol_cli_migrate, "Perform migration functions" },
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
        const char *binname = argv[0];

        /* Before we go anywhere, kill stdin */
        if (stdin && fileno(stdin) >= 0) {
                close(fileno(stdin));
                stdin = NULL;
        }

        if (argc < 2) {
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }

        --argc;
        ++argv;

        subcommand = argv[0];
        for (size_t i = 0; i < ARRAY_SIZE(command_table); i++) {
                SubCommand *c = &command_table[i];
                if (strcmp(c->name, subcommand) == 0) {
                        command = c;
                        break;
                }
        }

        if (!command) {
                fprintf(stderr, "Unknown command '%s'\n", subcommand);
                print_usage(binname);
                return EXIT_FAILURE;
        }

        --argc;
        ++argv;

        if (command->execute && command->execute(argc, argv)) {
                return EXIT_SUCCESS;
        }

        if (strcmp(command->name, "help") == 0 && !command->execute) {
                print_usage(binname);
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
