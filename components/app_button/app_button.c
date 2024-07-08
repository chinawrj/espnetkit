#include <stdio.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "app_button.h"

// 定义事件基础
ESP_EVENT_DEFINE_BASE(APP_BUTTON_EVENT_BASE);

#define TAG "APP_BUTTON"

static uint64_t last_interrupt_time = 0;
static int debounce_time_ms = 50;
static gpio_num_t button_gpio_num;

// 中断处理函数
void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint64_t current_time = esp_timer_get_time();
    if (current_time - last_interrupt_time > debounce_time_ms * 1000) {
        custom_event_post(APP_BUTTON_EVENT_BASE, APP_BUTTON_EVENT_PRESSED, NULL, 0);
        last_interrupt_time = current_time;
    }
}

esp_err_t app_button_init(gpio_num_t gpio_num, int debounce_time) {
    esp_err_t ret;

    // 设置去抖动时间
    debounce_time_ms = debounce_time;
    button_gpio_num = gpio_num;

    // 配置GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << gpio_num);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // 安装GPIO中断服务
    ret = gpio_install_isr_service(0);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGI(TAG, "GPIO ISR handler has been already installed");
            ret = ESP_OK;
        } else {
            ESP_LOGE(TAG, "GPIO ISR handler install failed: %s", esp_err_to_name(ret));
            return ret;
        }
    }

    // 添加GPIO中断处理函数
    ret = gpio_isr_handler_add(gpio_num, gpio_isr_handler, (void*) gpio_num);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add ISR handler: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

void app_button_cleanup(void) {
    gpio_isr_handler_remove(button_gpio_num);
}