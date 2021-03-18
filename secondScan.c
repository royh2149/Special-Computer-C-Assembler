#include "secondScan.h"
#include "numberingSystems.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* mark that there was an error, and skip further processing */
#define MARK_ERROR_AND_CONTINUE errors = 1; \
                                continue;

/* a constant to use whenever the program exits and needs to free all the dynamically allocated resources */
#define FREE_ALL free(line_holder); \
                free(token);

/* check every char left in the line, and if it isn't a space indicate error */
#define CHECK_EXTRA_TEXT while (*line != '\0'){ \
                                    if(!isspace(*line++)) { \
                                        printf("Error in line %d: unrecognized extra text!\n", line_counter); \
                                        line--; \
                                        break; \
                                    } \
                                } \
                          if (!isspace(*line) && *line != '\0'){ \
                              MARK_ERROR_AND_CONTINUE \
                          } \

/* Input: an assembly file, the symbols table, and the binary code as a list of nodes.
* Output: returns 0 if no errors occurred, 1 otherwise.
*
* Algorithm: First, declare necessary variables and make sure malloc() was successful. Then, iterate through every line
* in the file: 1. read the first token and update the line counter
*              2. skip symbol definition, .data, .string or .extern
*              3. if the token is a .entry, specify the appropriate symbol as an entry, only if it is not .extern as well
*
* Then, for every node in the code list that is not encoded, if it begins with a %, encode the corresponding distance
* only if the symbol is valid - no data nor external symbol. Otherwise, it is direct addressing so encode the symbol's address.
* In case of a .extern symbol, mark it as E in the ARE property and add a usage of it in the matching address.
* Before returning the errors variable to indicate whether the function succeeded, free all the dynamically-allocated variables.
*/
int secondScan(FILE *asmfile, Table *symbols_table, BinCodeNode *codeList){
    char *line_holder = (char *) malloc(sizeof (char) * MAX_LINE_LENGTH); /* stores the current line of the file */
    char *line; /* used to iterate through the current line of the file */
    char *token = (char *) malloc(sizeof (char) * ARG_SIZE); /* holds the current token */
    int read; /* to hold the amount of chars read */
    int errors = 0; /* used to mark errors */
    int line_counter = 0; /* to keep track of the current line */
    cell *entryCell;
    Symbol *symbol, *curSymbol;

    cell *c;

    /* ensure memory allocation succeeded */
    RETURN_IF_MEMORY_ALLOC_ERROR(line_holder, 1)
    RETURN_IF_MEMORY_ALLOC_ERROR(token, 1)

    while (fgets(line_holder, MAX_LINE_LENGTH, asmfile) != NULL){ /* read every line, until EOF is reached */
        line = line_holder;
        read = get_token(line, token, " :"); /* read the first token */
        line += read; /* skip the characters that have been just read */
        line_counter++; /* update the line counter */

        if (*line++ == ':'){ /* skip the symbol definition if exists */
            read = get_token(line, token, " "); /* get the next token */
            line += read + 1; /* proceed to the next chars, skip the delimiter */
        }

        if (is_data_store_inst(token) || strcmp(token, ".extern") == 0){ /* skip .data, .string or .extern */
            continue;
        }

        if (strcmp(token, ".entry") == 0){ /* check if a .entry is reached */
            read = get_token(line, token, " "); /* get the symbol name */
            line += read + 1; /* proceed to the next chars, skip the delimiter */

            entryCell = (cell *)lookup(token, symbols_table); /* search for the relevant symbol in the table */

            if (entryCell == NULL){ /* mark an error if a .entry was declared but never defined */
                printf("Error in line %d: No such symbol was defined. %s\n", line_counter, line_holder);
                MARK_ERROR_AND_CONTINUE
            }

            symbol = (Symbol *)entryCell->value; /* get the symbol */

            if (symbol->isExternal == true){ /* ensure the symbol is not an .extern as well */
                printf("Error in line %d: a symbol cannot be both .entry and .extern\n", line_counter);
                MARK_ERROR_AND_CONTINUE
            }

            symbol->isEntry = true; /* specify that the symbol is an entry symbol */

            /* ensure no extra text is written */
            CHECK_EXTRA_TEXT
        }
    }

    /* encode the symbols and relative addressing methods that couldn't be encoded in the first scan */
    while (codeList != NULL){ /* iterate through the whole list */
        /* check if the current word is not yet encoded */
        if (codeList->code[0] != '0' && codeList->code[0] != '1'){ /* update the current word only if it isn't encoded yet */
            if (codeList->code[0] == '%'){ /* check if it a relative addressing method */
                int val;

                c = (cell *)lookup((codeList->code+1), symbols_table); /* search for the symbol being used */

                if (c == NULL){ /* mark error if no such symbol was defined */
                    printf("Error in line %d: No such symbol was defined - %s\n", codeList->L, codeList->code+1);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                curSymbol = (Symbol *) c->value; /* get the symbol itself */

                if (curSymbol == NULL){ /* mark error if no such symbol was defined */
                    printf("Error in line %d: undefined symbol!\n", codeList->L);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                /* relative addressing method cannot be used with external symbols */
                if (curSymbol->isExternal == true){
                    printf("Error in line %d: Relative addressing cannot be applied on an external symbol!\n", codeList->L);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                /* relative addressing method cannot be used with .data symbols */
                if (curSymbol->isData == true){
                    printf("Error in line %d: Relative addressing cannot be applied on a .data symbol!\n", codeList->L);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                val = curSymbol->value - (codeList->IC); /* determine the distance value */
                codeList->code = decimal_to_bin(val); /* encode the distance to the current word */
                if (codeList->code == NULL){ /* ensure encoding succeeded */
                    MARK_ERROR_AND_CONTINUE
                }
            } else {
                c = (cell *)lookup((codeList->code), symbols_table); /* search for the symbol being used */

                if (c == NULL){ /* mark error if no such symbol was defined */
                    printf("Error in line %d: No such symbol was defined - %s\n", codeList->L, codeList->code);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                curSymbol = (Symbol *) c->value; /* get the symbol */

                if (curSymbol == NULL){ /* mark error if no such symbol was defined */
                    printf("Error in line %d: undefined symbol!\n", codeList->L);
                    codeList = codeList->next;
                    MARK_ERROR_AND_CONTINUE
                }

                codeList->code = decimal_to_bin((int)curSymbol->value); /* encode the address of the symbol */
                if (codeList->code == NULL){ /* ensure encoding succeeded */
                    MARK_ERROR_AND_CONTINUE
                }

                codeList->are = R; /* a symbol should be specified as R in the ARE property */

                if (curSymbol->isExternal == true){ /* if the symbol is external, mark its ARE as E and add a usage for it */
                    codeList->are = E;
                    if (addUsage(curSymbol, codeList->IC) != 0){ /* if usage addition failed */
                        /* free the dynamically allocated variables and exit */
                        FREE_ALL
                        return 1;
                    }
                }
            }
        }

        codeList = codeList->next; /* continue to the next word */
    }

    /* free the dynamically allocated variables */
    FREE_ALL

    return errors; /* indicate whether errors occurred during the scan */
}

/* Input: receive a symbol and an address in which it is used.
 * Output: adds the address to the usages of the symbol. Returns 0 if succeeded, 1 otherwise.
 *
 * Algorithm: first, allocate memory for the new usage and ensure it succeeded. Set its value to the received one, and
 * then, we go to the end of the usages list of the received symbol, and insert the new usage.
 */
int addUsage(Symbol *symbol, unsigned int value){
    Usage *curUsage = symbol->usage; /* get the head of the list */
    Usage *newUsage = (Usage *)calloc(1, sizeof(Usage)); /* allocate memory for a new usage */

    RETURN_IF_MEMORY_ALLOC_ERROR(newUsage, 1) /* ensure memory allocation went successful */

    newUsage->value = value; /* set the value in which the symbol is used */

    while (curUsage->next != NULL){ /* move to the end of the usages list */
        curUsage = curUsage->next;
    }

    curUsage->next = newUsage; /* add the new usage at the end of the list */

    return 0; /* indicate success */
}
