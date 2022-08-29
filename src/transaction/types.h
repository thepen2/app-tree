#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define MAX_TX_LEN   510
#define ADDRESS_LEN  20
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)

typedef enum {
    PARSING_OK = 1,
    NONCE_PARSING_ERROR = -1,
    TO_PARSING_ERROR = -2,
    VALUE_PARSING_ERROR = -3,
    MEMO_LENGTH_ERROR = -4,
    MEMO_PARSING_ERROR = -5,
    MEMO_ENCODING_ERROR = -6,
    WRONG_LENGTH_ERROR = -7,

// PEN: EXTRA ERROR MESSAGES

    NOT_DECIMAL_TIME_1 = -8,        // DESERIALIZE_1
    TIME_1_PARSING_ERROR = -9,
    TX_TYPE_1_PARSING_ERROR = -10,
    TEMP_PUBK_PARSING_ERROR = -11,
    TEMP_PUBK_ERROR = -12,
    NOT_HEX_START_CHAIN = -13,
    NOT_BASE58_START = -14,
    START_PARSING_ERROR = -15,

    NOT_DECIMAL_TIME_2 = -16,      // DESERIALIZE_2
    TIME_2_PARSING_ERROR = -17,
    TX_TYPE_2_PARSING_ERROR = -18,
    NOT_HEX_SENDER_CHAIN = -19,
    NOT_BASE58_SENDER = -20,
    SENDER_PARSING_ERROR = -21,
    TX_ID_PARSING_ERROR = -22,
    NUM_RECPS_PARSING_ERROR = -23,
    NOT_HEX_RECP_1_CHAIN = -34,
    NOT_BASE58_RECP_1 = -25,
    RECP_1_PARSING_ERROR = -26,
    NOT_DECIMAL_AMT_1 = -27,
    AMT_1_PARSING_ERROR = -28,
    NOT_HEX_RECP_2_CHAIN = -29,
    NOT_BASE58_RECP_2 = -30,
    RECP_2_PARSING_ERROR = -31,
    NOT_DECIMAL_AMT_2 = -32,
    AMT_2_PARSING_ERROR = -33,
    SENDER_PUBK_PARSING_ERROR = -34,
    SENDER_PUBK_ERROR = -35,
    NOT_TEXT_1 = -36,
 
} parser_status_e;

typedef struct {
    uint64_t nonce;     /// nonce (8 bytes)
    uint64_t value;     /// amount value (8 bytes)
    uint8_t *to;        /// pointer to address (20 bytes)
    uint8_t *memo;      /// memo (variable length)
    uint64_t memo_len;  /// length of memo (8 bytes)

// PEN: EXTRA TRANSACTION PARAMETERS 
    uint8_t *timestamp;   /// pointer to timestamp of transaction (10 bytes decimal acsii)
    uint8_t txType;       /// transaction type (1 byte hex), startup if txType == ":" (0x3a)
    uint8_t *tempPubK;    /// pointer to compressed temp public key (33 butes hex)
    uint8_t *startup;     /// pointer to colon + startup chain address (43 bytes ascii)
    uint8_t *sender;      /// pointer to sender chain address (43 bytes ascii)
    uint8_t *txID;        /// transaction ID (32 bytes hex)
    uint8_t numRecps;     /// number of recipients (1 byte hex)
    uint8_t *recp1;       /// pointer to recipient 1 chain address  (43 bytes ascii)
    uint8_t *amt1;        /// pointer to amount for recipient 1 (10 chars decimal ascii)
    uint8_t *recp2;       /// pointer to recipient 2 chain address  (43 bytes ascii)
    uint8_t *amt2;        /// pointer to amount for recipient 1 (10 chars decimal ascii)
    uint8_t *sendPubK;    /// pointer to compressed public key of sender (33 bytes hex)

} transaction_t;




