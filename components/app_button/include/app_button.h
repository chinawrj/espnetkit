#ifndef __APP_BUTTON_H__
#define __APP_BUTTON_H__

#include "custom_event.h"
#include "driver/gpio.h"

// 定义事件基础
ESP_EVENT_DECLARE_BASE(APP_BUTTON_EVENT_BASE);

// 定义事件ID
#define APP_BUTTON_EVENT_PRESSED 1

// 初始化按钮事件模块
esp_err_t app_button_init(gpio_num_t gpio_num, int debounce_time_ms);

// 清理按钮事件模块
void app_button_cleanup(void);

#endif // APP_BUTTON_H
