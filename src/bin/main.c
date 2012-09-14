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
Evas_Object *win;

Eina_List *elink_list[NUM_OF_IMAGES + 1];

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

int elink_obj_alloc(void)
{
	elink_data = (elink_obj_t *) malloc(elink_x * elink_y
		* sizeof(elink_obj_t));

	if (!elink_data)
		return -1;

	memset(elink_data , 0, elink_x * elink_y
		* sizeof(elink_obj_t));
	return 0;
}

int elink_debug_last_item(void)
{
	static int i = 0;
	elink_obj_t *e;
	e = elink_data + elink_y * elink_x - 1;
	elink_dbg("%d: last item id (%d), count(%d)\n", i++, e->id,
		eina_list_count(elink_list[NUM_OF_IMAGES]));
}

int elink_obj_init(Evas *ev)
{
	int i, j;
	elink_obj_t *e;
	Eina_List *t, *l;

	memset(elink_list, 0, (NUM_OF_IMAGES + 1) * sizeof(Eina_List *));

	for (i=0; i < elink_y; i++) {
		for (j=0; j < elink_x; j++) {
			e = elink_data + i * elink_x + j;
			e->x = j; e->y = i;
			if ((i == 0) || (j == 0)
				|| (i == (elink_y - 1))
				|| (j == (elink_x - 1))) {
				e->id = 0;
				e->retrived = -1;
			} else {
				e->id = random() % NUM_OF_IMAGES + 1;
				elink_list[e->id]=
					eina_list_append(elink_list[e->id], e);
				if (eina_error_get()) {
					elink_err("Memory Low, Fail alloc List");
				}
			}
		}
	}
	for (i = 1, t = NULL; i <= NUM_OF_IMAGES; i++) {
		if (eina_list_count(elink_list[i]) & 0x1) {
			if (!t) {
				t = eina_list_last(elink_list[i]);
				/* save data before free */
				e = eina_list_data_get(t);
				elink_info("odd list index one %d, id(%d)",
					i, e->id);
				elink_list[i]=
					eina_list_remove_list(elink_list[i], t);
			} else {
				elink_info("ajust id from %d, to %d", e->id, i);
				e->id = i;

				elink_list[i] =
					eina_list_append(elink_list[i], e);
				t = NULL;
			}
		}
	}

	for (i=0; i < elink_y; i++) {
		for (j=0; j < elink_x; j++) {
			e = elink_data + i * elink_x + j;
			if (e->id) {
				elink_object_rect_create(ev, e);
				elink_object_image_setup(ev, e);
#ifdef ELINK_SHOW_TEXT
				elink_object_text_set(ev, e);
#endif
			}
		}
	}
}

int elink_show_links_data(void)
{
	int i;
	elink_obj_t *e;
	Eina_List *l;

	elink_dbg("walk through all link data");
	for (i = 0; i< (NUM_OF_IMAGES + 1); i++) {
		elink_dbg("icon type: %d, list count(%d)\n",
			i, eina_list_count(elink_list[i]));
		EINA_LIST_FOREACH(elink_list[i], l, e) {
			elink_dbg("\t name(%s) id(%d)\n", e->name, e->id);
		}
	}
}

int elink_obj_destroy(void)
{
	free(elink_data);
}

int elink_obj_remove(elink_obj_t *o)
{
	elink_list[o->id] = eina_list_remove(elink_list[o->id], o);
	evas_object_del(o->rect);
	o->rect = NULL;
	evas_object_del(o->img);
	o->img = NULL;
#ifdef ELINK_SHOW_TEXT
	evas_object_del(o->text);
	o->text = NULL;
#endif
}

static void
elink_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elink_obj_t *en = (elink_obj_t *)data;
	if (!es) {
		es = en;
		evas_object_color_set(es->img, 100, 100, 100, 255);
		return;
	}
	if (es != en && !elink_algorithm_all(es, en)) {
		elink_obj_remove(es);
		elink_obj_remove(en);
	} else {
		/* change color back */
		evas_object_color_set(es->img, 255, 255, 255, 255);
	}

	es = NULL;
}

static void
elink_img_key_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elink_obj_t *o = (elink_obj_t *) data;
	elink_object_image_change(o);
	elink_dbg("elink image key up x(%x) y(%d)", o->x, o->y);
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

	for(i = 1; i <= NUM_OF_IMAGES; i++)
		elink_dbg("elink_list: index(%d) count(%d)\n", i,
			eina_list_count(elink_list[i]));
}

static void
elink_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Event_Key_Down *ev = event_info;

	if (!strcmp(ev->key, "q")) {
		elink_info("get key 'q', exit program\n");
		elm_exit();
	}

	if (!strcmp(ev->key, "d"))
		elink_show_links_data();

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

#ifdef ELINK_SHOW_TEXT
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
#endif

int elink_object_image_change(elink_obj_t *e)
{
	Evas_Object *o = e->img;
	char buf[128];

	snprintf(buf, sizeof(buf), "data/images/icon_%02d.png", 12);
	evas_object_image_file_set(o, buf, NULL);

	evas_object_image_fill_set(o, 0, 0, WIDTH, HEIGHT);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
}

int elink_object_image_setup(Evas *ev, elink_obj_t *e)
{
	Evas_Object *o;
	char buf[128];

	o = evas_object_image_add(ev);
	evas_object_move(o, e->x * WIDTH, e->y * HEIGHT);
	evas_object_resize(o, WIDTH, HEIGHT);
	evas_object_layer_set(o, 12);
	evas_object_color_set(o, 255, 255, 255, 255);

	snprintf(buf, sizeof(buf), "data/images/icon_%02d.png", e->id);
	evas_object_image_file_set(o, buf, NULL);

	evas_object_image_fill_set(o, 0, 0, WIDTH, HEIGHT);
	evas_object_pass_events_set(o, 1);

	evas_object_event_callback_add(
		o,
		EVAS_CALLBACK_KEY_UP,
		&elink_img_key_up, e);

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
	int x, y;

	o = evas_object_image_add(ev);
	evas_object_move(o, 0, 0);
	evas_object_layer_set(o, 12);
	evas_object_color_set(o, 255, 255, 255, 255);
	snprintf(buf, sizeof(buf), "data/images/bg.jpg");
	evas_object_image_file_set(o, buf, NULL);
	evas_object_image_size_get(o, &x, &y);

	elink_x = (x + WIDTH) / WIDTH + 2;
	elink_y = (y + HEIGHT) / HEIGHT + 2;

	evas_object_resize(o, WIDTH * elink_x, HEIGHT * elink_y);
	evas_object_resize(win, WIDTH * elink_x, HEIGHT * elink_y);

	evas_object_image_fill_set(o, 0, 0, WIDTH * elink_x, HEIGHT * elink_y);

	elink_info("back ground image w(%d) h(%d), render to x(%d) y(%d)\n",
		x, y, WIDTH * elink_x, HEIGHT * elink_y);

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
	Evas *ev;
	elink_obj_t *e;
	int i, j, ret = 0;
	if (argc > 2) {
		elink_x = atoi(argv[1]);
		elink_y = atoi(argv[2]);
	}

	elink_algorithm_init();
	elink_log_init();
	win = elm_win_add(NULL, "main", ELM_WIN_BASIC);

	elm_win_title_set(win, ELINK_NAME);

	evas_object_focus_set(win, 1);

	evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN,
		&elink_keydown, NULL);

	evas_object_smart_callback_add(win, "delete,request",
		elink_win_del, NULL);

	ev = evas_object_evas_get(win);

	evas_image_cache_set(ev, 3 * 1024 * 1024);
	elink_object_bg_setup(ev);
	if (elink_obj_alloc())
		goto out;

	evas_output_size_set(ev, elink_x, elink_y);
	evas_output_viewport_set(ev, 0, 0, elink_x, elink_y);

	elink_obj_init(ev);

	evas_object_show(win);

	ecore_timer_add(1, &elink_timer, NULL);

	elm_run();
	elink_obj_destroy();
out:
	elink_log_shutdown();
	elink_algorithm_shutdown();
	elm_shutdown();
	return ret;
}

ELM_MAIN()
