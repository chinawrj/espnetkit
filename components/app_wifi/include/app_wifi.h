/*
 * wifi_app.h
 *
 *  Created on: 2024年1月12日
 *      Author: love
 */

#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

typedef struct {
    char flag;
    char first;
    char ip_string[64];
} MY_WIFI_INFO;

void app_wifi(void);

#endif /* COMPONENTS_WIFI_APP_INCLUDE_WIFI_APP_H_ */
