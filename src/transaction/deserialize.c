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

#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "../common/buffer.h"
#include "../common/base58.h"
#include <stdio.h>

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    if (buf->size > MAX_TX_LEN) {
        return WRONG_LENGTH_ERROR;
    }

    // nonce
    if (!buffer_read_u64(buf, &tx->nonce, BE)) {
        return NONCE_PARSING_ERROR;
    }

    tx->to = (uint8_t *) (buf->ptr + buf->offset);
    // TO address
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }

    // amount value
    if (!buffer_read_u64(buf, &tx->value, BE)) {
        return VALUE_PARSING_ERROR;
    }

    // length of memo
    if (!buffer_read_varint(buf, &tx->memo_len) && tx->memo_len > MAX_MEMO_LEN) {
        return MEMO_LENGTH_ERROR;
    }

    // memo
    tx->memo = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, tx->memo_len)) {
        return MEMO_PARSING_ERROR;
    }

    if (!transaction_utils_check_encoding(tx->memo, tx->memo_len)) {
        return MEMO_ENCODING_ERROR;
    }
    
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


/////////////////////////////////////////
//  PEN: CUSTOM DESERIALIZE FUNCTIONS  //
/////////////////////////////////////////

// CUSTOM PARAMETER TYPES (STARTUP)
//          -> timestamp  (10 bytes decimal ascii)
//          -> spacer     (1 byte hex = 0x3a)
//          -> tempPubK   (33 bytes hex, but read in as chars)
//          -> startup    (43 bytes ascii, startup chain address)

parser_status_e transaction_deserialize_1(buffer_t *buf, transaction_t *tx) {

    if (buf->size > MAX_TX_LEN) {
          return WRONG_LENGTH_ERROR;
    }

    if (!isDecimal((uint8_t *) (buf->ptr + buf->offset), 0, 10)) {
        return NOT_DECIMAL_TIME_1;
    }

    // timestamp (10 bytes decimal ascii)
    tx->timestamp = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 10)) {
        return TIME_1_PARSING_ERROR;
    }

    // txType (1 byte hex)
    if (!buffer_read_u8(buf, &tx->txType)) {
        return TX_TYPE_1_PARSING_ERROR;
    }

    // tempPubK (33 bytes hex, but read in as chars)
    tx->tempPubK = (uint8_t *) (buf->ptr + buf->offset);

    if (tx->tempPubK[0] == 0x01) {
        if (!isTextString((uint8_t *) (buf->ptr + buf->offset), 1, 32)) {
            return NOT_TEXT_1;
        }
    }

    if (!buffer_seek_cur(buf, 33)) {
        return TEMP_PUBK_PARSING_ERROR;
    }

    if ((tx->tempPubK[0] != 0x02) &&   // COMPRESSED PUBLIC KEY
        (tx->tempPubK[0] != 0x03) &&   // COMPRESSED PUBLIC KEY
        (tx->tempPubK[0] != 0x01) ) {  // GENERIC APPROVAL TEXT
        return TEMP_PUBK_ERROR;
    }

    buffer_seek_cur(buf, 1); // SKIP COLON SPACER, SAME 

    if (!isHexadecimal((uint8_t *) (buf->ptr + buf->offset), 2, 8)) {
        return NOT_HEX_START_CHAIN;
    }

    // TEST ADDRESS PART OF STARTUP CHAIN ADDRESS FOR BASE58
    if (!isBase58((uint8_t *) (buf->ptr + buf->offset), 9, 34)) {
        return NOT_BASE58_START;
    }

    // startup (43 bytes ascii, startup chain address)
    tx->startup = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 43)) {
        return START_PARSING_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


// CUSTOM PARAMETER TYPES (STANDARD TRANSACTION)
//          -> timestamp  (10 bytes decimal ascii)
//          -> tx type    (1 byte hex)
//          -> sender     (43 bytes ascii, sender chain address)
//          -> txID       (32 bytes hex, transaction ID, read in as bytes)
//          -> numRecps   (1 byte hex, number of recipients)
//          -> recp1      (43 bytes ascii, recipient 1 chain address)
//          -> amt1       (16 bytes ascii, recipient 1 amount)
//          -> recp2      (43 bytes ascii, recipient 2 chain address) OPTIONAL
//          -> amt2       (16 bytes ascii, recipient 2 amount) OPTIONAL
//          -> senderPubK (33 bytes hex, sender compressed public key)

parser_status_e transaction_deserialize_2(buffer_t *buf, transaction_t *tx) {
    if (buf->size > MAX_TX_LEN) {
          return WRONG_LENGTH_ERROR;
    }

    if (!isDecimal((uint8_t *) (buf->ptr + buf->offset), 0, 10)) {
        return NOT_DECIMAL_TIME_2;
    }

    // timestamp (10 bytes decimal ascii)
    tx->timestamp = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 10)) {
        return TIME_2_PARSING_ERROR;
    }

    // txType (1 byte hex)
    if (!buffer_read_u8(buf, &tx->txType)) {
        return TX_TYPE_2_PARSING_ERROR;
    }

    if (!isHexadecimal((uint8_t *) (buf->ptr + buf->offset), 0, 8)) {
        return NOT_HEX_SENDER_CHAIN;
    }

    // TEST ADDRESS PART OF SENDER CHAIN ADDRESS FOR BASE58
    if (!isBase58((uint8_t *) (buf->ptr + buf->offset), 9, 34)) {
        return NOT_BASE58_SENDER;
    }

    // sender (43 bytes ascii, sender chain address)
    tx->sender = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 43)) {
        return SENDER_PARSING_ERROR;
    }

    // txID (32 bytes hex, transaction ID)
    tx->txID = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 32)) {
        return TX_ID_PARSING_ERROR;
    }

    // numRecps (1 byte hex)
    if (!buffer_read_u8(buf, &tx->numRecps)) {
        return NUM_RECPS_PARSING_ERROR;
    }

    if (!isHexadecimal((uint8_t *) (buf->ptr + buf->offset), 0, 8)) {
        return NOT_HEX_RECP_1_CHAIN;
    }

    // TEST ADDRESS PART OF RECIPIENT 1 CHAIN ADDRESS FOR BASE58
    if (!isBase58((uint8_t *) (buf->ptr + buf->offset), 9, 34)) {
        return NOT_BASE58_RECP_1;
    }

    // recp1 (43 bytes ascii, sender chain address)
    tx->recp1 = (uint8_t *) (buf->ptr + buf->offset);

    if (!buffer_seek_cur(buf, 43)) {
        //return TEST_ERROR;  // RETURNS 0xff80, THAT"S CORRECT
        return RECP_1_PARSING_ERROR;
    }

    // TEST AMOUNT FOR RECIPIENT 1 FOR DECIMAL
    if (!isDecimal((uint8_t *) (buf->ptr + buf->offset), 0, 16)) {
        return NOT_DECIMAL_AMT_1;
    }

    // amt1 (16 bytes decimal ascii)
    tx->amt1 = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 16)) {
        return AMT_1_PARSING_ERROR;
    }

// OPTIONAL SECOND RECIPIENT

    if (tx->numRecps > 1) {   

        if (!isHexadecimal((uint8_t *) (buf->ptr + buf->offset), 0, 8)) {
            return NOT_HEX_RECP_2_CHAIN;
        }

    // TEST ADDRESS PART OF RECIPIENT 2 CHAIN ADDRESS FOR BASE58
        if (!isBase58((uint8_t *) (buf->ptr + buf->offset), 9, 34)) {
            return NOT_BASE58_RECP_2;
        }

    // recp2 (43 bytes ascii, sender chain address)
        tx->recp2 = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, 43)) {
            return RECP_2_PARSING_ERROR;
        }

    // TEST AMOUNT FOR RECIPIENT 2 FOR DECIMAL
        if (!isDecimal((uint8_t *) (buf->ptr + buf->offset), 0, 16)) {
            return NOT_DECIMAL_AMT_2;
        }

    // amt2 (10 bytes decimal ascii)
        tx->amt2 = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, 16)) {
            return AMT_2_PARSING_ERROR;
        }
  
    }

    // sendPubK (33 bytes hex, but read in as chars)
    tx->sendPubK = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, 33)) {
        return SENDER_PUBK_PARSING_ERROR;
    }

    if ((tx->sendPubK[0] != 0x02) && (tx->sendPubK[0] != 0x03)) {
        return SENDER_PUBK_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}

