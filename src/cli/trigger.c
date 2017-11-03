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
#include "util.h"

static bool touch_file(const char *p)
{
        FILE *f = NULL;

        f = fopen(p, "w");
        if (!f) {
                return false;
        }
        fclose(f);
        return true;
}

bool qol_cli_trigger(__qol_unused__ int argc, __qol_unused__ char **argv)
{
        const char *trigger_dir = QOL_TRACK_DIR;
        const char *trigger_file = QOL_TRIGGER_FILE;

        if (geteuid() != 0 || getegid() != 0) {
                fprintf(stderr, "This command must be run with root privileges.\n");
                return false;
        }

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

        /* Now write a trigger file */
        if (!qol_file_exists(trigger_file) && !touch_file(trigger_file)) {
                fprintf(stderr,
                        "Failed to construct trigger file %s: %s\n",
                        trigger_file,
                        strerror(errno));
                return false;
        }

        fprintf(stdout, "Migration will run on next boot\n");

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
