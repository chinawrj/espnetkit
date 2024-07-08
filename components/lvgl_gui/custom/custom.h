// SPDX-License-Identifier: MIT
// Copyright 2023 NXP

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

void custom_init(lv_ui *ui);
typedef enum {
    SCREEN_NONE,
    SCREEN_MAIN,
    SCREEN_WIFI,
    SCREEN_ETHERNET,
    SCREEN_WG,
} screen_id_t;

int update_screen_id(screen_id_t id);
screen_id_t get_screen_id(void);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
