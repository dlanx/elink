#ifndef ELINKER_LOG_H
#define ELINKER_LOG_H

#include <Eina.h>

#define elink_info(arg...) \
	do { \
		EINA_LOG_DOM_INFO(elink_log_d, ##arg); \
	} while(0)

#define elink_warn(arg...) \
	do { \
		EINA_LOG_DOM_WARN(elink_log_d, ##arg); \
	} while(0)

#define elink_dbg(arg...) \
	do { \
		EINA_LOG_DOM_DBG(elink_log_d, ##arg); \
	} while(0)

#define elink_err(arg...) \
	do { \
		EINA_LOG_DOM_ERR(elink_log_d, ##arg); \
	} while(0)

extern int elink_log_d;
void elink_log_init(void);
void elink_log_shutdown(void);

#endif
