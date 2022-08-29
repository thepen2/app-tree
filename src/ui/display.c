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

#pragma GCC diagnostic ignored "-Wformat-invalid-specifier"  // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"         // snprintf

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"

#include "display.h"
#include "../constants.h"
#include "../globals.h"
#include "../io.h"
#include "../sw.h"
#include "../address.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../common/bip32.h"
#include "../common/format.h"

// PEN: EXTRA INCLUDE
#include "extra1.h"
#include "extra2.h"

static action_validate_cb g_validate_callback;
static char g_amount[30];
static char g_bip32_path[60];
static char g_address[43];

// PEN: EXTRA LOCAL GLOBALS, +1 IN SIZE FOR NULL TERMINATOR
static char g_timestamp[11];
static char g_tempPubK[69];  // TWO EXTRA BYTES FOR THE "0x"
static char g_startup_address[44];
static char g_sender_address[44];
static char g_recp1_address[44];
char g_recp1_amount[18];
static char g_recp2_address[44];
char g_recp2_amount[18];
static char g_text[33] = { 0 };

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm address"});
// Step with title/text for BIP32 path
UX_STEP_NOCB(ux_display_path_step,
             bnnn_paging,
             {
                 .title = "Path",
                 .text = g_bip32_path,
             });
// Step with title/text for address
UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = g_address,
             });
// Step with approve button
UX_STEP_CB(ux_display_approve_step,
           pb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_reject_step,
           pb,
           (*g_validate_callback)(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

// FLOW to display address and BIP32 path:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display BIP32 Path
// #3 screen: display address
// #4 screen: approve button
// #5 screen: reject button
UX_FLOW(ux_display_pubkey_flow,
        &ux_display_confirm_addr_step,
        &ux_display_path_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_address() {
    if (G_context.req_type != CONFIRM_ADDRESS || G_context.state != STATE_NONE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_bip32_path, 0, sizeof(g_bip32_path));
    if (!bip32_path_format(G_context.bip32_path,
                           G_context.bip32_path_len,
                           g_bip32_path,
                           sizeof(g_bip32_path))) {
        return io_send_sw(SW_DISPLAY_BIP32_PATH_FAIL);
    }

    memset(g_address, 0, sizeof(g_address));
    uint8_t address[ADDRESS_LEN] = {0};
    if (!address_from_pubkey(G_context.pk_info.raw_public_key, address, sizeof(address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    snprintf(g_address, sizeof(g_address), "0x%.*H", sizeof(address), address);

    g_validate_callback = &ui_action_validate_pubkey;

    ux_flow_init(0, ux_display_pubkey_flow, NULL);

    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "transaction",
             });
// Step with title/text for amount
UX_STEP_NOCB(ux_display_amount_step,
             bnnn_paging,
             {
                 .title = "Amount",
                 .text = g_amount,
             });


// FLOW to display transaction information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display amount
// #3 screen : display destination address
// #4 screen : approve button
// #5 screen : reject button
UX_FLOW(ux_display_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_amount_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_transaction() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_amount, 0, sizeof(g_amount));
    char amount[30] = {0};
    if (!format_fpu64(amount,
                      sizeof(amount),
                      G_context.tx_info.transaction.value,
                      EXPONENT_SMALLEST_UNIT)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    snprintf(g_amount, sizeof(g_amount), "BOL %.*s", sizeof(amount), amount);
    PRINTF("Amount: %s\n", g_amount);

    memset(g_address, 0, sizeof(g_address));
    snprintf(g_address, sizeof(g_address), "0x%.*H", ADDRESS_LEN, G_context.tx_info.transaction.to);

    g_validate_callback = &ui_action_validate_transaction;

    ux_flow_init(0, ux_display_transaction_flow, NULL);

    return 0;
}


/////////////////////////////////
//  PEN: TRANSACTION DISPLAYS  //
/////////////////////////////////

// Step with timestamp
UX_STEP_NOCB(ux_display_timestamp_step,
             bn,
             {
                "Timestamp",
                 g_timestamp,
             });
// Step with temp public key
UX_STEP_NOCB(ux_display_temp_public_key_step,
             bnnn_paging,
             {
                 "Temp public key",
                 g_tempPubK,
             });
// Step with startup chain address
UX_STEP_NOCB(ux_display_startup_chain_address_step,
             bnnn_paging,
             {
                 "Startup address",
                 g_startup_address,
             });
// Step with authorize startup button
UX_STEP_CB(ux_display_authorize_startup_step,
           pbb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Authorize",
               "start up",
           });

// FLOW to display transaction 1 (authorize startup) information:
// #1 screen : timestamp                // ONLY FOR DEBUG
// #2 screen : temp public key          // ONLY FOR DEBUG
// #3 screen : startup chain address
// #4 screen : approve button for authorize start up
// #5 screen : reject button
UX_FLOW(ux_display_transaction_1_flow,
//        &ux_display_timestamp_step,
//        &ux_display_temp_public_key_step,
        &ux_display_startup_chain_address_step,
//        &ux_display_hash_step,
        &ux_display_authorize_startup_step,
        &ux_display_reject_step);

// FLOW to display general action approval
// Step with approve general button
UX_STEP_CB(ux_display_approve_general_step,
           pbb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve", 
               g_text,
           });

UX_FLOW(ux_display_transaction_general_flow,
        &ux_display_approve_general_step,
        &ux_display_reject_step);

int ui_display_transaction_1() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (G_context.tx_info.transaction.tempPubK[0] == 0x01) {
        memmove((char *) g_text, G_context.tx_info.transaction.tempPubK + 1, 32);
        g_text[32] = 0x00;  // NULL TERMINATE, SUPERFLOUS, WHOLE ARRAY ZEROED TO INIT
    }
    else {
        memmove((char *) g_timestamp, G_context.tx_info.transaction.timestamp, 10);
        g_timestamp[10] = 0x00;  // NULL TERMINATE
    }

    memmove((char *) g_startup_address, G_context.tx_info.transaction.startup, 43);
    g_startup_address[43] = 0x00;    // NULL TERMINATE

    char legacyAddress2[34];
    memmove( (char *) legacyAddress2, (char * ) g_startup_address + 9, 34);
    int matchResult = matchAddress( (char * ) legacyAddress2);

    if (matchResult == 0) {
         return io_send_sw(SW_ADDRESS_MISMATCH);
    }

    snprintf(g_tempPubK, sizeof(g_tempPubK), "0x%.*H", 33, G_context.tx_info.transaction.tempPubK);
    g_tempPubK[68] = 0x00;   // NULL TERMINATE. 2 EXTRA BYTES FOR THE "0x"

    g_validate_callback = &ui_action_validate_transaction;

    if (G_context.tx_info.transaction.tempPubK[0]== 0x01) {
        ux_flow_init(0, ux_display_transaction_general_flow, NULL);
    }
    else {
        ux_flow_init(0, ux_display_transaction_1_flow, NULL);
    }

    return 0;

}

// Step with sender chain address
UX_STEP_NOCB(ux_display_sender_address_step,
             bnnn_paging,
             {
                 "Send from your",
                 g_sender_address,
             });
// Step with recipient1 chain address
UX_STEP_NOCB(ux_display_recipient_address_step,
             bnnn_paging,
             {
                 "To recp address",
                 g_recp1_address,
             });
// Step with single recipient amount
UX_STEP_NOCB(ux_display_recipient_amount_step, bn, {"Amount: ", g_recp1_amount});

// Step with approve transaction button
UX_STEP_CB(ux_display_approve_transaction_step,
           pbb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve", 
               "transaction",
           });

// FLOW to display transaction 2_1 (standard transaction, 1 recp) information:
// #1 screen : sender account    
// #2 screen : recipient
// #3 screen : recipient amount
// #4 screen : approve button for authorize start up
// #5 screen : reject button
UX_FLOW(ux_display_transaction_2_flow_1,
        &ux_display_sender_address_step,
        &ux_display_recipient_address_step,
        &ux_display_recipient_amount_step,
        &ux_display_approve_transaction_step,
        &ux_display_reject_step);

int ui_display_transaction_2_1() {

    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    
    memmove((char *) g_sender_address, G_context.tx_info.transaction.sender + 9, 34);
    g_sender_address[34] = 0x00;    // NULL TERMINATE

    int matchResult = matchAddress( (char * ) g_sender_address);

    if (matchResult == 0) {
         return io_send_sw(SW_ADDRESS_MISMATCH);
    }


    memmove((char *) g_recp1_address, G_context.tx_info.transaction.recp1 + 9, 34);
    g_recp1_address[34] = 0x00;    // NULL TERMINATE

// CONVERT TO DOTTED DECIMAL
    dotDecimal(G_context.tx_info.transaction.amt1, g_recp1_amount);
 
    g_validate_callback = &ui_action_validate_transaction;

    ux_flow_init(0, ux_display_transaction_2_flow_1, NULL);

    return 0;

}

// Step with recipient1 chain address
UX_STEP_NOCB(ux_display_recipient1_address_step,
             bnnn_paging,
             {
                 "To recp 1 address",
                 g_recp1_address,
             });
// Step with recipient2 chain address (OPTIONAL)
UX_STEP_NOCB(ux_display_recipient2_address_step,
             bnnn_paging,
             {
                 "To recp 2 address",
                 g_recp2_address,
             });

// Step with recipient 1 amount
UX_STEP_NOCB(ux_display_recipient1_amount_step, bn, {"Amount 1: ", g_recp1_amount});
// Step with recipient 2 amount (OPTIONAL)
UX_STEP_NOCB(ux_display_recipient2_amount_step, bn, {"Amount 2: ", g_recp2_amount});

// FLOW to display transaction 2_2 (standard transaction, 2 recps) information:
// #1 screen : sender account    
// #2 screen : recipient 1 
// #3 screen : recipient 1 amount
// #4 screen : recipient 2 (OPTIONAL)
// #5 screen : recipient 2 amount (OPTIONAL)
// #6 screen : approve button for authorize start up
// #7 screen : reject button
UX_FLOW(ux_display_transaction_2_flow_2,
        &ux_display_sender_address_step,
        &ux_display_recipient1_address_step,
        &ux_display_recipient1_amount_step,
        &ux_display_recipient2_address_step,
        &ux_display_recipient2_amount_step,
        &ux_display_approve_transaction_step,
        &ux_display_reject_step);

int ui_display_transaction_2_2() {

    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    
    memmove((char *) g_sender_address, G_context.tx_info.transaction.sender + 9, 34);
    g_sender_address[34] = 0x00;    // NULL TERMINATE

    int matchResult = matchAddress( (char * ) g_sender_address);

    if (matchResult == 0) {
         return io_send_sw(SW_ADDRESS_MISMATCH);
    }

    memmove((char *) g_recp1_address, G_context.tx_info.transaction.recp1 + 9, 34);
    g_recp1_address[34] = 0x00;    // NULL TERMINATE
    memmove((char *) g_recp2_address, G_context.tx_info.transaction.recp2 + 9, 34);
    g_recp2_address[34] = 0x00;    // NULL TERMINATE

// CONVERT TO DOTTED DECIMAL
    dotDecimal(G_context.tx_info.transaction.amt1, g_recp1_amount);
    dotDecimal(G_context.tx_info.transaction.amt2, g_recp2_amount);
 
    g_validate_callback = &ui_action_validate_transaction;

    ux_flow_init(0, ux_display_transaction_2_flow_2, NULL);

    return 0;

}

