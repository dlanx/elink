#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include <Elementary.h>
#include <Eina.h>

#undef DEBUG_ENABLE

#define MAJOR "1"
#define MINOR "2"

#define SIZE    1024
#define LEN     512

#define DEFAULT_WIDTH 6
#define DEFAULT_HEIGHT 6

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 360

#define RWIDTH (WINDOW_WIDTH / DEFAULT_WIDTH)
#define RHEIGHT (WINDOW_HEIGHT / DEFAULT_HEIGHT)

extern char *filename;
extern FILE *fp;
#define P(level, fmt, arg...)                       \
    do {                                            \
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
