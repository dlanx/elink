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

void elink_algorithm_horizon(void)
{

}

void elink_algorithm_vertical(void)
{

}

void elink_algorithm_one_corner(void)
{

}
