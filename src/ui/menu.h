#pragma once

/**
 * Show main menu (ready screen, version, about, quit).
 */
void ui_menu_main(void);

/**
 * Show about submenu (copyright, date).
 */
void ui_menu_about(void);

// PEN
/**
 * Show about hot public key.
 */

// PEN: ADDED FUNCTIONS
void ui_menu_show_hot_pub_key(void);
void ui_menu_show_cold_pub_key(void);
void ui_menu_show_hot_address(void);
void ui_menu_show_cold_address(void);

