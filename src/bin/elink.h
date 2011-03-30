#ifndef ELINK_H
#define ELINK_H
#include <Evas.h>

typedef struct elink_obj {
	int x;
	int y;
	int id;
	char name[8];
	char retrived;	
	Evas_Object *rect;
	Evas_Object *text;
}elink_obj_t;

extern int elink_x;
extern int elink_y;

extern elink_obj_t *elink_data;
#endif
