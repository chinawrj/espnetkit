#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_console.h"
#include "cmd_system.h"

#include "lcd.h"
#include "lv_port_disp_template.h"
#include "gui_guider.h"
#include "custom.h"

#include "wifi_cmd.h"
#include "iperf_cmd.h"
#include "ping_cmd.h"

#include "app_wifi.h"
#include "app_wireguard.h"
#include "app_eth.h"
#include "app_sd_card.h"
#include "app_gpio.h"

static const char *TAG = "main output";

TaskHandle_t  task_handle_gui;
esp_timer_create_args_t timer_args_gui;
esp_timer_handle_t timer_handle_gui;
lv_ui guider_ui;

extern MY_WIFI_INFO my_wifi_info;
extern MY_ETH_INFO my_eth_info;
extern MY_SDCARD_INFO my_SDcard_info;

static void lv_tick_task(void *arg)
{
    lv_tick_inc(1);
}
void lv_run_task(void *arg)
{
    while (true) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static int app_wireguard_started = 0;

static void init_cli(void)
{
       esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "iperf>";

    // init console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    /* Register commands */
    register_system();
    app_register_all_wifi_commands();
    app_register_iperf_commands();
    app_register_wireguard_command();


    printf("\n ==================================================\n");
    printf(" |       Steps to test Network throughput            |\n");
    printf(" |                                                |\n");
    printf(" |  1. Print 'help' to gain overview of commands  |\n");
    printf(" |  2. Configure device to station or soft-AP     |\n");
    printf(" |  3. Setup WiFi connection                      |\n");
    printf(" |  4. Run iperf to test UDP/TCP RX/TX throughput |\n");
    printf(" |                                                |\n");
    printf(" =================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
void app_main(void)
{
	LCD_Init();
    lv_init();
    lv_port_disp_init();
    setup_ui(&guider_ui);
    timer_args_gui.callback = &lv_tick_task;
    timer_args_gui.name = "periodic_gui";
    ESP_ERROR_CHECK(esp_timer_create(&timer_args_gui, &timer_handle_gui));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_gui, 1 * 1000));

    init_cli();

    BaseType_t status = xTaskCreate(lv_run_task, "lv_run_task", 4096, NULL, 2, &task_handle_gui);
    if (status == pdPASS) {
        ESP_LOGI(TAG,"Display xTaskCreate OK!");
    }
    //WIFI初始化
    app_wifi();
    //ETH(W5500)以太网初始化
    app_eth();
    while (true) {
        if(my_wifi_info.flag == 1){
            my_wifi_info.flag = 0;
            lv_obj_set_style_bg_color(guider_ui.counter_label_wifi_state,
                    lv_color_hex(0xff00), LV_PART_MAIN|LV_STATE_DEFAULT); // @suppress("Symbol is not resolved")
            lv_label_set_text(guider_ui.counter_label_wifi_state, "WIFI状态:OK");
            lv_label_set_text(guider_ui.counter_label_wifi_ip, my_wifi_info.ip_string);
        }
        if(my_eth_info.flag == 1){
            my_eth_info.flag = 0;
            lv_obj_set_style_bg_color(guider_ui.counter_label_eth_state,
                    lv_color_hex(0xff00), LV_PART_MAIN|LV_STATE_DEFAULT);// @suppress("Symbol is not resolved")
            lv_label_set_text(guider_ui.counter_label_eth_state, "ETH状态:OK");
            lv_label_set_text(guider_ui.counter_label_eth_ip, my_eth_info.ip_string);
            if (0 == app_wireguard_started) {
                app_wireguard();
            }
        }
        if(my_wg_info.flag == 1){
            char wg_str[16];
            lv_obj_set_style_bg_color(guider_ui.counter_label_tf_state,
                    lv_color_hex(0xff00), LV_PART_MAIN|LV_STATE_DEFAULT);// @suppress("Symbol is not resolved")
            lv_label_set_text(guider_ui.counter_label_tf_state, my_wg_info.is_up ? "WireGuard up" : "WireGuard down");
            snprintf(wg_str, sizeof(wg_str) - 1, "R:%llu", my_wg_info.packets_rx);
            lv_label_set_text(guider_ui.counter_label_tf_rw, wg_str);
            my_wg_info.flag = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
