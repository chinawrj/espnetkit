/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void screen_main_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_main_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_main_btn_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_main_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
void events_init_screen_main(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_main, screen_main_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_main_btn_1, screen_main_btn_1_event_handler, LV_EVENT_ALL, ui);
}
static void screen_wifi_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_ethernet, guider_ui.screen_ethernet_del, &guider_ui.screen_wifi_del, setup_scr_screen_ethernet, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_wifi_btn_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_ethernet, guider_ui.screen_ethernet_del, &guider_ui.screen_wifi_del, setup_scr_screen_ethernet, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
void events_init_screen_wifi(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_wifi, screen_wifi_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_wifi_btn_1, screen_wifi_btn_1_event_handler, LV_EVENT_ALL, ui);
}
static void screen_ethernet_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_wireguard, guider_ui.screen_wireguard_del, &guider_ui.screen_ethernet_del, setup_scr_screen_wireguard, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_ethernet_btn_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_wireguard, guider_ui.screen_wireguard_del, &guider_ui.screen_ethernet_del, setup_scr_screen_wireguard, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
void events_init_screen_ethernet(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_ethernet, screen_ethernet_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_ethernet_btn_1, screen_ethernet_btn_1_event_handler, LV_EVENT_ALL, ui);
}
static void screen_wireguard_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_wireguard_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_wireguard_btn_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_wireguard_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, false);
		break;
	}
	default:
		break;
	}
}
void events_init_screen_wireguard(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_wireguard, screen_wireguard_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_wireguard_btn_1, screen_wireguard_btn_1_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}
