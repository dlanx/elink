#ifndef ELINKER_ALGORITHM_H
#define ELINKER_ALGORITHM_H

#define ELINKER_ROWS 6
#define ELINKER_COLS 6

#include "elink.h"
void elink_algorithm_init(void);
void elink_algorithm_shutdown(void);

int elink_algorithm_one_line(elink_obj_t *o, elink_obj_t *n, int no_check);
int elink_algorithm_one_corner(elink_obj_t *o, elink_obj_t *n);
#endif
