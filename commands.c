#include "commands.h"
#include "numberingSystems.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Input: receives a Table to which the commands should be added.
 * Output: returns 0 if all went successfully, 1 otherwise. Fills the table with the different commands and their properties.
 *
 * Algorithm: first, allocate memory for all the commands, and ensure none failed. Then, edit the attributes for each command and
 * insert it to the received table using install(). If an insertion failed, 1 is returned. If not, 0 is returned.
 */
int fill_table(Table *table){
    /* allocate memory for all of the commands */
    Command *mov = (Command *) malloc(sizeof(Command));
    Command *cmp = (Command *) malloc(sizeof(Command));
    Command *add = (Command *) malloc(sizeof(Command));
    Command *sub = (Command *) malloc(sizeof(Command));
    Command *lea = (Command *) malloc(sizeof(Command));
    Command *clr = (Command *) malloc(sizeof(Command));
    Command *not = (Command *) malloc(sizeof(Command));
    Command *inc = (Command *) malloc(sizeof(Command));
    Command *dec = (Command *) malloc(sizeof(Command));
    Command *jmp = (Command *) malloc(sizeof(Command));
    Command *bne = (Command *) malloc(sizeof(Command));
    Command *jsr = (Command *) malloc(sizeof(Command));
    Command *red = (Command *) malloc(sizeof(Command));
    Command *prn = (Command *) malloc(sizeof(Command));
    Command *rts = (Command *) malloc(sizeof(Command));
    Command *stop = (Command *) malloc(sizeof(Command));

    /* ensure all memory allocation succeeded */
    RETURN_IF_MEMORY_ALLOC_ERROR(mov, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(cmp, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(add, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(sub, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(lea, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(clr, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(not, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(inc, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(dec, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(jmp, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(bne, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(jsr, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(red, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(prn, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(rts, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(stop, 1)


    /* specify the properties of each command and install it to the table */
    mov->name = "mov";
    mov->funct = 0;
    mov->opcode = 0;
    mov->num_of_args = 2;
    mov->valid_src_addr_methods = "1101";
    mov->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(mov->name, mov, table), 1) /* add the current command to the table and return in case of failure */

    cmp->name = "cmp";
    cmp->funct = 0;
    cmp->opcode = 1;
    cmp->num_of_args = 2;
    cmp->valid_src_addr_methods = "1101";
    cmp->valid_dst_addr_methods = "1101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(cmp->name, cmp, table), 1) /* add the current command to the table and return in case of failure */

    add->name = "add";
    add->funct = 10;
    add->opcode = 2;
    add->num_of_args = 2;
    add->valid_src_addr_methods = "1101";
    add->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(add->name, add, table), 1) /* add the current command to the table and return in case of failure */

    sub->name = "sub";
    sub->funct = 11;
    sub->opcode = 2;
    sub->num_of_args = 2;
    sub->valid_src_addr_methods = "1101";
    sub->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(sub->name, sub, table), 1) /* add the current command to the table and return in case of failure */

    lea->name = "lea";
    lea->funct = 0;
    lea->opcode = 4;
    lea->num_of_args = 2;
    lea->valid_src_addr_methods = "0100";
    lea->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(lea->name, lea, table), 1) /* add the current command to the table and return in case of failure */

    clr->name = "clr";
    clr->funct = 10;
    clr->opcode = 5;
    clr->num_of_args = 1;
    clr->valid_src_addr_methods = "0000";
    clr->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(clr->name, clr, table), 1) /* add the current command to the table and return in case of failure */

    not->name = "not";
    not->funct = 11;
    not->opcode = 5;
    not->num_of_args = 1;
    not->valid_src_addr_methods = "0000";
    not->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(not->name, not, table), 1) /* add the current command to the table and return in case of failure */

    inc->name = "inc";
    inc->funct = 12;
    inc->opcode = 5;
    inc->num_of_args = 1;
    inc->valid_src_addr_methods = "0000";
    inc->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(inc->name, inc, table), 1) /* add the current command to the table and return in case of failure */

    dec->name = "dec";
    dec->funct = 13;
    dec->opcode = 5;
    dec->num_of_args = 1;
    dec->valid_src_addr_methods = "0000";
    dec->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(dec->name, dec, table), 1) /* add the current command to the table and return in case of failure */

    jmp->name = "jmp";
    jmp->funct = 10;
    jmp->opcode = 9;
    jmp->num_of_args = 1;
    jmp->valid_src_addr_methods = "0000";
    jmp->valid_dst_addr_methods = "0110";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(jmp->name, jmp, table), 1) /* add the current command to the table and return in case of failure */

    bne->name = "bne";
    bne->funct = 11;
    bne->opcode = 9;
    bne->num_of_args = 1;
    bne->valid_src_addr_methods = "0000";
    bne->valid_dst_addr_methods = "0110";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(bne->name, bne, table), 1) /* add the current command to the table and return in case of failure */

    jsr->name = "jsr";
    jsr->funct = 12;
    jsr->opcode = 9;
    jsr->num_of_args = 1;
    jsr->valid_src_addr_methods = "0000";
    jsr->valid_dst_addr_methods = "0110";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(jsr->name, jsr, table), 1) /* add the current command to the table and return in case of failure */

    red->name = "red";
    red->funct = 0;
    red->opcode = 12;
    red->num_of_args = 1;
    red->valid_src_addr_methods = "0000";
    red->valid_dst_addr_methods = "0101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(red->name, red, table), 1); /* add the current command to the table and return in case of failure */

    prn->name = "prn";
    prn->funct = 0;
    prn->opcode = 13;
    prn->num_of_args = 1;
    prn->valid_src_addr_methods = "0000";
    prn->valid_dst_addr_methods = "1101";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(prn->name, prn, table), 1) /* add the current command to the table and return in case of failure */

    rts->name = "rts";
    rts->funct = 0;
    rts->opcode = 14;
    rts->num_of_args = 0;
    rts->valid_src_addr_methods = "0000";
    rts->valid_dst_addr_methods = "0000";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(rts->name, rts, table), 1) /* add the current command to the table and return in case of failure */

    stop->name = "stop";
    stop->funct = 0;
    stop->opcode = 15;
    stop->num_of_args = 0;
    stop->valid_src_addr_methods = "0000";
    stop->valid_dst_addr_methods = "0000";
    RETURN_IF_MEMORY_ALLOC_ERROR(install(stop->name, stop, table), 1); /* add the current command to the table and return in case of failure */

    return 0; /* indicate that everything went well */
}

/* Input: a binary code node, the argument as a string, and its addressing method. Assumption: the argument is valid.
 * Output: constructs the node to match the argument. Returns 0 if no errors occurred, 1 otherwise.
 *
 * Algorithm: If the addressing method is Immediate, the word of the argument is the binary representation of the scalar.
 * If the argument is a register, allocate memory for the word (ensure it succeeded), set all its chars to '0' except
 * the one in the appropriate index, which is set to '1'. Otherwise, allocate memory for the word in the size of a symbol name,
 * and after ensuring it succeeded, copy the name of the symbol to the word. If the addressing method is Direct, set the
 * ARE property to R. If something failed during function execution, we return 1. If all went fine, 0 is returned.
 */
int constructArg(BinCodeNode *node, char *arg, enum addr_method method){
    int i = 0;

    if (method == IMMEDIATE){
        node->code = decimal_to_bin(atoi(arg+1)); /* encode the scalar to binary, skip the # */
        RETURN_IF_MEMORY_ALLOC_ERROR(node->code, 1) /* ensure conversion succeeded */
        return 0; /* exit the function and indicate that everything went successfully */
    } else if (method == IMD_REGISTER){
        char *word = (char *)malloc(sizeof(char) * WORD_SIZE); /* allocate memory for the argument */

        /* check if memory allocation completed successfully */
        RETURN_IF_MEMORY_ALLOC_ERROR(word, 1)

        for (i = 0; i < WORD_SIZE; i++){ /* set all the bits to 0 */
            word[i] = '0';
        }

        word[WORD_SIZE - atoi(arg+1) - 1] = '1'; /* set the appropriate bit to 1, marking the right register */
        word[WORD_SIZE] = '\0'; /* mark end of string */

        node->code = word;
        return 0; /* exit the function and indicate that everything went successfully */
    }

    /* if the code arrived here, it is either a direct of a relative addressing method. In both cases, we should
     * store only the name of the symbol, and in the second scan it will be replaced to the right binary code */

    /* allocate memory for the name of the symbol  */
    node->code = (char *)malloc(sizeof(char) * (MAX_SYMBOL_LENGTH + 1));

    /* check if memory allocation completed successfully */
    RETURN_IF_MEMORY_ALLOC_ERROR(node->code, 1)

    /* copy the name of the symbol into the node, without possible spaces */
    while (!isspace(arg[i]) && arg[i] != '\0'){
        node->code[i] = arg[i];
        i++;
    }
    node->code[i] = '\0'; /* mark the end of the string */

    /* if the addressing method is direct, its ARE property should be R or E. The second scan will update it to E if needed */
    if (method == DIRECT){
        node->are = R;
    }

    return 0;
}
