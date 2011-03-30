#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#undef DEBUG_ENABLE

#define MAJOR "1"
#define MINOR "2"

#define SIZE    1024
#define LEN     512
#define MAX_REMOTES 8
#define DEFAULT_REPO "git@mipsandroid.com:mipsandroid/" /* must end of '/' */

extern char *filename;
extern FILE *fp;
#define P(level, fmt, arg...)                       \
    do {                                            \
        if (daemon_mode)                            \
            syslog(LOG_INFO, level fmt, ##arg);     \
        else                                        \
            fprintf(stderr, level fmt, ##arg);      \
    } while(0)

#ifdef DEBUG_ENABLE 
#define D(fmt, arg...) fprintf(stderr, "D: "fmt, ##arg);
#else
#define D(arg...)
#endif
#define E(fmt, arg...) P("E: ", fmt, ##arg);
#define I(fmt, arg...) P("I: ", fmt, ##arg);

#define S(fmt, arg...) P("I: ", fmt, ##arg);

#endif
