#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "core/lv_disp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_console.h"
#include "esp_wifi.h"
#include "cmd_system.h"

#include "lcd.h"
#include "lv_port_disp_template.h"
#include "gui_guider.h"
#include "custom.h"

#include "portmacro.h"
#include "wifi_cmd.h"
#include "iperf_cmd.h"
#include "ping_cmd.h"

#include "app_wifi.h"
#include "app_wireguard.h"
#include "app_eth.h"
#include "app_sd_card.h"
#include "app_gpio.h"
#include "app_button.h"
#include "custom_event.h"


#define BUTTON_GPIO_1 GPIO_NUM_0

static const char *TAG = "main output";

TaskHandle_t  task_handle_gui;
esp_timer_create_args_t timer_args_gui;
esp_timer_handle_t timer_handle_gui;
lv_ui guider_ui;

extern MY_WIFI_INFO my_wifi_info;
extern MY_ETH_INFO my_eth_info;
extern MY_SDCARD_INFO my_SDcard_info;

static QueueHandle_t button_event_queue;
static lv_indev_t * indev_button;

// 硬件按钮映射到屏幕的点
static lv_point_t button_points_array[] = {
    {65, 155}, // 根据实际需要设置
};

// 输入设备读取回调函数
void input_read_callback(lv_indev_drv_t * drv, lv_indev_data_t * data) {
    static bool last_state = false;

    // 初始化为未按下状态
    data->state = LV_INDEV_STATE_REL;

    // 从队列中接收事件
    int32_t event_id;
    if (xQueueReceive(button_event_queue, &event_id, 0)) {
        if (event_id == APP_BUTTON_EVENT_PRESSED) {
            data->state = LV_INDEV_STATE_PR;
            data->btn_id = 0; // 使用按钮ID 0，映射到button_points_array的第一个点
            last_state = true;
        }
    } else {
        if (last_state) {
            data->state = LV_INDEV_STATE_REL;
            data->btn_id = 0;
            last_state = false;
        }
    }
}

// 事件处理器
void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data) {
    if (base == APP_BUTTON_EVENT_BASE && id == APP_BUTTON_EVENT_PRESSED) {
        xQueueSend(button_event_queue, &id, portMAX_DELAY);
    }
}

void app_button(void) 
{
    esp_err_t ret;

    // 初始化自定义事件模块
    ret = custom_event_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Custom event init failed: %s", esp_err_to_name(ret));
        return;
    }

    // 初始化按钮事件模块
    ret = app_button_init(BUTTON_GPIO_1, 50);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Button event init failed: %s", esp_err_to_name(ret));
        return;
    }

    // 创建队列
    button_event_queue = xQueueCreate(10, sizeof(int32_t));
    if (button_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return;
    }


    // 注册事件处理器
    ret = custom_event_register_handler(APP_BUTTON_EVENT_BASE, APP_BUTTON_EVENT_PRESSED, run_on_event, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Event handler register failed: %s", esp_err_to_name(ret));
        return;
    }
}

static void lv_tick_task(void *arg)
{
    lv_tick_inc(1);
}

static void update_ui(void);

static void lv_run_task(void *arg)
{
    TickType_t screen_show_start_time = 0;
    screen_show_start_time = xTaskGetTickCount();
    while (true) {
        lv_task_handler();
        update_ui();

        // TODO 完成点击事件的发送
        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - screen_show_start_time) * portTICK_PERIOD_MS >= 5000) {
            // 发送点击事件到当前屏幕
            //lv_event_send(lv_scr_act(), LV_EVENT_CLICKED, NULL);
            
            // 重置计时器以防止重复触发
            screen_show_start_time = current_time;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

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

// 从 ESP-IDF 获取 Wi-Fi 信息并更新 LVGL 界面
static void update_wifi_status() 
{
    char wifi_str[64];

    // Update PHY mode
    lv_label_set_text(guider_ui.screen_wifi_wifi_list_item0, my_wifi_info.phy_modes);

    // Update Channel
    snprintf(wifi_str, sizeof(wifi_str), "Channel %d", my_wifi_info.channel);
    lv_label_set_text(guider_ui.screen_wifi_wifi_list_item2, wifi_str);

    // Update RSSI
    snprintf(wifi_str, sizeof(wifi_str), "RSSI %d", my_wifi_info.rssi);
    lv_label_set_text(guider_ui.screen_wifi_wifi_list_item3, wifi_str);

    // Update SSID
    snprintf(wifi_str, sizeof(wifi_str), "SSID %s", my_wifi_info.ssid);
    lv_label_set_text(guider_ui.screen_wifi_wifi_list_item4, wifi_str);

    // Update date time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(wifi_str, sizeof(wifi_str), "%Y/%m/%d %H:%M:%S", &timeinfo);

    // 获取按钮上的标签
    lv_obj_t *label = lv_obj_get_child(guider_ui.screen_wifi_btn_1, 0);
    if (label != NULL) {
        lv_label_set_text(label, wifi_str);
    }
}

// 获取并打印当前日期和时间
static void print_current_date(void *pvParameter)
{
    while (1) {
        time_t now;
        struct tm timeinfo;
        
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year < (2016 - 1900)) {
            ESP_LOGI(TAG, "Time not set yet.");
        } else {
            ESP_LOGI(TAG, "Current date and time: %s", asctime(&timeinfo));
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS); // 每10秒打印一次
    }
}

static void update_wifi_status_overview() 
{
    if (my_wifi_info.connected) {
        my_wifi_info.connected = true;  // WiFi已连接
        int rssi = my_wifi_info.rssi;

        // 根据RSSI的强度设置不同深浅的颜色
        lv_color_t color;
        if (rssi > -50) {
            color = lv_color_hex(0x008000); // 强信号，深绿色
        } else if (rssi > -70) {
            color = lv_color_hex(0x00FF00); // 中等信号，中绿色
        } else if (rssi > -85) {
            color = lv_color_hex(0x90EE90); // 弱信号，浅绿色
        } else if (rssi > -90) {
            color = lv_color_hex(0xFFFF00); // 较差信号，黄色
        } else {
            color = lv_color_hex(0xFF0000); // 非常差信号，红色
        }

        lv_label_set_text(guider_ui.screen_main_wireguard_list_item0, "Wi-Fi is UP");
        lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item0, color, LV_STATE_DEFAULT);
    } else {
        my_wifi_info.connected = false; // WiFi未连接
        lv_label_set_text(guider_ui.screen_main_wireguard_list_item0, "Wi-Fi is DOWN");
        lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item0, lv_color_hex(0x808080), LV_STATE_DEFAULT); // 使用灰色表示未连接
    }
}

static void update_ui(void)
{
    if (lv_scr_act() == guider_ui.screen_main) {
        ESP_LOGD(TAG, "---> update screen main");
        time_t now;
        struct tm timeinfo;
        char time_str[64];

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", &timeinfo);

        // 获取按钮上的标签
        lv_obj_t *label = lv_obj_get_child(guider_ui.screen_main_btn_1, 0);
        if (label != NULL) {
            lv_label_set_text(label, time_str);
        }
        //update Wi-Fi
        update_wifi_status_overview();
        //update ETH
        if (my_eth_info.is_up) {
            lv_label_set_text(guider_ui.screen_main_wireguard_list_item1, "Ethernet is Up");
            lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item1, lv_color_hex(0x90EE90), LV_STATE_DEFAULT); // Light Green
        } else {
            lv_label_set_text(guider_ui.screen_main_wireguard_list_item1, "Ethernet is Down");
            lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item1, lv_color_hex(0xFFB6C1), LV_STATE_DEFAULT); // Light Red
        }
        //update WireGuard
        if (my_wg_info.is_up) {
            lv_label_set_text(guider_ui.screen_main_wireguard_list_item2, "Wireguard is Up");
            lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item2, lv_color_hex(0x90EE90), LV_STATE_DEFAULT); // Light Green
        } else {
            lv_label_set_text(guider_ui.screen_main_wireguard_list_item2, "Wireguard is Down");
            lv_obj_set_style_bg_color(guider_ui.screen_main_wireguard_list_item2, lv_color_hex(0xFFB6C1), LV_STATE_DEFAULT); // Light Red
        }
    }

    // 检查以太网屏幕状态
    if (lv_scr_act() == guider_ui.screen_ethernet) {
        ESP_LOGD(TAG, "---> update screen ethernet");
        time_t now;
        struct tm timeinfo;
        char time_str[64];

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", &timeinfo);

        // 获取按钮上的标签
        lv_obj_t *label = lv_obj_get_child(guider_ui.screen_ethernet_btn_1, 0);
        if (label != NULL) {
            lv_label_set_text(label, time_str);
        }
    }

    if (lv_scr_act() == guider_ui.screen_wifi) {
        ESP_LOGD(TAG, "---> update screen Wi-Fi");
        update_wifi_status();
    }

    if (lv_scr_act() == guider_ui.screen_wireguard) {
        ESP_LOGD(TAG, "---> update screen Wireguard");
        time_t now;
        struct tm timeinfo;
        char time_str[64];

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M:%S", &timeinfo);

        // 获取按钮上的标签
        lv_obj_t *label = lv_obj_get_child(guider_ui.screen_wireguard_btn_1, 0);
        if (label != NULL) {
            lv_label_set_text(label, time_str);
        }
        if(my_wg_info.flag == 1){
            char wg_str[16];
            snprintf(wg_str, sizeof(wg_str), "Tx: %llu", my_wg_info.packets_tx);
            lv_label_set_text(guider_ui.screen_wireguard_wireguard_list_item0, wg_str);

            snprintf(wg_str, sizeof(wg_str), "Rx: %llu", my_wg_info.packets_rx);
            lv_label_set_text(guider_ui.screen_wireguard_wireguard_list_item1, wg_str);

            lv_label_set_text(guider_ui.screen_wireguard_wireguard_list_item3, my_wg_info.is_up ? "Link up" : "Link Down");
            my_wg_info.flag = 0;
        }
    }
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

	LCD_Init();
    lv_init();
    lv_port_disp_init();
    // 创建输入设备驱动
    app_button();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_BUTTON; // 设置为按钮设备
    indev_drv.read_cb = input_read_callback;
    indev_button = lv_indev_drv_register(&indev_drv);

    // 将硬件按钮映射到屏幕的点
    lv_indev_set_button_points(indev_button, button_points_array);

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

    setenv("TZ", "CST-8", 1);
    tzset();

    //WIFI初始化
    app_wifi();
    //ETH(W5500)以太网初始化
    app_eth();
    //Wireguard初始化
    app_wireguard();

    esp_wifi_set_bandwidth();
    esp_wifi_config_80211_tx_rate();
    
    while (true) {
        struct tm timeinfo = { 0 };
        time_t now;
        while (timeinfo.tm_year < (2016 - 1900)) {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            time(&now);
            localtime_r(&now, &timeinfo);
        }

        if (timeinfo.tm_year < (2016 - 1900)) {
            ESP_LOGE(TAG, "Failed to synchronize time.");
        } else {
            ESP_LOGI(TAG, "Time synchronized successfully.");
            ESP_LOGI(TAG, "Current date and time: %s", asctime(&timeinfo));
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
