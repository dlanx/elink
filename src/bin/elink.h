#ifndef ELINK_H
#define ELINK_H
#include <Evas.h>
#include <Eina.h>

#define WIDTH 60
#define HEIGHT 60

#define DEFAULT_X 6
#define DEFAULT_Y 7

typedef struct elink_obj {
	int x;
	int y;
	int id;
	char name[8];
	char retrived;	
	Evas_Object *rect;
	Evas_Object *text;
	Evas_Object *img;
}elink_obj_t;

typedef struct elink_map {
	int count;
	elink_obj_t *o;
	Eina_List *list;
}elink_map_t;

extern int elink_x;
extern int elink_y;

extern elink_obj_t *elink_data;
#endif
