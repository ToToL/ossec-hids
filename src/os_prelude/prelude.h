/* Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "shared.h"
#include "config/config.h"
#include "config/prelude-alerts-config.h"

#ifndef _PRELUDE__H
#define _PRELUDE__H

#define PRELUDE_DEFAULT_ANALYZER_NAME "OSSEC"
#define PRELUDE_ANALYZER_CLASS "Host IDS, File Integrity Checker, Log Analyzer"
#define PRELUDE_ANALYZER_MODEL "Ossec"
#define PRELUDE_ANALYZER_MANUFACTURER __site
#define PRELUDE_ANALYZER_VERSION __version
#define PRELUDE_CLIENT_MAX_TRIES 5
#define PRELUDE_USER "root"

/* Infinity loop */
void OS_PreludeRun(PreludeConfig *prelude) __attribute__((nonnull)) __attribute__((noreturn));

/* ossec-prelude help */
void help_prelude(void) __attribute__((noreturn));

/* Start Prelude client */
void prelude_start(const char *profile, int argc, char **argv);

/* Log to Prelude */
void OS_PreludeLog(alert_data *lf);

#endif
