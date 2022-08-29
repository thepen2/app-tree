#pragma once

#include "os.h"

#ifdef CMOCKA_TEST
//#include "../cmocka_add.h"
#endif


// PEN: ADDED FUNCTIONS
void ui_extra_addr_from_pubkey(unsigned char * pubKeyIn, char * addrOut, char netByte);
void bip32_to_pubkey(uint32_t * bip32_dwords, uint8_t bip32_len, char * pubKey_chars, int szPubKey_chars, unsigned char * pubkey_chars_hex);
void chars_to_lowerCase(unsigned char * myChars, int charCount);
bool matchAddress(char * addr);
void dotDecimal(unsigned char * rawDecimal, char * outChars);
