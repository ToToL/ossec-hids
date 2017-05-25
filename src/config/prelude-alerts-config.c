/* Copyright (C) 2009 Trend Micro Inc.
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#include "shared.h"
#include "prelude-alerts-config.h"
#include "config.h"

int Read_PreludeAlerts(XML_NODE node, __attribute__((unused)) void *configp, void *preludep)
{
    int i = 0;

    /* XML definitions */
    const char *xml_prelude = "enable";
    const char *xml_profile = "profile";
    const char *xml_log_level = "log_level";

    PreludeConfig *Prelude;
    Prelude = (PreludeConfig *)preludep;
    if (!Prelude) {
        return (0);
    }

    while (node[i]) {
        if (!node[i]->element) {
            merror(XML_ELEMNULL, __local_name);
            return (OS_INVALID);
        } else if (!node[i]->content) {
            merror(XML_VALUENULL, __local_name, node[i]->element);
            return (OS_INVALID);
        }

        /* Prelude support */
        else if (strcmp(node[i]->element, xml_prelude) == 0) {
            if (strcmp(node[i]->content, "yes") == 0) {
                Prelude->prelude = 1;
            } else if (strcmp(node[i]->content, "no") == 0) {
                Prelude->prelude = 0;
            } else {
                merror(XML_VALUEERR, __local_name, node[i]->element, node[i]->content);
                return (OS_INVALID);
            }
        } else if (strcmp(node[i]->element, xml_profile) == 0) {
            if (strlen(node[i]->content) == 0) {
                merror(XML_VALUEERR, __local_name, node[i]->element, node[i]->content);
                return (OS_INVALID);
            }
            os_realloc(Prelude->profile,
                       sizeof(char *) * (strlen(node[i]->content)+1), Prelude->profile);
            os_strdup(node[i]->content, Prelude->profile);
        } else if (strcmp(node[i]->element, xml_log_level) == 0) {
            if (!OS_StrIsNum(node[i]->content)) {
                merror(XML_VALUEERR, __local_name, node[i]->element, node[i]->content);
                return (OS_INVALID);
            }
            Prelude->log_level = (u_int8_t) atoi(node[i]->content);
        } else {
            merror(XML_INVELEM, __local_name, node[i]->element);
            return (OS_INVALID);
        }
        i++;
    }

    return (0);
}

