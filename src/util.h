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

#include <sys/types.h>

/**
 * Attempt to grab all system shells
 *
 * @note Free with qol_free_stringv
 *
 * @param n_shells Pointer to store the number of shell entries in
 * @returns Allocated array of allocated strings
 */
char **qol_get_shells(size_t *n_shells);

/**
 * Free an array of strings
 *
 * @param strv Pointer to the string array
 * @param n_str Number of entries in the list
 */
void qol_free_stringv(char **strv, size_t n_str);

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
