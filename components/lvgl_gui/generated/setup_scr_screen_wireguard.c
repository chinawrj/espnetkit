/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_wireguard(lv_ui *ui)
{
	//Write codes screen_wireguard
	ui->screen_wireguard = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen_wireguard, 130, 160);
	lv_obj_set_scrollbar_mode(ui->screen_wireguard, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_wireguard, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_wireguard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_wireguard_main_title
	ui->screen_wireguard_main_title = lv_label_create(ui->screen_wireguard);
	lv_label_set_text(ui->screen_wireguard_main_title, "ESPNetKit");
	lv_label_set_long_mode(ui->screen_wireguard_main_title, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_wireguard_main_title, 0, 0);
	lv_obj_set_size(ui->screen_wireguard_main_title, 130, 15);

	//Write style for screen_wireguard_main_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_wireguard_main_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_wireguard_main_title, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_wireguard_main_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_wireguard_main_title, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_wireguard_main_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_wireguard_main_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_wireguard_main_title, lv_color_hex(0xffab00), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_wireguard_main_title, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_wireguard_main_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_wireguard_wireguard_title
	ui->screen_wireguard_wireguard_title = lv_label_create(ui->screen_wireguard);
	lv_label_set_text(ui->screen_wireguard_wireguard_title, "Wireguard");
	lv_label_set_long_mode(ui->screen_wireguard_wireguard_title, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_wireguard_wireguard_title, 0, 13);
	lv_obj_set_size(ui->screen_wireguard_wireguard_title, 130, 14);

	//Write style for screen_wireguard_wireguard_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_wireguard_wireguard_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_wireguard_wireguard_title, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_wireguard_wireguard_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_wireguard_wireguard_title, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_wireguard_wireguard_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_wireguard_wireguard_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_wireguard_wireguard_title, lv_color_hex(0xffab00), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_wireguard_wireguard_title, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_wireguard_wireguard_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_wireguard_wireguard_list
	ui->screen_wireguard_wireguard_list = lv_list_create(ui->screen_wireguard);
	ui->screen_wireguard_wireguard_list_item0 =lv_list_add_text(ui->screen_wireguard_wireguard_list, "Tx 0");
	ui->screen_wireguard_wireguard_list_item1 =lv_list_add_text(ui->screen_wireguard_wireguard_list, "Rx 0");
	ui->screen_wireguard_wireguard_list_item2 =lv_list_add_text(ui->screen_wireguard_wireguard_list, "LastActive");
	ui->screen_wireguard_wireguard_list_item3 =lv_list_add_text(ui->screen_wireguard_wireguard_list, "Link Status");
	ui->screen_wireguard_wireguard_list_item4 =lv_list_add_text(ui->screen_wireguard_wireguard_list, "Peer No.");
	lv_obj_set_pos(ui->screen_wireguard_wireguard_list, 0, 27);
	lv_obj_set_size(ui->screen_wireguard_wireguard_list, 130, 112);
	lv_obj_set_scrollbar_mode(ui->screen_wireguard_wireguard_list, LV_SCROLLBAR_MODE_ACTIVE);

	//Write style state: LV_STATE_DEFAULT for &style_screen_wireguard_wireguard_list_main_main_default
	static lv_style_t style_screen_wireguard_wireguard_list_main_main_default;
	ui_init_style(&style_screen_wireguard_wireguard_list_main_main_default);
	
	lv_style_set_pad_top(&style_screen_wireguard_wireguard_list_main_main_default, 5);
	lv_style_set_pad_left(&style_screen_wireguard_wireguard_list_main_main_default, 5);
	lv_style_set_pad_right(&style_screen_wireguard_wireguard_list_main_main_default, 5);
	lv_style_set_pad_bottom(&style_screen_wireguard_wireguard_list_main_main_default, 5);
	lv_style_set_bg_opa(&style_screen_wireguard_wireguard_list_main_main_default, 255);
	lv_style_set_bg_color(&style_screen_wireguard_wireguard_list_main_main_default, lv_color_hex(0xffffff));
	lv_style_set_bg_grad_dir(&style_screen_wireguard_wireguard_list_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_border_width(&style_screen_wireguard_wireguard_list_main_main_default, 0);
	lv_style_set_radius(&style_screen_wireguard_wireguard_list_main_main_default, 0);
	lv_style_set_shadow_width(&style_screen_wireguard_wireguard_list_main_main_default, 0);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list, &style_screen_wireguard_wireguard_list_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_screen_wireguard_wireguard_list_main_scrollbar_default
	static lv_style_t style_screen_wireguard_wireguard_list_main_scrollbar_default;
	ui_init_style(&style_screen_wireguard_wireguard_list_main_scrollbar_default);
	
	lv_style_set_radius(&style_screen_wireguard_wireguard_list_main_scrollbar_default, 3);
	lv_style_set_bg_opa(&style_screen_wireguard_wireguard_list_main_scrollbar_default, 255);
	lv_style_set_bg_color(&style_screen_wireguard_wireguard_list_main_scrollbar_default, lv_color_hex(0xffffff));
	lv_style_set_bg_grad_dir(&style_screen_wireguard_wireguard_list_main_scrollbar_default, LV_GRAD_DIR_NONE);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list, &style_screen_wireguard_wireguard_list_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_screen_wireguard_wireguard_list_extra_btns_main_default
	static lv_style_t style_screen_wireguard_wireguard_list_extra_btns_main_default;
	ui_init_style(&style_screen_wireguard_wireguard_list_extra_btns_main_default);
	
	lv_style_set_pad_top(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 5);
	lv_style_set_pad_left(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 5);
	lv_style_set_pad_right(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 5);
	lv_style_set_pad_bottom(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 5);
	lv_style_set_border_width(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 0);
	lv_style_set_text_color(&style_screen_wireguard_wireguard_list_extra_btns_main_default, lv_color_hex(0x0D3055));
	lv_style_set_text_font(&style_screen_wireguard_wireguard_list_extra_btns_main_default, &lv_font_montserratMedium_12);
	lv_style_set_text_opa(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 255);
	lv_style_set_radius(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 3);
	lv_style_set_bg_opa(&style_screen_wireguard_wireguard_list_extra_btns_main_default, 255);
	lv_style_set_bg_color(&style_screen_wireguard_wireguard_list_extra_btns_main_default, lv_color_hex(0x000000));
	lv_style_set_bg_grad_dir(&style_screen_wireguard_wireguard_list_extra_btns_main_default, LV_GRAD_DIR_NONE);

	//Write style state: LV_STATE_DEFAULT for &style_screen_wireguard_wireguard_list_extra_texts_main_default
	static lv_style_t style_screen_wireguard_wireguard_list_extra_texts_main_default;
	ui_init_style(&style_screen_wireguard_wireguard_list_extra_texts_main_default);
	
	lv_style_set_pad_top(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 5);
	lv_style_set_pad_left(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 5);
	lv_style_set_pad_right(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 5);
	lv_style_set_pad_bottom(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 5);
	lv_style_set_border_width(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 0);
	lv_style_set_text_color(&style_screen_wireguard_wireguard_list_extra_texts_main_default, lv_color_hex(0x0D3055));
	lv_style_set_text_font(&style_screen_wireguard_wireguard_list_extra_texts_main_default, &lv_font_montserratMedium_10);
	lv_style_set_text_opa(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 255);
	lv_style_set_radius(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 3);
	lv_style_set_bg_opa(&style_screen_wireguard_wireguard_list_extra_texts_main_default, 255);
	lv_style_set_bg_color(&style_screen_wireguard_wireguard_list_extra_texts_main_default, lv_color_hex(0xffffff));
	lv_style_set_bg_grad_dir(&style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_GRAD_DIR_NONE);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list_item4, &style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list_item3, &style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list_item2, &style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list_item1, &style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_wireguard_wireguard_list_item0, &style_screen_wireguard_wireguard_list_extra_texts_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_wireguard_btn_1
	ui->screen_wireguard_btn_1 = lv_btn_create(ui->screen_wireguard);
	ui->screen_wireguard_btn_1_label = lv_label_create(ui->screen_wireguard_btn_1);
	lv_label_set_text(ui->screen_wireguard_btn_1_label, "Time Now");
	lv_label_set_long_mode(ui->screen_wireguard_btn_1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_wireguard_btn_1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_wireguard_btn_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->screen_wireguard_btn_1_label, LV_PCT(100));
	lv_obj_set_pos(ui->screen_wireguard_btn_1, 0, 137);
	lv_obj_set_size(ui->screen_wireguard_btn_1, 130, 23);

	//Write style for screen_wireguard_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_wireguard_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_wireguard_btn_1, lv_color_hex(0x9d8453), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_wireguard_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_wireguard_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_wireguard_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_wireguard_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_wireguard_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_wireguard_btn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_wireguard_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_wireguard_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of screen_wireguard.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->screen_wireguard);

	//Init events for screen.
	events_init_screen_wireguard(ui);
}
