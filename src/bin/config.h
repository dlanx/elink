#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include <Elementary.h>
#include <Eina.h>

#undef ELINK_DEBUG
#define ELINK_SHOW_TEXT
#define ELINK_NAME "Elinker Game"

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define P(level, fmt, arg...)                       \
    do {                                            \
            fprintf(stderr, level fmt, ##arg);      \
    } while(0)

#ifdef ELINK_DEBUG
#define D(fmt, arg...) fprintf(stderr, "D: "fmt, ##arg);
#else
#define D(arg...)
#endif
#define E(fmt, arg...) P("E: ", fmt, ##arg);
#define I(fmt, arg...) P("I: ", fmt, ##arg);

#define S(fmt, arg...) P("I: ", fmt, ##arg);

#endif
