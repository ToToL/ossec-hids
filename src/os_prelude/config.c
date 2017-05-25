/* Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#include "shared.h"
#include "config/config.h"
#include "prelude.h"


/* Read the Prelude configuration */
int PreludeConf(const char *cfgfile, PreludeConfig *Prelude)
{
    int modules = 0;

    modules |= CPRELUDE;

    Prelude->prelude = 0;
    Prelude->profile = NULL;
    Prelude->log_level = 0;

    if (ReadConfig(modules, cfgfile, NULL, Prelude) < 0) {
        return (OS_INVALID);
    }

    return (0);
}

