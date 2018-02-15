/*
 * This file is part of qol-assist.
 *
 * Copyright © 2017-2018 Solus Project
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
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/**
 * This is a bit gross, but basically we read the file twice..
 * Once to get the size, and a second time to copy.
 *
 * This ensures we still use glibc getusershell but is much cheaper than
 * trying to get the shells for every single user.
 */
char **qol_get_shells(size_t *n_shells)
{
        char **ret = NULL;
        size_t shells = 0;
        size_t i = 0;
        char *s = NULL;

        *n_shells = 0;

        setusershell();
        while (getusershell()) {
                ++shells;
        }
        endusershell();

        /* Allocate root array */
        ret = calloc(shells, sizeof(char *));
        if (!ret) {
                return NULL;
        }

        setusershell();
        while ((s = getusershell())) {
                char *tmp = strdup(s);
                if (!tmp) {
                        shells = i;
                        endusershell();
                        goto failed;
                }
                ret[i] = tmp;
                ++i;
        }
        endusershell();

        *n_shells = shells;
        return ret;

failed:
        qol_free_stringv(ret, shells);
        return NULL;
}

void qol_free_stringv(char **strv, size_t n_str)
{
        if (!strv) {
                return;
        }
        for (size_t i = 0; i < n_str; i++) {
                if (strv[i]) {
                        free(strv[i]);
                }
        }
        free(strv);
}

bool qol_exec_command(char **command)
{
        pid_t p;
        int status = 0;
        int ret = 0;
        int r = -1;

        if ((p = fork()) < 0) {
                fprintf(stderr, "Failed to fork(): %s\n", strerror(errno));
                return false;
        } else if (p == 0) {
                /* Execute the command */
                if ((r = execv(command[0], command)) != 0) {
                        fprintf(stderr, "Failed to execve(%s): %s\n", command[0], strerror(errno));
                        exit(EXIT_FAILURE);
                }
                /* We'd never actually get here. */
        } else {
                if (waitpid(p, &status, 0) < 0) {
                        fprintf(stderr, "Failed to waitpid(%d): %s\n", (int)p, strerror(errno));
                        return false;
                }
                /* i.e. sigsev */
                if (!WIFEXITED(status)) {
                        fprintf(stderr, "Child process '%s' exited abnormally\n", command[0]);
                }
        }

        /* At this point just make sure the return code was 0 */
        ret = WEXITSTATUS(status);
        if (ret != 0) {
                fprintf(stderr, "'%s' failed with exit code %d\n", command[0], r);
                return false;
        }

        /* Hunky dory. */
        return true;
}

bool qol_file_exists(const char *path)
{
        __qol_unused__ struct stat st = { 0 };
        return lstat(path, &st) == 0;
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
