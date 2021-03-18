#include "inputHandlers.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* Input: a string representing a line of the assembly file
 * Output: returns 1 if the line contains white spaces only or is a comment line, 0 otherwise
 *
 * Algorithm: iterate through the whole string. In each iteration:
 *              1. check if a ; encountered. If so, its a comment line so we return 1
 *              2. check if the char is not a space - if its neither a ; nor a space, we should return false
 *              3. increase line to continue check the rest
 */
int is_empty_or_comment_line(char *line){
    while (*line != '\0'){ /* scan the whole string */
        if (*line == ';'){ /* if the first non-space char is a ;, it is a comment line so we return 1 "true" */
            return 1;
        }

        /* if we encountered a char that isn't a ; nor a space, it is not an empty/comment line, return 0 "false" */
        if (!isspace(*line++)){
            return 0;
        }
    }

    return 1; /* if we arrived here, it means all the chars are white spaces so we return 1 "true" */
}

/* Input: receives 2 strings, one representing a line of text, and the other is a destination string
 * Output: returns the amount of chars read.
 *         Extracts the next token (from the first no-space char until a char in {delims} is reached) and stores it in the destination string
 *
 * Algorithm: iterate through line until its end (or until we manually break out of the loop), in each iteration increment the amount of chars read.
 * Skip all the spaces until the token is reached. Then, until a char in {delims} was encountered (in this case we manually break),
 * copy the chars we read to the destination string.
 * */
int get_token(char *line, char *dest, char *delims){
    int read_chars = 0; /* counter for amount of characters read */
    int reached_token = (isspace(line[0])) ? 0 : 1; /* states whether we have reached the token yet */
    int i; /* used in the for loop */

    for (i = 0; line[i] != '\0'; ++i) { /* scan the line until its end */
        read_chars++; /* update amount of characters read */
        if (reached_token == 0){ /* if we haven't reached the command yet */
            if (isspace(line[i])){ /* continue if the current char is a white space */
                continue;
            } else{ /* if we reached the first char of the token */
                *dest++ = line[i]; /* copy the first char to the destination string */
                reached_token = 1; /* mark that the token was reached */
            }
        } else{ /* this means we have already reached the token */
            if (is_char_in_delims(line[i], delims)){ /* exit the loop if a delimiter was encountered */
                break;
            }
            *dest++ = line[i]; /* if the token isn't finished yet, copy the current char to the destination string */
        }
    }

    *dest++ = '\0'; /* mark the end of the string */
    return read_chars - 1; /* return the amount of chars read, minus 1 to show the delimiter */
}

/* Input: receive a char an a string.
 * Output: returns 1 if c is in delims, 0 otherwise.
 *
 * Algorithm: iterate through the provided string. For each char, check if it is equal to c, and if so return 1 ("true").
 * If the current char is a space, check c with isspace. If the for loop is done, it means we checked to whole string
 * and c isn't in delims so we return 0 ("false")
 */
int is_char_in_delims(char c, char *delims){
    int i; /* used for the for loop */

    for (i = 0; delims[i] != '\0'; ++i) { /* iterate through the string */
        if (delims[i] == ' '){ /* check if the current char in delims is a space */
            if (isspace(c)){ /* check if c is any white space using isspace */
                return 1; /* return 1 ("true") if c is a space and space is in delims */
            }
        } else if (c == delims[i]){ /* check if c is identical to the current char in delims */
            return 1; /* return 1 ("true") if c is in delims */
        }
    }

    return 0; /* if we arrived here, it means c isn't in delims, so we return 0 ("false") */
}

/* Input: receives a string that represents a symbol, and a table in which the symbol must not be.
 * Output: returns 1 if the symbol is legal, 0 otherwise.
 *
 * Algorithm: First, check whether the symbol begins with a letter. Then, iterate through the string, and verify
 * that each char is a letter or a digit only. During the loop, count the length of the string, and ensure it is at most 31.
 * Also ensure the symbol name is not in the received table (the commands table, presumably), not a register nor keyword.
 * If one of these conditions isn't fulfilled, return 0 ("false"). Otherwise, return 1 ("true").
 */
int legal_symbol(char *symbol, Table *table){
    int i; /* to use in the for loop */

    if (!isalpha(symbol[0])) { /* return 0 if the symbol doesn't begin with a letter */
        return 0;
    }

    if (is_key_word(symbol, table)){
        return 0; /* return 0 if the symbol name is an existing command, a register or a keyword */
    }

    /* iterate through the string, begin in the second char because we have already checked the first */
    for (i = 1; symbol[i] != '\0'; ++i) {
        if (!isalpha(symbol[i]) && !isdigit(symbol[i])) { /* return 0 if the symbol contains an invalid char */
            return 0;
        }
    }

    if (i > MAX_SYMBOL_LENGTH){ /* return 0 ("false") if the symbol name is too long */
        return 0;
    }

    return 1; /* if we arrived here, it means all the conditions were met and the symbol is legal */
}

/* Input: a string.
 * Output: returns a non-zero representing the instruction value if the string is a data storing instruction, 0 otherwise.
 *
 * Algorithm: use strcmp to check if the received string is either .string or .data, and return the corresponding value.
 * If it is neither, return 0
 */
int is_data_store_inst(char *token){
    /* if token is equal to either of them, strcmp will return 0, so we flip it to return a non-zero value - "true" */

    /* check if the token is a .string instruction. If so, return STRING_INST */
    if (strcmp(token, ".string") == 0){
        return STRING_INST;
    }

    /* check if the token is a .data instruction. If so, return DATA_INST */
    if (strcmp(token, ".data") == 0){
        return DATA_INST;
    }

    return 0; /* if we arrived here, it means the token isn't a dta storing one, so we return 0 */
}

/* Input: a string representing an argument.
 * Output: the enum value representing the address method.
 *
 * Algorithm: if the first char in the string is a #, it means an immediate addressing method. If it is %, it means a
 * relative addressing method. Then, check if the string might represent a register, and if so it is an immediate register
 * addressing method. If non of the above conditions were met, the addressing method is direct - a symbol.
 */
enum addr_method get_addressing_method(char *arg){
    if (arg[0] == '#'){
        return IMMEDIATE; /* if the argument begins with a #, it is an Immediate addressing */
    } else if (arg[0] == '%'){
        return RELATIVE; /* if the argument begins with a %, it is a Relative addressing */
    }

    if (is_register(arg)){ /* check if it is a possible register */
        return IMD_REGISTER;
    }

    return DIRECT; /* if we reached here, it means the addressing method is Direct - the argument is a symbol */
}

/* Input: receives a string that represents a scalar.
 * Output: returns 0 if the scalar isn't a valid one, returns 1 otherwise.
 *
 * Algorithm: skip all white spaces. Decrement ind by one because the spaces check may have skipped the first important char.
 * Check if the scalar is introduced by a sign, and if so continue to the next char. Then ensure the scalar is not empty,
 * and until the end of the string is reached, we ensure it is a valid scalar: only digits.
 * If we encountered a char that isn't a digit, return 0. If the loop has ended, we return 1 because it means the scalar is valid
 */
int legal_scalar(char *scalar){
    int ind = 0; /* current index to read from scalar */
    int scalar_done = 0; /* marks whether a space was encountered after the scalar */

    for (; isspace(scalar[ind]); ind++); /* skip spaces */
    ind = (ind > 0) ? ind - 1 : ind; /* the for loop has skipped the first "important" char only if there were spaces, so we decrease ind to re-read it if necessary */

    if (scalar[ind] == '-' || scalar[ind] == '+'){
        ind++; /* skip the sign if it exists */
    }

    if (scalar[ind] == '\0'){
        return 0; /* an empty scalar is not legal */
    }

    while (scalar[ind] != '\0'){ /* check every character of the string */
        /* if the current character is not a number neither a space, and is not done yet (space wasn't reached), the scalar is invalid so we return 0 */
        if (!isdigit(scalar[ind]) && scalar_done == 0 && !isspace(scalar[ind])){
            return 0;
        } else if (isspace(scalar[ind])){
            scalar_done = 1; /* mark that the scalar is finished */
        } else if (scalar_done == 1){ /* if a digit was encountered after the scalar is already done, it isn't a valid scalar */
            return 0;
        }

        ind++; /* increment ind to continue to check the next character */
    }

    return 1; /* if we arrived here, it means the scalar is legal so we return 1 */
}

/* Input: the argument as a string, its addressing method and the commands table.
 * Output: returns 1 if the argument is valid, 0 otherwise.
 *
 * Algorithm: if the argument is a scalar (immediate addressing method) return whether it is a valid one. If it is a register,
 * ensure it is in the valid range: 0 to 7 (in our computer). If it is a relative or direct addressing method, ensure that
 * the symbol name is valid: not a command nor a keyword in the assembly language
 */
int legal_arg(char *arg, enum addr_method method, Table *commands_table){
    if (method == IMMEDIATE){ /* if the addressing method is immediate */
        return legal_scalar(arg+1); /* return whether the scalar is valid */
    }

    if (method == IMD_REGISTER){ /* if the addressing method is immediate register */
        int num = atoi(arg+1); /* get the number of the register */

        if (num >= 0 && num < NUM_OF_REGISTERS){ /* if the register is valid, return 1 */
            return 1;
        } else { /* if the register isn't valid, return 0 */
            return 0;
        }
    }

    if (*arg == '%'){ /* skip the % if it exists to test the label name */
        arg++;
    }

    if (is_key_word(arg, commands_table)){ /* an existing command or keyword is not a legal argument */
        return 0;
    }

    return 1; /* if we arrived here, it means the argument is legal, so we return 1 */
}

/* Input: a string.
 * Output: returns 1 if the string represents a register, 0 otherwise.
 *
 * Algorithm: First, check if str begins with a small r. If not, return 0. Then, check if all the chars after it represent
 * a scalar. If not, return 0. If yes, get the represented number and check whether it is a valid
 * register number. If so, return 1. Else, return 0.
 */
int is_register(char *str){
    int i; /* for the for loop */

    /* a name of a register must begin with a small r. If the received string doesn't begin with it, it isn't a register and we can return 0 */
    if (*str != 'r'){
        return 0;
    }

    /* check if all the chars after the r represent a valid scalar, if not - it isn't a register! */
    if (!legal_scalar(str+1)){
        return 0;
    }

    i = atoi(str+1);

    if (i >= 0 && i < NUM_OF_REGISTERS){
        return 1; /* if the argument was r followed by digits, it is an Immediate Register */
    }

    return 0; /* if one of the previous conditions is not met, str is not a register so we return 0 */
}

/* Input: a string.
 * Output: returns 1 if the string contains a space between 2 words, 0 otherwise.
 *
 * Algorithm: check every character in the string using a for loop. For each char, check whether it is a space.
 * If so, mark it. Else, check if a space was already encountered, and if so return 1 since a non-space char was met
 * after a space. If the for loop is done without returning anything, it means the string doesn't contain a space,
 * so we return 0.
 */
int is_containing_space_between_words(char *c){
    int i; /* to be used in the for loop */
    int space = 0; /* marks whether a space was encountered */

    for (i = 0; c[i] != '\0'; i++) { /* go over the whole string */
        if (isspace(c[i])){ /* if the current char is a space, mark it */
            space = 1;
        } else if (space == 1){ /* if a space was encountered, and then a non-space char, return 1 */
            return 1;
        }
    }

    return 0; /* if no space was encountered, or had no other char after itself, we should return 1 */
}

/* Input: a string and the commands table.
 * Output: returns 1 if the string is a keyword in our assembly language, 0 otherwise.
 *
 * Algorithm: first, check if the string is a key in the received table. If so, return 1. Then, using strcmp, we compare
 * the received string to all of the other keywords. If it matches one of them, we return 1. Then, we check whether it
 * represents a register, and if so return 1. Else, we return 0.
 */
int is_key_word(char *c, Table *commands_table){
    if (lookup(c, commands_table) != NULL){ /* return 1 if the symbol is in the commands table */
        return 1;
    }

    /* compare the string to all of the other keywords. If it matches one of them, return 1 */
    if (strcmp(c, "entry") == 0 || strcmp(c, "extern") == 0 || strcmp(c, "data") == 0 || strcmp(c, "string") == 0){
        return 1;
    }

    /* check if the string represents a name of a register. If it is, return 1 */
    if (is_register(c)){
        return 1;
    }

    return 0; /* if the code arrives here, it means the string isn't a keyword so 0 is returned */
}