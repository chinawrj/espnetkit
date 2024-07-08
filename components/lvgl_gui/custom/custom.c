/**
 * @file custom.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "gui_guider.h"
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

static screen_id_t screen_id = SCREEN_NONE;

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

int update_screen_id(screen_id_t id)
{
    switch (id) {
        case SCREEN_NONE:
        {
            switch (screen_id) {
                case SCREEN_NONE:
                {
                    screen_id = SCREEN_MAIN;
                    setup_scr_screen_main(&guider_ui);
                }
                break;
                case SCREEN_MAIN:
                {
                    screen_id = SCREEN_WIFI;
                    setup_scr_screen_wifi(&guider_ui);
                }
                break;
                case SCREEN_WIFI:
                {
                    screen_id = SCREEN_ETHERNET;
                    setup_scr_screen_ethernet(&guider_ui);
                }
                break;
                case SCREEN_ETHERNET:
                {
                    screen_id = SCREEN_WG;
                    setup_scr_screen_wireguard(&guider_ui);
                }
                break;
                case SCREEN_WG:
                {
                    screen_id = SCREEN_MAIN;
                    setup_scr_screen_main(&guider_ui);
                }
                break;
                default:
                {
                    screen_id = SCREEN_MAIN;
                    setup_scr_screen_main(&guider_ui);
                }
            }
        }
        break;
        case SCREEN_MAIN:
        case SCREEN_WIFI:
        case SCREEN_ETHERNET:
        case SCREEN_WG:
        default:
        {
            screen_id = id;
        }
    }
    return 0;
}

screen_id_t get_screen_id(void)
{
    return screen_id;
}