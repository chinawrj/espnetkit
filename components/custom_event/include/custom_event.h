#ifndef __CUSTOM_EVENT_H__
#define __CUSTOM_EVENT_H__

#include "esp_event.h"

// 初始化自定义事件模块
esp_err_t custom_event_init(void);

// 注册事件处理器
esp_err_t custom_event_register_handler(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t handler, void* handler_arg);

// 注销事件处理器
esp_err_t custom_event_unregister_handler(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t handler);

// 发布事件
esp_err_t custom_event_post(esp_event_base_t event_base, int32_t event_id, void* event_data, size_t event_data_size);

// 清理自定义事件模块
void custom_event_cleanup(void);

#endif // CUSTOM_EVENT_H

