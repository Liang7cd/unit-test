
#ifndef CMCC_PTZ_PRESET_H
#define CMCC_PTZ_PRESET_H

#include "list.h"

ListD_T ptz_preset_get_list(void);

int ptz_preset_parse_file(char *pathname, ListD_T list);

int ptz_preset_save_file(char *pathname, ListD_T list);

int ptz_preset_add(int index, int x, int y);

int ptz_preset_del(int old);

int ptz_preset_search(int index, int *x, int *y);

int ptz_preset_init(void);

int ptz_preset_deinit(void);


#endif // end of CMCC_PTZ_PRESET_H

