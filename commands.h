#include "tableTools.h"
#include "generals.h"

struct Command{
    char *name; /* the name of the command */
    unsigned int opcode; /* the command's operation code */
    unsigned int funct; /* the funct value of the command */
    unsigned int num_of_args; /* the amount of args the command needs */
    char *valid_src_addr_methods; /* string of 0,1 that represents which addressing methods are valid for the source operand */
    char *valid_dst_addr_methods; /* string of 0,1 that represents which addressing methods are valid for the destination operand */
};

typedef struct Command Command;

/* Input: receives a Table to which the commands should be added.
 * Output: returns 0 if all went successfully, 1 otherwise. Fills the table with the different commands and their properties.
 *
 * This function receives a table and fills it with the different commands and their properties. Returns 1 in case of
 * failure, 0 otherwise.
 */
int fill_table(Table *table);

/* Input: a binary code node, the argument as a string, and its addressing method. Assumption: the argument is valid.
 * Output: constructs the node to match the argument. Returns 0 if no errors occurred, 1 otherwise.
 *
 * This function encodes the received argument and fills it to the node, and returns 0 if no errors occurred.
 * Else, 1 is returned.
 */
int constructArg(BinCodeNode *node, char *arg, enum addr_method method);