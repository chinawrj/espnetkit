#include "custom_event.h"
#include "esp_event.h"
#include "esp_log.h"

#define TAG "CUSTOM_EVENT"

static esp_event_loop_handle_t loop_handle = NULL;

esp_err_t custom_event_init(void) {
    esp_err_t ret;

    // 配置事件循环
    esp_event_loop_args_t loop_args = {
        .queue_size = 10,
        .task_name = "custom_event_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id = tskNO_AFFINITY
    };

    ret = esp_event_loop_create(&loop_args, &loop_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Event loop create failed: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t custom_event_register_handler(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t handler, void* handler_arg) {
    return esp_event_handler_register_with(loop_handle, event_base, event_id, handler, handler_arg);
}

esp_err_t custom_event_unregister_handler(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t handler) {
    return esp_event_handler_unregister_with(loop_handle, event_base, event_id, handler);
}

esp_err_t custom_event_post(esp_event_base_t event_base, int32_t event_id, void* event_data, size_t event_data_size) {
    return esp_event_post_to(loop_handle, event_base, event_id, event_data, event_data_size, portMAX_DELAY);
}

void custom_event_cleanup(void) {
    if (loop_handle) {
        esp_event_loop_delete(loop_handle);
        loop_handle = NULL;
    }
}
