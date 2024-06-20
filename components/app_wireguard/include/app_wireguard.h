#ifndef __APP_WIREGUARD_H__
#define __APP_WIREGUARD_H__
#include <stdint.h>
typedef struct {
    int flag;
    int is_up;
    uint32_t last_handshake;
    uint64_t packets_rx;
    uint64_t packets_tx;
} MY_WG_INFO;
extern MY_WG_INFO my_wg_info;

void app_register_wireguard_command(void);
void app_wireguard(void);
#endif
