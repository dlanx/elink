#include <Eina.h>
#include "algorithm.h"
#include "log.h"

void elink_free_func(void *user_data, void *cell_data)
{

}

void elink_alogrithm_init(void)
{
	eina_init();
}

void elink_alogrithm_shutdown(void)
{
	eina_shutdown();
}

int elink_rparse_data(void)
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
