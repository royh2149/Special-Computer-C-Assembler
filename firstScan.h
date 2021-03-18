#include <stdio.h>
#include "secondScan.h"

/* Input: an assembly file, the commands table, and the base filename.
 * Output: returns nothing.
 *
 * This function begins the process of the assembler, returns 0 if no errors occurred, non-zero value otherwise.
 */
int first_scan(FILE *asmfile, Table *commands_table, char *filename);

/* Input: the symbols table and a value - ICF.
 * Output: returns nothing. Adds to every cell in the table the value to add, to the "value" attribute.
 *
 * This function receives the symbols table and the ICF, and adds it to the value of every cell.
 */
void update_all_symbols_value(Table *symbols_table, unsigned int to_add);

/* Input: a string of valid ascii chars and a pointer to the data counter.
 * Output: returns a list of nodes that represents the string. Updates the data counter received. If something failed,
 * NULL is returned.
 *
 * This function receives a string and encodes it, returns it as a list of nodes. It also updates
 * the data counter value.
 */
BinCodeNode *encode_string(char *string, int *DC);

/* Input: a string representing the name of the symbol.
 * Output: returns a symbol with the provided name, all additional attributes set to false. If memory Allocation failed,
 * NULL is returned.
 *
 * This function receives a name, creates a symbol with it, resets its properties and returns it, or NULL if something
 * failed.
 */
Symbol *create_new_symbol(char *name);


/* Input: a list of binary codes to free.
 * Output: returns nothing.
 *
 * This function receives a list of binary code nodes, and frees the dynamically-allocated memory it uses.
 */
void free_list(BinCodeNode *head);
