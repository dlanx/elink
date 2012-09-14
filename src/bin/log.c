#include "log.h"

int elink_log_d;
void elink_log_init(void)
{
	eina_log_level_set(EINA_LOG_LEVEL_DBG);
	elink_log_d = eina_log_domain_register("elink", EINA_COLOR_GREEN);
}

void elink_log_shutdown(void)
{
	eina_log_domain_unregister(elink_log_d);
}
