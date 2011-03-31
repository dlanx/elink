#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "config.h"
#include "log.h"
#include "elink.h"
/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

elink_obj_t *elink_data = NULL;
elink_obj_t *es = NULL;

int elink_x = DEFAULT_X;
int elink_y = DEFAULT_Y;

void usage(int argc, char *argv[])
{
	I("Usage: %s \n", argv[0]);
	I("     -b [destination base directory]\n");
	I("     -d [dry run]\n");
	I("     -k [keep run background]\n");
	return;
}

Eina_Bool elink_timer(void *data)
{
	return 1;
}

static void
elink_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elink_obj_t *en = (elink_obj_t *)data;
	if (!es) {
		es = en;
		return;
	}

	if (!elink_algorithm_one_line(es, en, 0) ||
		!elink_algorithm_one_corner(es, en)) {
		elink_dbg("mouse down, previous: x(%d) y(%d)\n", es->x, es->y);
		elink_dbg("mouse down, current: x(%d) y(%d)\n", en->x, en->y);
		evas_object_color_set(es->rect, 0, 0, 0, 0);
		evas_object_show(es->rect);
		evas_object_hide(es->img);

		evas_object_color_set(en->rect, 0, 0, 0, 0);
		evas_object_show(en->rect);
		evas_object_hide(en->img);
	}


	es = NULL;
}

static void
elink_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	int i, j;
	elink_obj_t *eo;
	for (i=0; i < elink_x; i++) {
		for (j=0; j < elink_y; j++) {
			eo = elink_data + i * elink_x + j;
			if (eo->retrived)
				elink_dbg("trived x(%d) y(%d)", i, j);
		}
	}

}

static void
elink_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Event_Key_Down *ev = event_info;

	if (!strcmp(ev->key, "q")) {
		elink_info("get key 'q', exit program\n");
		elm_exit();
	}
	if (!strcmp(ev->key, "space") || !strcmp(ev->key, "Down"))
		elink_dbg("do_space\n");
	else if (!strcmp(ev->key, "Left"))
		elink_dbg("move_left\n");
	else if (!strcmp(ev->key, "Right"))
		elink_dbg("move_right\n");
	else if (!strcmp(ev->key, "Up"))
		elink_dbg("do_rotate\n");
}

static void
elink_win_del(void *data, Evas_Object *obj, void *event_info)
{
	elm_exit();
}

static void elink_bg_key_down(void *data, Evas * e,
	Evas_Object * obj, void *event_info)
{
	elink_dbg("elink_bg_key_down enter");
}

int elink_object_text_set(Evas *ev, elink_obj_t *eo)
{
	Evas_Object *o;
	o = evas_object_text_add(ev);
	evas_object_layer_set(o, 10);
	evas_object_color_set(o, 255, 0, 0, 255);

	evas_object_resize(o, eo->x * WIDTH, eo->y * HEIGHT);

	snprintf(eo->name, 8, "%c.%c", 'A' + eo->x, '0' + eo->y);

	evas_object_text_text_set(o, eo->name);
	evas_object_text_font_set(o, "Vera", 10);
	evas_object_pass_events_set(o, 1);
	evas_object_move(o, eo->x * WIDTH, eo->y * HEIGHT);
	evas_object_show(o);
	eo->text = o;
}

int elink_object_image_setup(Evas *ev, elink_obj_t *e)
{
	Evas_Object *o;
	char buf[128];
	int value;

	o = evas_object_image_add(ev);
	evas_object_move(o, e->x * WIDTH, e->y * HEIGHT);
	evas_object_resize(o, WIDTH, HEIGHT);
	evas_object_layer_set(o, 12);
	evas_object_color_set(o, 255, 255, 255, 255);

	value = (e->x * elink_x + e->y) % 36;
	snprintf(buf, sizeof(buf), "data/images/icon_%02d.png",
		value);
	evas_object_image_file_set(o, buf, NULL);
	evas_object_image_fill_set(o, 0, 0, WIDTH, HEIGHT);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
	evas_object_focus_set(o, 1);
	evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN,
	elink_bg_key_down, NULL);
	e->img = o;
}

int elink_object_bg_setup(Evas *ev)
{
   Evas_Object *o;
   char buf[128];

   o = evas_object_image_add(ev);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, elink_x, elink_y);
   evas_object_layer_set(o, -999);
   evas_object_color_set(o, 255, 255, 255, 255);
   snprintf(buf, sizeof(buf), "data/images/bg.png");
   evas_object_image_file_set(o, buf, NULL);
   evas_object_image_fill_set(o, 0, 0, 128, 128);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
   evas_object_focus_set(o, 1);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN,
	elink_bg_key_down, NULL);

   o = evas_object_image_add(ev);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, WIDTH, HEIGHT);
   evas_object_layer_set(o, -999);
   evas_object_color_set(o, 255, 255, 255, 255);
   snprintf(buf, sizeof(buf), "data/images/shadow.png");
   evas_object_image_file_set(o, buf, NULL);
   evas_object_image_smooth_scale_set(o, 0);
   evas_object_image_fill_set(o, 0, 0, elink_x, elink_y);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
}

int elink_object_rect_create(Evas *ev, elink_obj_t *eo)
{
	Evas_Object *o;
	o = evas_object_rectangle_add(ev);

	evas_object_event_callback_add(
		o,
		EVAS_CALLBACK_MOUSE_DOWN,
		&elink_mouse_down,
		eo);

	evas_object_event_callback_add(
		o,
		EVAS_CALLBACK_MOUSE_WHEEL,
		&elink_mouse_wheel, eo);

	evas_object_resize(o, WIDTH, HEIGHT);
	evas_object_move(o, WIDTH * eo->x, HEIGHT * eo->y);

	evas_object_color_set(o, 20 * eo->x, 20 * eo->y,
		13 * eo->x + 5 * eo->y, 255);
	evas_object_show(o);
	eo->rect = o;
}

EAPI int
elm_main(int argc, char *argv[])
{
	Evas_Object *win;
	Evas *ev;
	elink_obj_t *e;
	int i, j, ret = 0;

	elink_data = (elink_obj_t *) malloc(elink_x * elink_y
		* sizeof(elink_obj_t));

	if (!elink_data)
		return -1;

	memset(elink_data , 0, elink_x * elink_y
		* sizeof(elink_obj_t));

	elink_algorithm_init();
	elink_log_init();
	win = elm_win_add(NULL, "main", ELM_WIN_BASIC);

	elm_win_title_set(win, "Elinker Game");

	evas_object_focus_set(win, 1);

	evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN,
		&elink_keydown, NULL);
	evas_object_smart_callback_add(win, "delete,request",
		elink_win_del, NULL);

	evas_object_resize(win, elink_x * WIDTH, elink_y * HEIGHT);

	ev = evas_object_evas_get(win);

	for (i=0; i < elink_y; i++) {
		for (j=0; j < elink_x; j++) {
			e = elink_data + i * elink_x + j;
			e->x = j; e->y = i;
			elink_object_rect_create(ev, e);
			elink_object_image_setup(ev, e);
#ifdef ELINK_SHOW_TEXT
			elink_object_text_set(ev, e);
#endif
		}
	}
	evas_object_show(win);

	ecore_timer_add(1, &elink_timer, NULL);

	elm_run();

	free(elink_data);
	elink_log_shutdown();
	elink_algorithm_shutdown();
	elm_shutdown();
	return ret;
}

ELM_MAIN()
