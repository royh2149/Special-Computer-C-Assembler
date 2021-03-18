#include <stdio.h>
#include "inputHandlers.h"

/* Input:  an assembly file, the symbols table, and the binary code as a list of nodes.
 * Output: returns 0 if no errors occurred, 1 otherwise.
 *
 * This function completes the compilation process, according to the second scan requirements. First, it locates the
 * .entry symbols. Then, it completes the encoding of the program, replacing the names of the symbols with their address,
 * or distance necessary to "jump".
 */
int secondScan(FILE *asmfile, Table *symbols_table, BinCodeNode *codeList);

/* Input: receive a symbol and an address in which it is used.
 * Output: adds the address to the usages of the symbol. Returns 0 if succeeded, 1 otherwise.
 *
 * This function inserts the received address of usage to the received symbol, and returns 0 if insertion went properly,
 * else returns 1.
 */
int addUsage(Symbol *symbol, unsigned int value);

