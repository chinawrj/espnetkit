/*
 * wifi_app.h
 *
 *  Created on: 2024年1月12日
 *      Author: love
 */

#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_
typedef struct {
    char first;
    char flag;
    char ip_string[16]; // IP地址
    char ssid[33];      // SSID可以最长32个字符 + 1个null terminator
    int channel;        // 信道
    int rssi;           // 信号强度
    char phy_modes[128];// 物理层模式字符串
    int connected;      // 连接状态，1为已连接，0为未连接
    int phy_11b: 1;                  /**< bit: 0 flag to identify if 11b mode is enabled or not */
    int phy_11g: 1;                  /**< bit: 1 flag to identify if 11g mode is enabled or not */
    int phy_11n: 1;                  /**< bit: 2 flag to identify if 11n mode is enabled or not */
    int phy_lr: 1;                   /**< bit: 3 flag to identify if low rate is enabled or not */
    int phy_11a: 1;                  /**< bit: 4 flag to identify if 11ax mode is enabled or not */
    int phy_11ac: 1;                 /**< bit: 5 flag to identify if 11ax mode is enabled or not */
    int phy_11ax: 1;                 /**< bit: 6 flag to identify if 11ax mode is enabled or not */
    int wps: 1;                      /**< bit: 7 flag to identify if WPS is supported or not */
    int ftm_responder: 1;            /**< bit: 8 flag to identify if FTM is supported in responder mode */
    int ftm_initiator: 1;            /**< bit: 9 flag to identify if FTM is supported in initiator mode */
} MY_WIFI_INFO;

void app_wifi(void);

#endif /* COMPONENTS_WIFI_APP_INCLUDE_WIFI_APP_H_ */
