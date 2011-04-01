#include <Eina.h>
#include "algorithm.h"
#include "elink.h"
#include "log.h"

void elink_free_func(void *user_data, void *cell_data)
{

}

void elink_algorithm_init(void)
{
	eina_init();
}

void elink_algorithm_shutdown(void)
{
	eina_shutdown();
}

int elink_algorithm_rparse_data(void)
{
	Eina_Matrixsparse *m;
	m = eina_matrixsparse_new(
		ELINKER_ROWS,
		ELINKER_COLS,
		elink_free_func,
		NULL);
	if (!m) {
		elink_err("fail to crate mavrix sparse\n");
		return -1;
	}
	elink_info("success to crate mavrix sparse\n");
	eina_matrixsparse_free(m);
}

int elink_algorithm_vertical(elink_obj_t *o, elink_obj_t *n)
{
	elink_obj_t *t;
	int i, min, max;

	elink_dbg("vertical: X(%d), o->y(%d), n->y(%d)", o->x, o->y, n->y);
	min = o->y > n->y ? n->y : o->y;
	max = o->y > n->y ? o->y : n->y;
	for (i = (min + 1); i < max; i++) {
		t = elink_data + i * elink_x + o->x;
		elink_dbg("i(%d) elink_x(%d), o->x(%d)",
			i, elink_x, o->x);
		if (!t->retrived) {
			elink_dbg("err: name(%s) t->x(%d), y(%d)",
					t->name, t->x, i);
			return -1;
		}
	}
	return 0;
}

int elink_algorithm_horizon(elink_obj_t *o, elink_obj_t *n)
{
	elink_obj_t *t;
	int i, min, max;

	elink_dbg("horizon: Y(%d), o->x(%d), n->x(%d)", o->y, o->x, n->x);
	min = o->x > n->x ? n->x : o->x;
	max = o->x > n->x ? o->x : n->x;
	for (i = min + 1; i < max; i++) {
		t = elink_data + o->y * elink_x + i;
		if (!t->retrived) {
			elink_dbg("x(%d), o->y(%d)\n", i, o->y);
			return -1;
		}
	}
	return 0;
}

int elink_algorithm_one_line(elink_obj_t *o, elink_obj_t *n, int no_check)
{
	if ((o->x != n->x) && (o->y != n->y))
		return -1;

	if ((o->x == n->x) &&
		!elink_algorithm_vertical(o, n))
		return 0;

	if ((o->y == n->y) &&
		!elink_algorithm_horizon(o, n))
		return 0;
	return -1;
}

int elink_algorithm_one_corner(elink_obj_t *o, elink_obj_t *n)
{
	elink_obj_t *t;

	t = elink_data + o->y * elink_x + n->x;

	if (t->retrived &&
		!elink_algorithm_one_line(o, t, 1) &&
		!elink_algorithm_one_line(n, t, 1)) {
		return 0;
	}

	t = elink_data + n->y * elink_x + o->x;

	if (t->retrived &&
		!elink_algorithm_one_line(o, t, 1) &&
		!elink_algorithm_one_line(n, t, 1)) {
		o->retrived = 1;
		n->retrived = 1;
		return 0;
	}
	return -1;
}

int elink_algorithm_two_corner(elink_obj_t *o, elink_obj_t *n)
{
	int i;
	elink_obj_t *t;

	/* 0 to o->x */
	for (i = o->x - 1; i >= 0; i--) {
		t = elink_data + o->y * elink_x + i;
		if (!t->retrived)
			break;
		if (!elink_algorithm_one_corner(t, n))
			return 0;
	}
	/* o->x to elink_x */
	for (i = o->x + 1; i < elink_x; i++) {
		t = elink_data + o->y * elink_x + i;
		if (!t->retrived)
			break;
		if (!elink_algorithm_one_corner(t, n))
			return 0;
	}
	/* o->y to elink_y */
	for (i = o->y - 1; i >= 0; i--) {
		t = elink_data + i * elink_x + o->x;
		if (!t->retrived)
			break;
		if (!elink_algorithm_one_corner(t, n))
			return 0;
	}

	/* o->y to elink_y*/
	for (i = o->y + 1; i < elink_y; i++) {
		t = elink_data + i * elink_x + o->x;
		if (!t->retrived)
			break;
		if (!elink_algorithm_one_corner(t, n))
			return 0;
	}

	return -1;
}

int elink_algorithm_all(elink_obj_t *o, elink_obj_t *n)
{
	int ret = -1;

	if (!o->id || !n->id)
		return -1;

	if (o->id != n->id)
		return -1;

	if ((o->retrived) || (n->retrived))
		return -1;

	elink_dbg("elink_algorithm all enter");
	if (!elink_algorithm_one_line(o, n, 0))
		ret = 0;
	else if (!elink_algorithm_one_corner(o, n))
		ret = 0;
	else if (!elink_algorithm_two_corner(o, n))
		ret = 0;
	if (!ret) {
		o->retrived = 1;
		n->retrived = 1;
	}
	return ret;
}
