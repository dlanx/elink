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
	Evas_Object *o = (Evas_Object *)data;

	elink_dbg("mouse down, catch\n");
	evas_object_color_set(o, 0, 0, 0, 0);
	evas_object_show(o);
}

static void
elink_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elink_info("mouse wheel, catch\n");
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

int elink_object_rect_create(Evas *ev, elink_obj_t *eo)
{
	eo->rect = evas_object_rectangle_add(ev);

	evas_object_event_callback_add(eo->rect,
		EVAS_CALLBACK_MOUSE_DOWN,
		&elink_mouse_down,
		eo->rect);

	evas_object_event_callback_add(eo->rect,
		EVAS_CALLBACK_MOUSE_WHEEL,
		&elink_mouse_wheel,
		eo->rect);

	evas_object_resize(eo->rect, RWIDTH, RHEIGHT);
	evas_object_move(eo->rect, RWIDTH * eo->x, RHEIGHT * eo->y);

	evas_object_color_set(eo->rect, 20 * eo->x, 20 * eo->y,
		13 * eo->x + 5 * eo->y, 255);
	evas_object_show(eo->rect);

	/* create text */
	elink_object_text_set(ev, eo);
}

int elink_object_text_set(Evas *ev, elink_obj_t *eo)
{
	Evas_Object *o;
	char buf[32];
	o = evas_object_text_add(ev);
	evas_object_layer_set(o, 10);
	evas_object_color_set(o, 255, 0, 0, 255);

	evas_object_resize(o, eo->x * RWIDTH, eo->y * RHEIGHT);

	snprintf(buf, 32, "%c.%c", 'A' + eo->x, '0' + eo->y);

	evas_object_text_text_set(o, buf);
	evas_object_text_font_set(o, "Vera", 10);
	evas_object_pass_events_set(o, 1);
	evas_object_move(o, eo->x * RWIDTH, eo->y * RHEIGHT);
	evas_object_show(o);
}

EAPI int
elm_main(int argc, char *argv[])
{
	Evas_Object *win;
	Evas *ev;
	elink_obj_t *e;
	int i, j, rw, rh, ret = 0;

	elink_alogrithm_init();
	elink_log_init();

	elink_data = (elink_obj_t *) malloc(DEFAULT_WIDTH * DEFAULT_HEIGHT
		* sizeof(elink_obj_t));

	elink_rparse_data();

	win = elm_win_add(NULL, "main", ELM_WIN_BASIC);

	elm_win_title_set(win, "Elinker Game");

	evas_object_focus_set(win, 1);

	evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN,
		&elink_keydown, NULL);
	evas_object_smart_callback_add(win, "delete,request",
		elink_win_del, NULL);

	evas_object_resize(win, WINDOW_WIDTH, WINDOW_HEIGHT);

	rw = WINDOW_WIDTH / DEFAULT_WIDTH;
	rh = WINDOW_HEIGHT / DEFAULT_HEIGHT;

	ev = evas_object_evas_get(win);

	for (i=0; i<DEFAULT_HEIGHT; i++) {
		for (j=0; j<DEFAULT_WIDTH; j++) {
			e = elink_data + i * DEFAULT_HEIGHT + j;
			e->x = i; e->y = j;
			elink_object_rect_create(ev, e);
		}
	}
	evas_object_show(win);

	ecore_timer_add(1, &elink_timer, NULL);

	elm_run();

	free(elink_data);
	elink_log_shutdown();
	elink_alogrithm_shutdown();
	elm_shutdown();
	return ret;
}

ELM_MAIN()
