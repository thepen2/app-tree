/*****************************************************************************
 *   Nano App Extra Functions
 *   (c) 2022 The Pen
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

#include "../crypto.h"
#include "../common/base58.h"
#include "../types.h"
#include "../globals.h"
#include "../common/bip32.h"
#include "extra1.h"

#pragma GCC diagnostic ignored "-Wformat-invalid-specifier"  // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"  // snprintf


//////////////////////////////
//  PEN: EXTRA FUNCTIONS 1  //
//////////////////////////////

void bip32_to_pubkey(uint32_t * bip32_dwords, uint8_t bip32_len, char * pubkey_chars, int szPubKey_chars, unsigned char * pubkey_chars_hex) {
    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};
    pubkey_ctx_t pubkey_ctx;

    crypto_derive_private_key(&private_key,
                              pubkey_ctx.chain_code,
                              bip32_dwords,
                              bip32_len);

    crypto_init_public_key(&private_key, &public_key, pubkey_ctx.raw_public_key);

    explicit_bzero(&private_key, sizeof(private_key));

   szPubKey_chars = 64;  // QUIET COMPILER WARNING ABOUT UNUSED VARIABLE

   snprintf(pubkey_chars, szPubKey_chars, "0x%.*H", \
             sizeof(pubkey_ctx.raw_public_key), pubkey_ctx.raw_public_key);

    // ALSO PASS THE RAW 64 BYTES
    memmove(pubkey_chars_hex, pubkey_ctx.raw_public_key, 64);

    pubkey_chars[130] = 0x00;
}


void ui_extra_addr_from_pubkey(unsigned char * pubKeyIn, char * addrOut, char netByte) {

    cx_ripemd160_t riprip;
    unsigned char buffer[32];
    unsigned char ripemd160_buf[20];
    unsigned char checksum_buf[22];
    checksum_buf[0] = netByte;  // INITIALIZE BUFFER WITH netByte IN PLACE
    unsigned char checksum_buf2[32];
    unsigned char finalHash[37];
    unsigned char compressedPubKey[33];

// DERIVE ADDRESS FROM COMPRESSED PUBLIC KEY, pubKeyIn IS RAW PUBLIC KEY
    unsigned char testByte =  pubKeyIn[63];
    if (testByte & 0x01) {  // LAST BYTE DETERMINES FIRST BYTE OF COMPRESSED
        compressedPubKey[0] = 0x03;
    }   
    else {
        compressedPubKey[0] = 0x02;
    }

    memmove((unsigned char * ) compressedPubKey + 1, (char *) pubKeyIn, 32);

    cx_hash_sha256(compressedPubKey, 33, buffer, 32);

    cx_ripemd160_init(&riprip);
    cx_hash(&riprip.header, CX_LAST, buffer, 32, ripemd160_buf, 20);

    memmove((unsigned char *) checksum_buf + 1, (unsigned char *) ripemd160_buf, 20);

    cx_hash_sha256(checksum_buf, 21, checksum_buf2, 32);

    cx_hash_sha256(checksum_buf2, 32, checksum_buf2, 32);

    memmove((unsigned char *) finalHash, (unsigned char *) checksum_buf, 21);
    memmove((unsigned char *) finalHash + 21, (unsigned char *) checksum_buf2, 4);

// WRITE THIS ONE DIRECTLY TO TEXT ARRAY
    base58_encode((uint8_t *) finalHash, 25, (char *) addrOut, 34);
    addrOut[34] = 0x00;

    return;
}

bool matchAddress( char * addr) {

    int i;
    static char pubkey_2_chars[131];
    static unsigned char pubkey_2_hex[64];
    static char testAddress[35];

    bip32_to_pubkey( (uint32_t *) G_context.bip32_path, G_context.bip32_path_len, \
                     (char *) pubkey_2_chars, 131, (unsigned char * ) pubkey_2_hex);

    char netByte2 = 0x28;
    uint32_t dword3 = G_context.bip32_path[2];

    if (dword3 == (uint32_t) 0x800000000) {
        netByte2 = 0x28;
    }
    else if (dword3 == (uint32_t) 0x80000001) {
        netByte2 = 0x1c;
    }

    ui_extra_addr_from_pubkey( (unsigned char *) pubkey_2_hex, (char *) testAddress, netByte2);

    for (i = 0; i < 34; i++) {
        if (addr[i] != testAddress[i]) {
            return 0;
        }
    }

    return 1;
}

