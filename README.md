# Tree Nano App<br><br>

<b>Tree cryptocurrency application interface, registered as coin type 644 with https://github.com/satoshilabs/slips/blob/master/slip-0044.md:</b><br><br>

This repository is a fork of https://github.com/LedgerHQ/app-boilerplate, merged with files modified for the Tree Nano app.  Per Ledger Nano documentation requirements for app submission, there is a new /docs/apdu.md file.<br><br>

In the /bin-tree_cli directory is our tree_cli_plus.exe Win32 command line interface (CLI) program to test the new functions.<br><br>

Usage: (for tree_cli_plus.exe)<br>
    tree_cli_plus 0      &nbsp;&nbsp;&nbsp;&nbsp;// sign_tx address mismatch (deliberate)<br>
    tree_cli_plus 1      &nbsp;&nbsp;&nbsp;&nbsp;// get_app_and_version<br>
    tree_cli_plus 2      &nbsp;&nbsp;&nbsp;&nbsp;// approve_password_change<br>
    tree_cli_plus 3      &nbsp;&nbsp;&nbsp;&nbsp;// get_version<br>
    tree_cli_plus 4      &nbsp;&nbsp;&nbsp;&nbsp;// get_app_name<br>
    tree_cli_plus 5      &nbsp;&nbsp;&nbsp;&nbsp;// get_public_key m/44'/644'/0'<br>
    tree_cli_plus 6      &nbsp;&nbsp;&nbsp;&nbsp;// sign_startup<br>
    tree_cli_plus 7      &nbsp;&nbsp;&nbsp;&nbsp;// get_public_key m/44'/644'/1'<br>
    tree_cli_plus 8      &nbsp;&nbsp;&nbsp;&nbsp;// sign_tx (1 recipient)<br>
    tree_cli_plus 9      &nbsp;&nbsp;&nbsp;&nbsp;// sign_tx (2 recipients)<br><br>

CHANGES TO THE FOLLOWING FILES:<br>
/src/common/base58.c (new functions to validate base58, decimal, hexadecimal, and text string char arrays)<br>
/src/common/base58.h<br>
/src/handler/sign_tx.c (forks to new transaction format handlers)<br>
/src/sw.h (new 0x7001 status word for address/BIP32 mismatch)<br>
/src/transaction/deserialize.c (new functions to parse additional transaction formats)<br>
/src/transaction/deserialize.h<br>
/src/transaction/types.h (new transaction struct elements and additional error messsage defines)<br>
/src/ui/display.c (handlers for reviewing the new transaction signing operations)<br>
/src/ui/display.h<br>
/src/ui/extra1.c<br> (new functions to generate public keys from BIP32 paths, convert BIP32 to Tree base58 addresses, and match addresses with BIP32)<br>
/src/ui/extra1.h<br>
/src/ui/extra2.c<br> (new functions to format dotted decimal amounts and convert char arrays to lower case)<br>
/src/ui/extra2.h<br>
/src/ui/menu.c (added UX functions to display two account type base58 addresses)<br>
/src/ui/menu.h<br>
ALL CHANGES CAN BE FOUND BY SEARCHING THE ABOVE FOR "PEN:"<br><br>

7 new tests in the /unit-tests directory<br>
test_tx_deserialize_1.c, test_tx_deserialize_2_1.c, text_tx_deserialize_2_2.c, text_tx_deserialize_2_3.c, test_lowercase.c, test_charsets.c and test_dotted_decimal.c, and corresponding entries added to the /unit-tests/CMakeLists.txt file.  Run tests with:<br><br>

cmake -Bbuild -H. && make -C build<br>
CTEST_OUTPUT_ON_FAILURE=1 make -C build test<br><br>

ALSO ADDED ICONS:<br>
/icons/nanos_app_tree4.gif<br>
/icons/nanox_app_tree4.gif<br>
/glyphs/boilerplate_logo.gif (overwritten)<br>
Makefile (uploaded here also) modified to reflect icon name changes, app name, version and path (m/44'/644').<br><br>



