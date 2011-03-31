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

/* return value:
 * 0 for success
 * other for success
 */
int elink_algorithm_one_line(elink_obj_t *o, elink_obj_t *n, int no_check)
{
	elink_obj_t *t;
	int i, min, max;
#if 0
	if (o->id != n->id)
		return -1;
#endif
	if (!no_check) {
		if ((o->retrived) || (n->retrived))
			return -1;
	}

	if ((o->x == n->x) && (o->y == n->y))
		return -1;

	if ((o->x != n->x) && (o->y != n->y))
		return -1;

	if (o->x == n->x) {
		elink_dbg("X: o->y(%d), n->y(%d)", o->y, n->y);
		if (o->y > n->y) {
			min = n->y;
			max = o->y;
		} else {
			min = o->y;
			max = n->y;
		}
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
	}

	if (o->y == n->y) {
		elink_dbg("Y: o->x(%d), n->x(%d)", o->x, n->x);
		if (o->x > n->x) {
			min = n->x;
			max = o->x;
		} else {
			min = o->x;
			max = n->x;
		}
		for (i = min + 1; i < max; i++) {
			t = elink_data + o->y * elink_x + i;
			if (!t->retrived) {
				elink_dbg("x(%d), o->y(%d)\n", i, o->y);
				return -1;
			}
		}
	}

	if (!no_check) {
		o->retrived = 1;
		n->retrived = 1;
	}
	return 0;
}

int elink_algorithm_one_corner(elink_obj_t *o, elink_obj_t *n)
{
	elink_obj_t *t;

	if ((o->retrived) || (n->retrived))
		return -1;

	if ((o->x == n->x) || (o->y == n->y))
		return -1;

	t = elink_data + o->y * elink_x + n->x;

	if (t->retrived &&
		!elink_algorithm_one_line(o, t, 1) &&
		!elink_algorithm_one_line(n, t, 1)) {
		o->retrived = 1;
		n->retrived = 1;
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
