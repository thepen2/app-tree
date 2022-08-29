/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include "os.h"
#include "ux.h"
#include "glyphs.h"

#include "../globals.h"
#include "menu.h"
#include "../crypto.h"

// PEN: EXTRA INCLUDES
#include "extra1.h"  
#include "extra2.h"  
#pragma GCC diagnostic ignored "-Wformat-invalid-specifier" // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"        // snprintf


UX_STEP_NOCB(ux_menu_ready_step, pnn, {&C_boilerplate_logo, "Tree", "is ready"});
UX_STEP_NOCB(ux_menu_version_step, bn, {"Version", APPVERSION});

// PEN: EXTRA STEPS
UX_STEP_CB(ux_menu_show_hot_pub_key_step, bn, ui_menu_show_hot_pub_key(), {"Show hot pub key", "m/44'/644'/0'"});
UX_STEP_CB(ux_menu_show_cold_pub_key_step, bn, ui_menu_show_cold_pub_key(), {"Show cold pub key", "m/44'/644'/1'"});
UX_STEP_CB(ux_menu_show_hot_address_step, bn, ui_menu_show_hot_address(), {"Show hot address", "m/44'/644'/0'"});
UX_STEP_CB(ux_menu_show_cold_address_step, bn, ui_menu_show_cold_address(), {"Show cold address", "m/44'/644'/1'"});

UX_STEP_CB(ux_menu_about_step, pb, ui_menu_about(), {&C_icon_certificate, "About"});
UX_STEP_VALID(ux_menu_exit_step, pb, os_sched_exit(-1), {&C_icon_dashboard_x, "Quit"});

// FLOW for the main menu:
// #1 screen: ready
// #2 screen: show hot address    // PEN: EXTRA STEP
// #3 screen: show cold address   // PEN: EXTRA STEP
// #4 screen: version of the app
// #5 screen: about submenu
// #6 screen: quit
UX_FLOW(ux_menu_main_flow,
        &ux_menu_ready_step,
//        &ux_menu_show_hot_pub_key_step,    // PEN: EXTRA STEP FOR DEBUG
        &ux_menu_show_hot_address_step,    
//        &ux_menu_show_cold_pub_key_step,   // PEN: EXTRA STEP FOR DEBUG
        &ux_menu_show_cold_address_step,   
        &ux_menu_version_step,
        &ux_menu_about_step,
        &ux_menu_exit_step,
        FLOW_LOOP);

void ui_menu_main() {
    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }

    ux_flow_init(0, ux_menu_main_flow, NULL);
}

UX_STEP_NOCB(ux_menu_info_step, bn, {"Tree app", "(c) 2022 The Pen"});
UX_STEP_CB(ux_menu_back_step, pb, ui_menu_main(), {&C_icon_back, "Back"});

// FLOW for the about submenu:
// #1 screen: app info
// #2 screen: back button to main menu
UX_FLOW(ux_menu_about_flow, &ux_menu_info_step, &ux_menu_back_step, FLOW_LOOP);

void ui_menu_about() {
    ux_flow_init(0, ux_menu_about_flow, NULL);
}


/////////////////////////////////
//  PEN: EXTRA MENU FUNCTIONS  //
/////////////////////////////////

    pubkey_ctx_t pubkey_ctx_0;  // LOCAL GLOBALS
    static char pubkey_0_chars[131];
    pubkey_ctx_t pubkey_ctx_1;
    static char pubkey_1_chars[131];
    static unsigned char pubkey_0_hex[64];
    static unsigned char pubkey_1_hex[64];

    static char legacy_hot_address[35];
    static char legacy_cold_address[35];
   
UX_STEP_NOCB(ux_menu_show_hot_pub_key_step2, bnnn_paging, 
            {
                .title = "Hot pub key:", 
                .text = pubkey_0_chars,
            });

UX_FLOW(ux_menu_show_hot_pub_key_flow2, &ux_menu_show_hot_pub_key_step2, &ux_menu_back_step, FLOW_LOOP);

void ui_menu_show_hot_pub_key() {

    uint8_t bip32_path_len_0 = 3;
    uint32_t bip32_path_0[3] = {0x8000002c, 0x80000284, 0x80000000};

    bip32_to_pubkey((uint32_t *) bip32_path_0, bip32_path_len_0, (char * ) pubkey_0_chars, 131, (unsigned char * )pubkey_0_hex);

    chars_to_lowerCase( (unsigned char *) pubkey_0_chars, 130);

    ux_flow_init(0, ux_menu_show_hot_pub_key_flow2, NULL);

}


UX_STEP_NOCB(ux_menu_show_hot_address_step2, bnnn_paging, 
            {
                .title = "Hot address:", 
                .text = legacy_hot_address,
            });
UX_FLOW(ux_menu_show_hot_address_flow2, &ux_menu_show_hot_address_step2, &ux_menu_back_step, FLOW_LOOP);

void ui_menu_show_hot_address() {

    uint8_t bip32_path_len_0 = 3;
    uint32_t bip32_path_0[3] = {0x8000002c, 0x80000284, 0x80000000};

    bip32_to_pubkey((uint32_t *) bip32_path_0, bip32_path_len_0, (char * ) pubkey_0_chars, 131, (unsigned char * ) pubkey_0_hex);

    ui_extra_addr_from_pubkey((unsigned char *) pubkey_0_hex, (char *) legacy_hot_address, 0x28);

    ux_flow_init(0, ux_menu_show_hot_address_flow2, NULL);

}


UX_STEP_NOCB(ux_menu_show_cold_pub_key_step2, bnnn_paging, 
            {
                .title = "Cold pub key:", 
                .text = pubkey_1_chars,
            });
UX_FLOW(ux_menu_show_cold_pub_key_flow2, &ux_menu_show_cold_pub_key_step2, &ux_menu_back_step, FLOW_LOOP);

void ui_menu_show_cold_pub_key() {

    uint8_t bip32_path_len_1 = 3;
    uint32_t bip32_path_1[3] = {0x8000002c, 0x80000284, 0x80000001};

    bip32_to_pubkey((uint32_t *) bip32_path_1, bip32_path_len_1, (char * ) pubkey_1_chars, 131, (unsigned char * )pubkey_1_hex);

    chars_to_lowerCase( (unsigned char *) pubkey_1_chars, 130);

    ux_flow_init(0, ux_menu_show_cold_pub_key_flow2, NULL);

}


UX_STEP_NOCB(ux_menu_show_cold_address_step2, bnnn_paging, 
            {
                .title = "Cold address:", 
                .text = legacy_cold_address,
            });
UX_FLOW(ux_menu_show_cold_address_flow2, &ux_menu_show_cold_address_step2, &ux_menu_back_step, FLOW_LOOP);

void ui_menu_show_cold_address() {

// CALCULATE COLD KEY HERE, SO IT CAN BE SHOWN IN NEXT STEP

    uint8_t bip32_path_len_1 = 3;
    uint32_t bip32_path_1[3] = {0x8000002c, 0x80000284, 0x80000001};

    bip32_to_pubkey((uint32_t *) bip32_path_1, bip32_path_len_1, (char * ) pubkey_1_chars, 131, (unsigned char * ) pubkey_1_hex);

    ui_extra_addr_from_pubkey((unsigned char *) pubkey_1_hex, (char *) legacy_cold_address, 0x1C);

    ux_flow_init(0, ux_menu_show_cold_address_flow2, NULL);

}

