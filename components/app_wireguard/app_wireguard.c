#include <esp_wireguard.h>
#include <stdbool.h>
#include <time.h>
#include <inttypes.h>
#include <esp_sntp.h>
#include <esp_log.h>
#include <stdio.h>
#include "esp_console.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "app_wireguard.h"

#define CONFIG_WG_PRIVATE_KEY "WFODEnl85ICOajGwsFJVcUZpTiu86uiy8yMEkJd8s1Y="
#define CONFIG_WG_LOCAL_PORT 51820
#define CONFIG_WG_PEER_PUBLIC_KEY "nNkyUznSUY5J5KoTqnEqj44HtcKD2HaHoqkh3kjFX0Y="
#define CONFIG_WG_LOCAL_IP_ADDRESS "192.168.123.200"
#define CONFIG_WG_LOCAL_IP_NETMASK "255.255.255.0"
//#define CONFIG_WG_PEER_ADDRESS "192.168.1.1"
#define CONFIG_WG_PEER_ADDRESS "ahwrj.3322.org"
#define CONFIG_WG_PEER_PORT 50123

static const char *TAG = "wireguard_cmd";
MY_WG_INFO my_wg_info;

static void time_sync_notification_cb(struct timeval *tv)
{
	ESP_LOGI(TAG, "Time synced");
}

static void initialize_sntp(void)
{
	ESP_LOGI(TAG, "Initializing SNTP");
	esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
	esp_sntp_setservername(0, "time.apple.com");
	esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
	esp_sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
	esp_sntp_init();
}

static void obtain_time(void)
{
	int retry = 0;
	const int retry_count = 40;

	initialize_sntp();
	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
		ESP_LOGI(TAG, "Waiting for system time to be set... (%i/%i)", retry, retry_count);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

static esp_err_t wireguard_service_start(void)
{
    esp_err_t err = ESP_FAIL;

    ESP_LOGI(TAG, "WireGuard service started.");

    wireguard_config_t wg_config = ESP_WIREGUARD_CONFIG_DEFAULT();
    wireguard_ctx_t *ctx = NULL;

    wg_config.private_key = CONFIG_WG_PRIVATE_KEY;
    wg_config.listen_port = CONFIG_WG_LOCAL_PORT;
    wg_config.public_key = CONFIG_WG_PEER_PUBLIC_KEY;
    wg_config.allowed_ip = CONFIG_WG_LOCAL_IP_ADDRESS;
    wg_config.allowed_ip_mask = CONFIG_WG_LOCAL_IP_NETMASK;
    wg_config.endpoint = CONFIG_WG_PEER_ADDRESS;
    wg_config.port = CONFIG_WG_PEER_PORT;
    wg_config.persistent_keepalive = 10;

    /* If the device is behind NAT or stateful firewall, set persistent_keepalive.
    persistent_keepalive is disabled by default */
    // wg_config.persistent_keepalive = 10;

    err = esp_wireguard_init(&wg_config, &ctx);

    /* start establishing the link. after this call, esp_wireguard start
    establishing connection. */
    err = esp_wireguard_connect(ctx);
    bool peer_is_up = false;

    while (1) {
            /* after some time, see if the link is up. note that it takes some time to
            establish the link */
            err = esp_wireguardif_peer_is_up(ctx);
            if (err == ESP_OK) {
                /* the link is up */
                my_wg_info.is_up = 1;
                if (false == peer_is_up) {
                    ESP_LOGI(TAG, "connect result is OK");
                    ESP_LOGI(TAG, "Setup default route using wireguard");
                    esp_netif_set_default_netif(ctx->s_netif);
                    peer_is_up = true;
                }
            } else {
                /* the link is not up */
                my_wg_info.is_up = 0;
            }
            if (ctx->statistic_updated) {
                my_wg_info.packets_tx = ctx->packet_sent;
                my_wg_info.packets_rx = ctx->packet_received;
                my_wg_info.flag = 1;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
    }

    /* do something */
    err = esp_wireguard_disconnect(ctx);
    return err;
}

// 定义 log 标签
void wireguard_service_task(void *pvParameters) 
{
    obtain_time();
    ESP_LOGI(TAG, "WireGuard task is running.");
    wireguard_service_start();
    vTaskDelete(NULL); // 任务完成后删除任务
}

// wireguard 命令的处理函数
int do_wireguard_cmd(int argc, char **argv) 
{
    ESP_LOGI(TAG, "Executing WireGuard command...");
    BaseType_t result = xTaskCreate(
        wireguard_service_task,  // 任务函数
        "wireguard_task",        // 任务名称
        4096,                    // 任务堆栈大小
        NULL,                    // 任务参数
        5,                       // 任务优先级
        NULL                    // 任务句柄
    );

    if (result == pdPASS) {
        ESP_LOGI(TAG, "WireGuard task started successfully.");
    } else {
        ESP_LOGE(TAG, "Failed to start WireGuard task.");
    }

    return 0; // 返回成功状态
}

void app_wireguard(void) 
{
    do_wireguard_cmd(0, NULL);
}

void app_register_wireguard_command(void)
{
    // 没有参数，使用一个空的 argtable
    void *argtable = NULL;

    // 定义控制台命令
    const esp_console_cmd_t wireguard_cmd = {
        .command = "wireguard",
        .help = "Start WireGuard",
        .hint = NULL,
        .func = &do_wireguard_cmd,
        .argtable = &argtable
    };

    // 注册控制台命令
    ESP_ERROR_CHECK(esp_console_cmd_register(&wireguard_cmd));
}
