/* Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "shared.h"
#include "prelude.h"

#ifndef ARGV0
#define ARGV0 "ossec-prelude"
#endif

/* Print help statement */
void help_prelude()
{
    print_header();
    print_out("  %s: -[Vhdtf] [-u user] [-g group] [-c config] [-D dir]", ARGV0);
    print_out("    -V          Version and license message");
    print_out("    -h          This help message");
    print_out("    -d          Execute in debug mode. This parameter");
    print_out("                can be specified multiple times");
    print_out("                to increase the debug level.");
    print_out("    -t          Test configuration");
    print_out("    -f          Run in foreground");
    print_out("    -u <user>   User to run as (default: %s)", PRELUDE_USER);
    print_out("    -g <group>  Group to run as (default: %s)", GROUPGLOBAL);
    print_out("    -c <config> Configuration file to use (default: %s)", DEFAULTCPATH);
    print_out("    -D <dir>    Directory to chroot into (default: %s)", DEFAULTDIR);
    print_out(" ");
    exit(1);
}

int main(int argc, char **argv)
{
    int c, test_config = 0, run_foreground = 0;
    uid_t uid;
    gid_t gid;
    const char *dir  = DEFAULTDIR;
    const char *user = PRELUDE_USER;
    const char *group = GROUPGLOBAL;
    const char *cfg = DEFAULTCPATH;

    /* Prelude Structure */
    PreludeConfig prelude;

    /* Set the name */
    OS_SetName(ARGV0);

    while ((c = getopt(argc, argv, "Vdhtfu:g:D:c:")) != -1) {
        switch (c) {
            case 'V':
                print_version();
                break;
            case 'h':
                help_prelude();
                break;
            case 'd':
                nowDebug();
                break;
            case 'f':
                run_foreground = 1;
                break;
            case 'u':
                if (!optarg) {
                    ErrorExit("%s: -u needs an argument", ARGV0);
                }
                user = optarg;
                break;
            case 'g':
                if (!optarg) {
                    ErrorExit("%s: -g needs an argument", ARGV0);
                }
                group = optarg;
                break;
            case 'D':
                if (!optarg) {
                    ErrorExit("%s: -D needs an argument", ARGV0);
                }
                dir = optarg;
                break;
            case 'c':
                if (!optarg) {
                    ErrorExit("%s: -c needs an argument", ARGV0);
                }
                cfg = optarg;
                break;
            case 't':
                test_config = 1;
                break;
            default:
                help_prelude();
                break;
        }
    }

    /* Start daemon */
    debug1(STARTED_MSG, ARGV0);

    /* Check if the user/group given are valid */
    uid = Privsep_GetUser(user);
    gid = Privsep_GetGroup(group);
    if (uid == (uid_t) - 1 || gid == (gid_t) - 1) {
        ErrorExit(USER_ERROR, ARGV0, user, group);
    }

    /* Read configuration */
    if (PreludeConf(cfg, &prelude) < 0) {
        ErrorExit(CONFIG_ERROR, ARGV0, cfg);
    }

    /* Read internal options */
    prelude.prelude = getDefine_Int("prelude",
                                         "prelude",
                                         0, 1);

    /* Get subject type */
    prelude.log_level = getDefine_Int("prelude",
                                      "log_level",
                                      0, 1);

    /* Exit here if test config is set */
    if (test_config) {
        exit(0);
    }

    if (!run_foreground) {
        nowDaemon();
        goDaemon();
    }

    /* Privilege separation */
    if (Privsep_SetGroup(gid) < 0) {
        ErrorExit(SETGID_ERROR, ARGV0, group, errno, strerror(errno));
    }

    /* Change user */
    if (Privsep_SetUser(uid) < 0) {
        ErrorExit(SETUID_ERROR, ARGV0, user, errno, strerror(errno));
    }

    debug1(PRIVSEP_MSG, ARGV0, user);

    /* Signal manipulation */
    StartSIG(ARGV0);

    /* Create PID files */
    if (CreatePID(ARGV0, getpid()) < 0) {
        ErrorExit(PID_ERROR, ARGV0);
    }

    /* Start up message */
    verbose(STARTUP_MSG, ARGV0, (int)getpid());

    /* Connect to Prelude */
    prelude_start(prelude->profile, argc, argv);

    /* The real daemon now */
    OS_PreludeRun(&prelude);
}

/* Read the queue and send the appropriate alerts
 * Not supposed to return
 */
void OS_PreludeRun(PreludeConfig *prelude)
{
    int s = 0;
    time_t tm;
    struct tm *p;
    int tries = 0;
    file_queue *fileq;
    alert_data *al_data;

    /* Get current time before starting */
    tm = time(NULL);
    p = localtime(&tm);

    /* Initialize file queue to read the alerts */
    os_calloc(1, sizeof(file_queue), fileq);
    while ( (Init_FileQueue(fileq, p, 0) ) < 0 ) {
        tries++;
        if ( tries > PRELUDE_CLIENT_MAX_TRIES ) {
            merror("%s: ERROR: Could not open queue after %d tries, exiting!",
                   ARGV0, tries
                  );
            exit(1);
        }
        sleep(1);
    }
    debug1("%s: INFO: File queue connected.", ARGV0 );

    /* Infinite loop reading the alerts and inserting them */
    while (1) {
        tm = time(NULL);
        p = localtime(&tm);

        /* Get message if available (timeout of 5 seconds) */
        al_data = Read_FileMon(fileq, p, 5);
        if (!al_data) {
            continue;
        }

        /* Send to Prelude */
        OS_PreludeLog(al_data);

        /* Clear the memory */
        FreeAlertData(al_data);
    }

}
