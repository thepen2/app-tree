Tree Nano application : Technical Specifications


## Framework

### APDUs

The messaging format of the Tree Nano app is compatible with the [APDU protocol](https://developers.ledger.com/docs/nano-app/application-structure/#apdu-interpretation-loop). The `P1` and `P2` fields are set to 0 in all messages but `INS` = 0x06, where `P2` is set to 0x80 for the first message, and `P1` is set to 0x01 for second message in a set of 2 messages.

The main commands use `CLA` = 0xE0, except for `INS` 01 where `CLA` = 0xB0.

| CLA | INS | COMMAND NAME         | DESCRIPTION |
|-----|-----|----------------------|-------------|
|  B0 |  01 | GET_APP_NAME_VERSION | Return app name and version |
|  E0 |  03 | GET_APP_NAME         | Return app name |
|  E0 |  04 | GET_VERSION          | Return app version |
|  E0 |  05 | GET PUBLIC KEY       | Return public key from BIP32 path |
|  E0 |  06 | SIGN_TRANSACTION     | Sign a message with a key matching a BIP32 path |


### Interactive commands

The transaction signing operation is executed via an interactive protocol that requires 3 rounds. First a GET_PUBLIC_KEY command `INS` = 0x05 is sent.  If a status word of 0x9000 (success) is received, then the SIGN_MESSAGE command itself is sent in 2 messages.  The first is a second transmission of the BIP32 path of the signing account, with `P2` is set to 0x80.  If a status word of 0x9000 (success) is received in response to that, the transaction data to be signed is sent in a second `INS` = 0x06 command with `P1` is set to 0x01.

The specs for the client commands are detailed below.

## Wallet specifications

The Tree Nano app provides for two 34 character base58 account addresses per Nano device, derived from the operational internal secret seeds using the BIP32 paths `m/44'/644'/0'` and `m/44'/644'/1'`.  The first is designated as the Tree hot wallet, the second the Tree cold wallet.  The hot wallet uses the network byte 0x28 so that the base58 address generation results in the first character of the account address being `H`.  The cold wallet uses a network byte of 0x1c resulting in a first character of `C`.  

In addition, this pair of account addresses is permanently assigned to a particular 8 digit hexadecimal blockchain number represented in ASCII, in a one time process on the network in which the Tree Nano app is not involved.  The full blockchain address of an account in the Tree network is then given as `blockchain_number:account_address` (43 characters), using a colon as a separator.  The blockchain assignment is what authorizes the accounts to transact with other accounts on the Tree network.

## CData for SIGN_TRANSACTION requests (second message)

### Tree client application start up

The Tree client application on start up first sends a special signing request to the Tree Nano app, with byte [10] of the CData set to 0x3a, and byte [11] set to 0x02 or 0x03 (the first byte of a compressed public key).  If the signature returned by the Tree Nano app verifies this authorizes the start up of the Tree client application for further operations on the Tree network.  The full format of the CData of this signing request is as follows:

| Length      | Description     |
|-------------|-----------------|
| 10          | Timestamp (decimal ASCII) |
| 1           | Transaction type (0x3a) |
| 33          | Compressed public key (hexadecimal) |
| 1           | Colon separator (0x3a) |
| 43          | Sender blockchain address (mixed format) |
 

### Tree client general authorizations

The Tree client application also uses a signing request to authorize other operations, with byte [10] of the CData also set to 0x3a, but with byte [11] set to 0x01.  In this case the field that follows is a plain text string, a descriptor of the operation to be authorized.  The full format of the CData of this signing request is as follows:


| Length      | Description     |
|-------------|-----------------|
| 10          | Timestamp (decimal ASCII) |
| 1           | Transaction type (0x3a) |
| 1           | General authorization flag (0x01) |
| 32          | Text string (ASCII) |
| 1           | Colon separator (0x3a) |
| 43          | Sender blockchain address (mixed format) |


### Tree cryptocurrency transaction signing

Otherwise signing requests are used to sign transaction data.  In this case, byte [10] of the CDATA is the transaction type, a hexadecimal character other than 0x3a.  There can be either 1 or 2 recipients of a Tree cryptocurrency transaction, flagged by byte [86] of the CData.  The recipient 2 blockchain address and amount fields are only present if the number of recipients is 2.  The full format of a cryptocurrency transaction signing request is as follows:

| Length      | Description     |
|-------------|-----------------|
| 10          | Timestamp (decimal ASCII) |
| 1           | Transaction type (hexadecimal) |
| 43          | Sender blockchain address (mixed format) |
| 32          | Transaction ID (hexadecimal) |
| 1           | Number of recipients (hexadecimal) |
| 43          | Recipient 1 blockchain address (43 chars mixed format)
| 16          | Amount 1 (decimal ASCII)
| 43          | Recipient 2 blockchain address (43 chars mixed format) OPTIONAL
| 16          | Amount 2 (decimal ASCII) OPTIONAL
| 33          | Sender compressed public key (hexadecimal) |


## Address matching 

As a security feature the Tree Nano app will only sign a transaction if the Sender base58 account address matches the one generated internally by the corresponding BIP32 path, `m/44'/644'/0'` for the hot account or `m/44/644'/1'` for the cold account, respectively.  Otherwise a status of 0x7001 is returned.

## Main status words

| SW     | SW name                    | Description |
|--------|----------------------------|-------------|
| 0x6985 | SW_DENY                    | Rejected by user |
| 0x6A86 | SW_WRONG_P1P2              | Either `P1` or `P2` is incorrect |
| 0x6A87 | SW_WRONG_DATA_LENGTH       | `Lc` or minimum APDU length is incorrect |
| 0x6D00 | SW_INS_NOT_SUPPORTED       | No command exists with `INS` |
| 0x6E00 | SW_CLA_NOT_SUPPORTED       | Bad `CLA` used for this application |
| 0xB000 | SW_WRONG_RESPONSE_LENGTH   | Wrong response length (buffer size problem) |
| 0xB007 | SW_BAD_STATE               | Aborted because unexpected state reached |
| 0x9000 | SW_OK                      | Success |
| 0x7001 | SW_ADDRESS_MISMATCH        | Address mismatch |

## Device state status words

| SW     | SW name                      | Description |
|--------|------------------------------|-------------|
| 0x6511 | APP NOT OPEN                 | Tree Nano app not open |
| 0x6D01 | APP NOT OPEN                 | Tree Nano app not open |
| 0x0000 | TIMED OUT                    | Signing operation timed out |
| 0x530C | NOT LOGGED IN                | Not logged into Nano device |

## Transaction CData parsing status words (passed through to Nano response)

| SW     | SW name                      | Description |
|--------|------------------------------|-------------|
| -7     | WRONG_LENGTH_ERROR           | CData length mismatch |
| -8     | NOT_DECIMAL_TIME_1           | Timestamp chars not decimal |
| -9     | TIME_1_PARSING_ERROR         | Other timestamp parsing error |
| -10    | TX_TYPE_1_PARSING_ERROR      | Transaction type parsing error |
| -11    | TEMP_PUBK_PARSING_ERROR      | Temporary public key parsing error |
| -12    | TEMP_PUBK_ERROR              | CData byte [11] not good |
| -13    | NOT_HEX_START_CHAIN          | Sender blockchain number not hexadecimal |
| -14    | NOT_BASE58_START             | Sender account address not base58 |
| -15    | START_PARSING_ERROR          | Other account parsing error |
| -16    | NOT_DECIMAL_TIME_2           | Timestamp not decimal |
| -17    | TIME_2_PARSING_ERROR         | Other timestamp parsing error |
| -18    | TX_TYPE_2_PARSING_ERROR      | Transaction type parsing error |
| -19    | NOT_HEX_SENDER_CHAIN         | Sender blockchain number not hexadecimal |
| -20    | NOT_BASE58_SENDER            | Sender account address not base58 |
| -21    | SENDER_PARSING_ERROR         | Other sender account parsing error |
| -22    | TX_ID_PARSING_ERROR          | Transaction ID parsing eror |
| -23    | NUM_RECPS_PARSING_ERROR      | Number of recipients not good |
| -24    | NOT_HEX_RECP_1_CHAIN         | Recipient 1 blockchain not hexadecimal |
| -25    | NOT_BASE58_RECP_1            | Recipient 1 account not base58 |
| -26    | RECP_1_PARSING_ERROR         | Other recipient 1 parsing error |
| -27    | NOT_DECIMAL_AMT_1            | Amount 1 not decimal |
| -28    | AMT_1_PARSING_ERROR          | Other amount 1 parsing error |
| -29    | NOT_HEX_RECP_2_CHAIN         | Recipient 2 blockchain not hexadecimal |
| -30    | NOT_BASE58_RECP_2            | Recipient 2 account not base58 |
| -31    | RECP_2_PARSING_ERROR         | Other recipient 2 parsing error |
| -32    | NOT_DECIMAL_AMT_2            | CData length mismatch |
| -33    | AMT_2_PARSING_ERROR          | Amount 2 not decimal |
| -34    | SENDER_PUBK_PARSING_ERROR    | Sender public key parsing error |
| -35    | SENDER_PUBK_ERROR            | Sender compressed public key error |
| -36    | NOT_TEXT_1                   | Not plain text string |


## Commands

### GET_APP_NAME_VERSION

Returns the app name and version open on the Nano device.

#### Encoding

**Command**

| CLA | INS |
|-----|-----|
| B0  | 01  |

**Input data**

None

**Output data**

The app name and version open on the Nano device.

#### Description

The return is in the following format:

| Length  | Description |
|---------|-------------|
| 1       | 0x01        |
| 1       | Length of app name (hexadecimal) |
| varies  | App name (ASCII) |
| 1       | Length of app version (hexadecimal) |
| varies  | App version (ASCII) |

### GET_VERSION

Gets the version of the open Nano app.

#### Encoding

**Command**

| CLA | INS |
|-----|-----|
| E0  | 03  |

**Input data**

None

**Output data**

The version of the open app on the Nano device.

#### Description

The return is in the following format:

| Length  | Description       |
|---------|-------------------|
| 3       | App version (hexadecimal) |

### GET_APP_NAME

Gets the name of the open Nano app.

#### Encoding

**Command**

| CLA | INS |
|-----|-----|
| E0  | 04  |

**Input data**

None

**Output data**

The name of the open app on the Nano device.

#### Description

The return is in the following format:

| Length  | Description      |
|---------|------------------|
| varies  | App name (ASCII) |


### GET_PUBLIC_KEY

Gets the public key corresponding to the corresponding BIP32 path for the respective Tree account.

#### Encoding

**Command**

| CLA | INS |
|-----|-----|
| E0  | 05  |

**Input data**

| Length  | Description          |
|---------|----------------------|
| 1       | BIP32 length (0x03) |
| 4       | 0x800002c (BIP44 type) |
| 4       | 0x8000284 (coin type 644') |
| 4       | Account number (0x80000000 or 0x80000001) |

**Output data**

| Length   | Description     |
|----------|-----------------|
| 1        | Length of public key (0x41) |
| 65       | Public key (hexadecimal) |
| 1        | Length of chain code (0x20) |
| 32       | Chain code (hexadecimal) |


#### Description

The return is the public key and the chain code corresponding to the BIP32 path input.


### SIGN_TRANSACTION

Request signing of both cryptocurrency transactions and other authorization operations.  The first message input data duplicates the GET_PUBLIC_KEY message, but with `INS` = 0x06 and `P2` = 0x80.  The input CData for the second message is given above, depending on the type of signing request or authorization requested.


**Command**

| CLA | INS |
|-----|-----|
| E0  | 06  |

**Input data**

See the above.

**Output data**


| Length   | Description     |
|----------|-----------------|
| 1        | Length of signature (hexadecimal) |
| varies   | DER signature (hexadecimal, max 72 chars) |



## Security considerations for signing requests

The input data for signing transactions is vetted to confirm that all fields are limited to appropriate characters.  Compressed public keys must start with either 0x02 or 0x03. Timestamp and amount fields are limited to decimal ASCII characters.  Blockchain numbers must be ASCII respresentations of hex characters (0-9, a-f).  Account addresses must be comprised of base58 characters.

In addition, the Tree Nano app checks to confirm that the sender account Tree address matches the one generated by the corresponding BIP32 path on device from the secret seed words.  If the first character of the sender address is `H` it must match that generated by `m/44'/644'/0'`.  If the first character of the sender address is `C`, it must match that generated by `m/44'/644'/1'`.  Otherwise the transaction is rejected.
