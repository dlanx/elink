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
/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
Evas_Object *board[DEFAULT_WIDTH * DEFAULT_HEIGHT];
int board_width = DEFAULT_WIDTH;
int board_height = DEFAULT_HEIGHT;

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
	int i, j;
	elink_info("mouse down, catch\n");
	for (i=0; i<DEFAULT_HEIGHT; i++) {
		for (j=0; j<DEFAULT_WIDTH; j++)
			if ((int)data == (int) &board[i * DEFAULT_WIDTH + j]) {
				elink_info("found match obj i(%d) j(%d)\n",
					i, j);
				break;
			}
	}
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

EAPI int
elm_main(int argc, char *argv[])
{
	Evas_Object *win;
	Evas *ev;
	int i, j, rw, rh, ret = 0;

	elink_alogrithm_init();
	elink_log_init();

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
			Evas_Object **eo = &board[i * DEFAULT_WIDTH + j];
			*eo = evas_object_rectangle_add(ev);

			evas_object_event_callback_add(*eo,
				EVAS_CALLBACK_MOUSE_DOWN,
				&elink_mouse_down,
				&board[i * DEFAULT_WIDTH + j]);

			evas_object_event_callback_add(*eo,
				EVAS_CALLBACK_MOUSE_WHEEL,
				&elink_mouse_wheel,
				&board[i * DEFAULT_WIDTH + j]);


			evas_object_resize(*eo, rw, rh);
			evas_object_move(*eo, rw * j, rh * i);
			evas_object_color_set(*eo, 20*i, 20*j, 13*i + 5*j, 255);
			evas_object_show(*eo);
		}
	}

	evas_object_show(win);

	ecore_timer_add(1, &elink_timer, NULL);

	elm_run();

out:
	elink_log_shutdown();
	elink_alogrithm_shutdown();
	return ret;
}

ELM_MAIN()
