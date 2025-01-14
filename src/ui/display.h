#pragma once

#include <stdbool.h>  // bool

/**
 * Callback to reuse action with approve/reject in step FLOW.
 */
typedef void (*action_validate_cb)(bool);

/**
 * Display address on the device and ask confirmation to export.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_address(void);

/**
 * Display transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_transaction(void);


// PEN: CUSTOM DISPLAY FUNCTIONS
int ui_display_transaction_1(void);
int ui_display_transaction_2_1(void);
int ui_display_transaction_2_2(void);
int ui_display_transaction_2_3(void);

