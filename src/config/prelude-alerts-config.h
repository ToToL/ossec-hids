/* Copyright (C) 2017 Thomas Andrejak.
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#ifndef _PRELUDE_CONFIG__H
#define _PRELUDE_CONFIG__H

#include "shared.h"

/* Prelude config structure */
typedef struct _PreludeConfig {
    u_int8_t prelude;
    u_int8_t log_level;
    char *profile;
} PreludeConfig;

#endif /* _PRELUDE_CONFIG__H */

