#include "app_wifi.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <string.h>

#include "esp_err.h"
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_timer.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define EXAMPLE_ESP_WIFI_SSID      "NETGEAR51"
#define EXAMPLE_ESP_WIFI_PASS      "tinystar287"
#define EXAMPLE_ESP_WIFI_SSID_BACKUP2      "ChatGPT"
#define EXAMPLE_ESP_WIFI_PASS_BACKUP2      "12345678"
#define EXAMPLE_ESP_WIFI_SSID_BACKUP1      "bouffalolab"
#define EXAMPLE_ESP_WIFI_PASS_BACKUP1      "bouffalolab2016"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

#define ESP_WIFI_SAE_MODE 1
#define EXAMPLE_H2E_IDENTIFIER ""
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT 1
#define WIFI_FAIL_BIT      2

static const char *TAG = "APP Wi-Fi";

static int s_retry_num = 0;

MY_WIFI_INFO my_wifi_info;

static esp_err_t wifi_reconfig(void)
{
    static int wifi_profile_backup = 0;
    wifi_config_t wifi_config;
    switch (wifi_profile_backup) {
        case 0:
        {
            wifi_profile_backup++;
            wifi_config = (wifi_config_t) {
                .sta = {
                    .ssid = EXAMPLE_ESP_WIFI_SSID,
                    .password = EXAMPLE_ESP_WIFI_PASS,
                    /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
                    * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
                    * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
                    * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                    */
                    .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                    .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
                    .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
                },
            };
        }
        break;
        case 1:
        {
            wifi_profile_backup++;
            wifi_config = (wifi_config_t) {
                .sta = {
                    .ssid = EXAMPLE_ESP_WIFI_SSID_BACKUP1,
                    .password = EXAMPLE_ESP_WIFI_PASS_BACKUP1,
                    /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
                    * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
                    * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
                    * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                    */
                    .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                    .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
                    .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
                },
            };
        }
        break;
        case 2:
        default:
        {
            wifi_profile_backup = 0;
            wifi_config = (wifi_config_t) {
                .sta = {
                    .ssid = EXAMPLE_ESP_WIFI_SSID_BACKUP2,
                    .password = EXAMPLE_ESP_WIFI_PASS_BACKUP2,
                    /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
                    * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
                    * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
                    * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                    */
                    .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                    .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
                    .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
                },
            };
        }
        break;
    }
    ESP_LOGI(TAG, "Config SSID to %s", wifi_config.sta.ssid);
    return esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

static void update_wifi_info() 
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        // 复制SSID
        strncpy(my_wifi_info.ssid, (const char *)ap_info.ssid, sizeof(my_wifi_info.ssid));
        // 更新信道
        my_wifi_info.channel = ap_info.primary;
        // 更新RSSI
        my_wifi_info.rssi = ap_info.rssi;
        // 构建PHY模式字符串
        strcpy(my_wifi_info.phy_modes, "PHY mode:");
        if (ap_info.phy_11b) strcat(my_wifi_info.phy_modes, " b");
        if (ap_info.phy_11g) strcat(my_wifi_info.phy_modes, " g");
        if (ap_info.phy_11n) strcat(my_wifi_info.phy_modes, " n");
        if (ap_info.phy_11a) strcat(my_wifi_info.phy_modes, " a");
        if (ap_info.phy_11ac) strcat(my_wifi_info.phy_modes, " ac");
        if (ap_info.phy_11ax) strcat(my_wifi_info.phy_modes, " ax");
        // 标记为已连接
        my_wifi_info.connected = 1;
    } else {
        // 如果获取失败，标记为未连接
        my_wifi_info.connected = 0;
        ESP_LOGE("WIFI", "Failed to get Wi-Fi AP info");
    }
}

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGD(TAG, "retry to connect to the AP");
        } else {
            ESP_LOGD(TAG,"connect to the AP fail");
            wifi_reconfig();
            esp_wifi_connect();
            s_retry_num = 0;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sprintf(my_wifi_info.ip_string, "%d.%d.%d.%d",IP2STR(&event->ip_info.ip));
        my_wifi_info.first = 1;
        my_wifi_info.flag = 1;
        update_wifi_info();
    }
}

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // @suppress("Symbol is not resolved")
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(wifi_reconfig());
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_LOGI(TAG, "wifi_init_sta finished.");
}
void app_wifi(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}
