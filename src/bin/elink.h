#ifndef ELINK_H
#define ELINK_H

typedef struct elink_obj {
	int x;
	int y;
	int id;
	char retrived;	
	Evas_Object *rect;
	Evas_Object *text;
}elink_obj_t;

#endif
