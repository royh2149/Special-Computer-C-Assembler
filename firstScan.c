#include "numberingSystems.h"
#include "buildOutputFiles.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* a constant to use whenever the program exits and needs to free all the dynamically allocated resources */
#define FREE_ALL free(line_holder); \
                free(token); \
                free(newSymbol); \
                free_list(instList);\
                free_list(dataList);  \
                free(arg1);         \
                free(arg2); \
                free_table(symbols_table, 1); \
                free(symbols_table); \
                free(symbol); \

/* free the dynamically allocated variables and exit with an error code if memory allocation failed */
#define CHECK_MEMORY_ALLOC(x) if (x == NULL) { \
                                    printf("Memory allocation error. Terminating!\n"); \
                                    FREE_ALL \
                                    return 1; \
                                }

/* mark that there was an error, and skip further processing */
#define MARK_ERROR_AND_CONTINUE errors = 1; \
                                continue;

/* both macros check if the argument addressing method is valid for the command.
 * If not, inform the user, mark that there was an error, and skip further processing */
#define CHECK_SRC_ADDRESSING_METHOD(curCommand, addr_method) if (currentCommand->valid_src_addr_methods[addr_method] != '1') \
                                                                ADDR_METHOD_ERROR

#define CHECK_DST_ADDRESSING_METHOD(curCommand, addr_method) if (currentCommand->valid_dst_addr_methods[addr_method] != '1') \
                                                                ADDR_METHOD_ERROR

#define ADDR_METHOD_ERROR { \
                            printf("Error in line %d: invalid addressing method!\n", line_counter); \
                            MARK_ERROR_AND_CONTINUE \
                            }

#define CHECK_MISSING_ARG(arg) if (strcmp(arg, "") == 0) { \
                            printf("Error in line %d: missing argument!\n", line_counter); \
                            MARK_ERROR_AND_CONTINUE \
                            }

#define CHECK_LEGAL_ARG(arg, method, table) if (legal_arg(arg, method, table) != 1) { \
                            printf("Error in line %d: illegal argument! %s\n", line_counter, arg); \
                            MARK_ERROR_AND_CONTINUE \
                            }

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

/* to get to the last char in the line - before the \n */
#define TO_LAST_CHAR 2

/* Input: an assembly file, the commands table and the base filename.
 * Output: returns nothing.
 *
 * Algorithm: First, declare necessary variables, ensure all memory allocations succeeded. Then, read every line in the input
 * assembly file. For each line:
 *  1. reset the relevant variables, update the line counter.
 *  2. get the first token, skip the read chars. If it is a symbol definition, mark it and ensure it is followed by a space.
 *  3. if a new symbol is defined, ensure its name is legal, and it is not empty. If either is correct, issue an explained
 *      error, mark it and continue to the next line of the file. If not, create a new symbol with the received name, after
 *      ensuring it wasn't already defined. Add the new symbol to the symbols table. Read the next token and skip the read chars.
 *  4. check if .data or .string is encountered. If so, set the current symbol's isCode property to true. If the instruction is .string:
 *          read the string itself, ensure it isn't empty, encode it and add it to the data image.
 *     If the instruction is .data: check it isn't empty, and read all the scalars provided, ensuring each is separated by a comma.
 *          For every value, ensure it is a legal scalar, create a node for it and add it to the data image. Issue an error
 *          if the last scalar is followed by a comma.
 *     Check there isn't unnecessary extra text in the line, and continue to the next line.
 *  5. if .entry is encountered, issue a warning in case of symbol definition and skip it, since it will be handled in the
 *     second scan.
 *  6. if .extern is encountered, read the symbol name, ensure it's valid, create a new symbol and add it to the table.
 *     Mark an error if the symbol is was already defined not as an external one, if it was already declared, or if something failed.
 *     Initialize its usages property, and ensure no extra text is written. Continue to the next line.
 *
 *  If the code arrived this stage, it means the token must be a command if it is indeed a legal keyword.
 *  7. search for the command in the table. If not found, issue an error and skip further processing. If the command is found,
 *      set the current symbol's (if there is) isCode property to true. Then, encode the necessary arguments (if needed),
 *      ensuring they are legal, considering both addressing method and the argument itself (a legal scalar, legal register, etc.)
 *     Check there isn't unnecessary extra text in the line.
 *  8. encode the command and its arguments, and add them to the instructions image. If something failed, mark an error and continue.
 *
 *
 * If errors occurred during the first scan, free all the dynamically-allocated variables and return 1 to indicate error.
 * Set the values of ICF & DCF, and then update all the symbols that are .data values - increase by ICF, as well as the
 * Data Image nodes ICs. Return to the beginning of the file, and launch the second scan. If errors occurred during it,
 * free all the dynamically-allocated variables and return 1 to indicate error. Otherwise, create the output files, and
 * free all the dynamically-allocated variables. If files creation failed, 1 is returned. Otherwise, we return 0.
 */
int first_scan(FILE *asmfile, Table *commands_table, char *filename){
    int IC = CODE_LOADING_ADDRESS; /* initialize the instruction counter */
    int DC = 0; /* initialize the data counter */
    int L = 0; /* the number of words the current command is taking */
    int line_counter = 0; /* indicates the current line the assembler handles */
    int errors = 0; /* indicates whether errors were encountered */
    int illegal_scalar = 0; /* marks whether an illegal scalar was encountered in .data */
    char *token = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH); /* used to hold the current token */
    char *symbol = (char *)malloc(sizeof(char) * MAX_SYMBOL_LENGTH); /* used to hold the current symbol's name */
    char *line; /* used to traverse through the input line */
    char *word; /* the current encoded word */
    char *arg1 = (char *)malloc(sizeof(char) * ARG_SIZE); /* allocate memory for the first argument */
    char *arg2 = (char *)malloc(sizeof(char) * ARG_SIZE); /* allocate memory for the second argument */
    enum addr_method arg1_addr_method, arg2_addr_method; /* to hold the addressing method of each argument */

    Command *currentCommand; /* the current command handled */
    cell *currentCell; /* the current cell of the table from which the current command or symbol would be taken */
    Symbol *newSymbol = (Symbol *)malloc(sizeof(Symbol)); /* allocate memory for the symbol variable */

    BinCodeNode *instList = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the head of the instructions List */
    BinCodeNode *currentInstNode = instList; /* holds the current node of the instructions list */
    BinCodeNode *newNode; /* to create more nodes */
    BinCodeNode *dataList = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the head of the data List */
    BinCodeNode *currentDataNode = dataList; /* holds the current node int the data list */

    /* the table to hold the different commands and their properties */
    Table *symbols_table = (Table *)calloc(1, sizeof(Table));

    int ICF, DCF; /* the final values of IC and DC */

    char *line_holder = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH); /* allocate memory for to read the file's lines into */

    /* ensure all memory allocations succeeded. If not, exit the function with an error code */
    CHECK_MEMORY_ALLOC(token)
    CHECK_MEMORY_ALLOC(symbol)
    CHECK_MEMORY_ALLOC(line_holder)
    CHECK_MEMORY_ALLOC(arg1)
    CHECK_MEMORY_ALLOC(arg2)
    CHECK_MEMORY_ALLOC(newSymbol)
    CHECK_MEMORY_ALLOC(instList)
    CHECK_MEMORY_ALLOC(dataList)

    while (fgets(line_holder, MAX_LINE_LENGTH, asmfile) != NULL){ /* read the file, line by line, until EOF is encountered */
        int read; /* to hold the amount of chars read by get_token() */
        int data_store_inst = 0; /* marks whether a data store instruction is in force, and which */
        int new_symbol_flag = 0; /* indicates whether a new symbol is declared */
        line_counter++; /* update the line number */

        line = line_holder;

        if (is_empty_or_comment_line(line_holder)){
            continue; /* if the current line is a comment line or an empty one, we should ignore it */
        }

        read = get_token(line, token, " :"); /* find the next token, might be separated by a space or a colon for a symbol */
        line += read; /* continue to the next chars */

        if (*line++ == ':'){ /* check if the delimiter is :, thus a new symbol is declared, and skip it */
            new_symbol_flag = 1; /* mark that there is a new symbol to store in the symbols table */

            if(!isspace(*line)){ /* ensure there is a space after the colon */
                printf("Error in line %d: no space after label name!\n", line_counter);
                MARK_ERROR_AND_CONTINUE
            }
        }

        if (new_symbol_flag){ /* if a new symbol is defined in the current row */
            if (!legal_symbol(token, commands_table)){ /* mark an error if the symbol's name isn't valid */
                printf("Error in line %d: Illegal symbol name! %s\n", line_counter, token); /* inform the user about the error */
                printf("A legal symbol is at most %d chars length, begins with a letter, not a command nor register"
                       " or keyword, and contains letters and digits only.\n", MAX_SYMBOL_LENGTH); /* explain the rules for a symbol */
                MARK_ERROR_AND_CONTINUE
            } else if (is_empty_or_comment_line(line)){ /* ensure the symbol is not empty */
                printf("Error in line %d: an empty label was defined! %s\n", line_counter, token); /* inform the user about the error */
                MARK_ERROR_AND_CONTINUE
            }

            newSymbol = create_new_symbol(token);
            CHECK_MEMORY_ALLOC(newSymbol)

            /* check if the symbol was already defined */
            if (lookup(newSymbol->name, symbols_table) != NULL){
                printf("Error in line %d: Redefining a symbol %s\n", line_counter, newSymbol->name); /* inform the user about the error */
                MARK_ERROR_AND_CONTINUE
            } else { /* install the new symbol only if it wasn't already defined */
                if (install(newSymbol->name, newSymbol, symbols_table) == NULL){ /* add the symbol to the table. Ensure operation succeeded */
                    printf("Error while adding symbol to table.\n");
                    MARK_ERROR_AND_CONTINUE
                }
            }

            read = get_token(line, token, " "); /* store the "real" token in token */
            line += read; /* skip the chars we have just read */
        }

        if ((data_store_inst = is_data_store_inst(token))){ /* check whether a data storing instruction was encountered */
            if (new_symbol_flag){
                newSymbol->isData = true; /* mark that the symbol is associated with data storing instruction */
                newSymbol->value = DC; /* add the address of the symbol */
            }

            if (data_store_inst == STRING_INST){ /* check if the instruction is .string */
                read = get_token(line, token, "\""); /* store the string in token */
                line += read + 1;

                if (strcmp(token, "") == 0){ /* ensure no empty .string is provided */
                    printf("Error in line %d: empty .string is declared!\n", line_counter);
                    MARK_ERROR_AND_CONTINUE
                }

                currentDataNode->next = encode_string(token, &DC);

                /* proceed to the last item in the list */
                while (currentDataNode->next != NULL){
                    currentDataNode = currentDataNode->next;
                }

                /* ensure no extra text is written */
                CHECK_EXTRA_TEXT
            } else { /* if the instruction is .data */
                read = get_token(line, token, ","); /* store the string in token */
                line += read + 1;

                if (strcmp(token, "") == 0){ /* ensure no empty .data is provided */
                    printf("Error in line %d: empty .data is declared!\n", line_counter);
                    MARK_ERROR_AND_CONTINUE
                }

                while (strcmp(token, "") != 0){ /* read all the data */
                    if (!legal_scalar(token)){ /* ensure only legal scalars are provided */
                        errors = 1; /* indicate that there was an error, and print it */
                        illegal_scalar = 1; /* indicate that an invalid scalar was encountered */
                        printf("Error in line %d: Illegal scalar in .data, or no comma between arguments! %s\n", line_counter, token); /* inform the user about the error */
                        break;
                    }

                    newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the new node */
                    CHECK_MEMORY_ALLOC(newNode) /* ensure memory allocation completed successfully */
                    newNode->code = decimal_to_bin(atoi(token)); /* encode the number */
                    if (newNode->code == NULL){
                        MARK_ERROR_AND_CONTINUE
                    }

                    newNode->IC = DC++; /* set data node value and update DC */
                    newNode->are = A; /* specify ARE attribute */

                    /* add the new node to the list and move the pointer to the current node to the end of the list */
                    ADD_TO_LIST(currentDataNode, newNode)

                    read = get_token(line, token, ","); /* read the next scalar */
                    line += read + 1; /* skip the chars we read */
                }

                /* check if data ends with a comma */
                if (*(line - TO_LAST_CHAR) == ','){
                    printf("Error in line %d: .data cannot end with a comma!\n", line_counter);
                }

                /* ensure no extra text is written */
                if (!illegal_scalar){
                    CHECK_EXTRA_TEXT
                }
            }

            continue; /* continue to the next line */
        }

        if (strcmp(token, ".entry") == 0){
            if (new_symbol_flag){ /* issue a warning if a symbol id declared before .entry */
                printf("line %d - Warning: Symbol definition before .entry\n", line_counter);
            }

            continue; /* if it is an .entry instruction, skip to the next line, since .entry will be handled in the second scan */
        }

        if (strcmp(token, ".extern") == 0){ /* if an external symbol is defined, add it to the symbols table with isExtern=true */
            Symbol *curSymbol;
            cell *curCell;

            if (new_symbol_flag){ /* issue a warning if a symbol id declared before .extern */
                printf("line %d - Warning: Symbol definition before .extern\n", line_counter);
            }

            read = get_token(line, token, " "); /* read the external symbol name */
            line += read + 1; /* skip the characters we read */

            curCell = (cell *)lookup(token, symbols_table); /* search for the name of the label in the current table */

            /* indicate an error if the symbol name isn't valid */
            if (!legal_symbol(token, commands_table)){
                printf("Error in line: %d: problematic external symbol %s\n", line_counter, token); /* inform the user regarding the error */
                MARK_ERROR_AND_CONTINUE
            } else if (curCell != NULL && ((Symbol *)curCell->value)->isExternal == false){ /* error if the symbol is already defined without the external property */
                printf("Error in line: %d: multiple declarations of external symbol %s\n", line_counter, token); /* inform the user regarding the error */
                MARK_ERROR_AND_CONTINUE
            } else if (curCell != NULL && ((Symbol *)curCell->value)->isExternal == true){/* error if an external symbol is re-declared */
                printf("Error in line: %d: multiple declarations of external symbol %s\n", line_counter, token); /* inform the user regarding the error */
                MARK_ERROR_AND_CONTINUE
            }

            curSymbol = create_new_symbol(token);
            CHECK_MEMORY_ALLOC(curSymbol)

            curSymbol->isExternal = true;

            curSymbol->usage = (Usage *)calloc(1, sizeof(Usage));
            CHECK_MEMORY_ALLOC(curSymbol->usage)

            curSymbol->value = 0; /* external symbols value is always 0 */
            if (install(curSymbol->name, curSymbol, symbols_table) == NULL){ /* add the symbol to the table. Ensure operation succeeded */
                printf("Error while adding symbol to table.\n");
                MARK_ERROR_AND_CONTINUE
            }

            /* ensure no extra text is written */
            CHECK_EXTRA_TEXT
            continue;
        }

        /* if we arrived here, it means the line is an instruction line, an assembly command line */
        currentCell = lookup(token, commands_table); /* get the cell of the command */
        if (currentCell == NULL){
            printf("Error in line %d: Invalid command! %s\n", line_counter, token); /* inform the user about the error */
            MARK_ERROR_AND_CONTINUE
        }

        L = 1; /* the current command's size is at least one word - the command name */

        /* if we arrived here, it means the command was found in the table, so we extract it */
        currentCommand = (Command *)currentCell->value;
        L += currentCommand->num_of_args; /* update the num of words - add the amount of arguments */

        if (new_symbol_flag){
            /* if a new symbol is defined, add the code attribute to it */
            newSymbol->isCode = 1;
            newSymbol->value = IC; /* set the value of the symbol to the instruction counter's value */
        }

        /* encode the command word and handle the arguments */
        if (currentCommand->num_of_args == 0){
            word = encode_command(currentCommand->opcode, currentCommand->funct, 0, 0);
        } else if (currentCommand->num_of_args == 1){
            read = get_token(line, arg1, " "); /* get the argument */
            line += read; /* skip the chars we have already read */

            arg1_addr_method = get_addressing_method(arg1); /* determine the addressing method */

            CHECK_MISSING_ARG(arg1) /* ensure there was provided an argument */

            /* check if the addressing method is valid for the command */
            CHECK_DST_ADDRESSING_METHOD(currentCommand, arg1_addr_method)

            /* check if the argument is legal */
            CHECK_LEGAL_ARG(arg1, arg1_addr_method, commands_table)

            word = encode_command(currentCommand->opcode, currentCommand->funct, 0, arg1_addr_method);
        } else {
            read = get_token(line, arg1, ","); /* get the first argument */
            line += read; /* skip the chars we have already read */

            /* check if there is a separating comma. Ensure that the problem is indeed the comma - only if two args were indeed supplied */
            if (*line++ != ',' && is_containing_space_between_words(arg1)){
                printf("Error in line %d: no comma between arguments!\n", line_counter);
                MARK_ERROR_AND_CONTINUE
            }

            arg1_addr_method = get_addressing_method(arg1); /* determine the addressing method */

            read = get_token(line, arg2, " "); /* get the argument */
            line += read; /* skip the chars we have already read */

            arg2_addr_method = get_addressing_method(arg2); /* determine the addressing method */

            CHECK_MISSING_ARG(arg1)
            CHECK_MISSING_ARG(arg2)

            /* check if the addressing methods are valid for the command */
            CHECK_SRC_ADDRESSING_METHOD(currentCommand, arg1_addr_method)
            CHECK_DST_ADDRESSING_METHOD(currentCommand, arg2_addr_method)

            /* check if the arguments arg legal */
            CHECK_LEGAL_ARG(arg1, arg1_addr_method, commands_table)
            CHECK_LEGAL_ARG(arg2, arg2_addr_method, commands_table)

            word = encode_command(currentCommand->opcode, currentCommand->funct, arg1_addr_method, arg2_addr_method);
        }

        /* ensure no extra text is written */
        CHECK_EXTRA_TEXT

        if (word == NULL){ /* if word encoding failed, mark an error and continue */
            printf("Memory allocation error during compilation, line %d\n", line_counter);
            MARK_ERROR_AND_CONTINUE
        }

        newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode));
        CHECK_MEMORY_ALLOC(newNode)
        newNode->code = word;
        newNode->IC = IC++; /* set the address value of the node */
        newNode->L = L; /* set the L attribute */
        newNode->are = A; /* the address of an instruction line is absolute */

        /* add the new node to the list and move the pointer to the current node to the end of the list */
        ADD_TO_LIST(currentInstNode, newNode)

        if (currentCommand->num_of_args == 0){
            continue; /* skip addition of arguments */
        }

        /* add the arguments */

        /* allocate memory for the new node and check if it failed */
        newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode));
        CHECK_MEMORY_ALLOC(newNode)
        newNode->are = A; /* the default ARE is A */

        if (constructArg(newNode, arg1, arg1_addr_method) != 0){ /* if argument encoding failed, mark an error and continue */
            printf("Memory allocation error during compilation, line %d\n", line_counter);
            MARK_ERROR_AND_CONTINUE
        }

        newNode->IC = IC++; /* set the address value of the node */
        newNode->L = line_counter; /* for an argument, set the L property to the line number */

        /* add the new node to the list and move the pointer to the current node to the end of the list */
        ADD_TO_LIST(currentInstNode, newNode)

        if (currentCommand->num_of_args == 1){
            continue; /* skip addition of second argument */
        }

        /* allocate memory for the new node and check if it failed */
        newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode));
        CHECK_MEMORY_ALLOC(newNode)
        newNode->are = A; /* the default ARE is A */

        if (constructArg(newNode, arg2, arg2_addr_method) != 0){ /* if argument encoding failed, mark an error and continue */
            printf("Memory allocation error during compilation, line %d\n", line_counter);
            MARK_ERROR_AND_CONTINUE
        }

        newNode->IC = IC++;/* set the address value of the node */
        newNode->L = line_counter; /* for an argument, set the L property to the line number */

        /* add the new node to the list and move the pointer to the current node to the end of the list */
        ADD_TO_LIST(currentInstNode, newNode)
    }

    if (errors){ /* skip further processing if an error was occurred*/
        FREE_ALL
        return 1;
    }

    currentInstNode = instList->next; /* skip the beginning node */
    currentDataNode = dataList->next; /* skip the beginning node */

    ICF = IC;
    DCF = DC;

    while (currentDataNode != NULL){
        currentDataNode->IC += ICF;
        currentDataNode = currentDataNode->next;
    }

    update_all_symbols_value(symbols_table, ICF);

    rewind(asmfile); /* return to the beginning of the file */
    errors += secondScan(asmfile, symbols_table, instList->next);

    if (errors){
        FREE_ALL
        return 1;
    }

    errors += build_output_files(instList->next, dataList->next, symbols_table, filename, ICF - CODE_LOADING_ADDRESS, DCF);

    FREE_ALL

    return errors; /* indicate whether some errors occurred */
}

/* Input: the symbols table and a value - ICF.
 * Output: returns nothing. Adds to every cell in the table the value to add, to the "value" attribute.
 *
 * Algorithm:
 */
void update_all_symbols_value(Table *symbols_table, unsigned int to_add){
    int i;
    cell *c;

    for (i = 0; i < TABLE_SIZE; i++) { /* go over all the cells */
        for (c = symbols_table->storage[i];  c != NULL ; c = c->next) {
            Symbol *symbol = (Symbol *)c->value; /* get the current symbol */

            if (symbol->isData == true){ /* if it is a data symbol, update its value */
                symbol->value += to_add;
            }
        }
    }
}

/* Input: a string of valid ascii chars.
 * Output: returns a list of nodes that represent the string. Updates the data counter received. If something failed,
 * NULL is returned.
 *
 * Algorithm: Create the first node of the string, ensuring memory allocation succeeded. Then, we skip the opening " if
 * included, and iterate through every char in the received string. For each one, we create a node, setting its value
 * to the binary representation of its numeric value, specifying its address value (and updating the received IC),
 * setting its ARE to A, and finally adding it to the list. We repeat the process to add the last node with the char '\0'.
 * If something failed, NULL is returned. If everything succeeded, we return the node of the first char in the received string.
 */
BinCodeNode *encode_string(char *string, int *DC){
    BinCodeNode *first = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the first node of the string */
    BinCodeNode *cur = first; /* used to add new nodes */
    BinCodeNode *newNode;

    RETURN_IF_MEMORY_ALLOC_ERROR(first, NULL)

    if (*string == '\"'){ /* check if the opening " of the string is included */
        string++; /* skip the opening " */
    }

    for (; *string != '\0'; string++){ /* iterate through the string */
        newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the node of the current char */
        RETURN_IF_MEMORY_ALLOC_ERROR(newNode, NULL)

        newNode->code = decimal_to_bin(*string); /* encode the character */
        RETURN_IF_MEMORY_ALLOC_ERROR(newNode->code, NULL) /* ensure encoding succeeded */
        newNode->IC = *DC; /* specify the memory address of the current char */
        *DC += 1; /* update the data counter */
        newNode->are = A;

        /* add the new node to the list and move the pointer to the current node to the end of the list */
        ADD_TO_LIST(cur, newNode)
    }

    /* add the \0 char to mark the end of the string */
    newNode = (BinCodeNode *)calloc(1, sizeof(BinCodeNode)); /* allocate memory for the node of the current char */
    RETURN_IF_MEMORY_ALLOC_ERROR(newNode, NULL)

    newNode->code = decimal_to_bin('\0'); /* encode the character */
    RETURN_IF_MEMORY_ALLOC_ERROR(newNode->code, NULL) /* ensure encoding succeeded */
    newNode->IC = *DC; /* specify the memory address of the current char */
    *DC += 1; /* update the data counter */
    newNode->are = A;

    /* add the new node to the list and move the pointer to the current node to the end of the list */
    ADD_TO_LIST(cur, newNode)

    cur = first->next; /* save the node to be returned */
    free(first); /* free the first, dummy node */
    return cur; /* return the beginning of the string */
}

/* Input: a string representing the name of the symbol.
 * Output: returns a symbol with the provided name, all additional attributes set to false. If memory Allocation failed,
 * NULL is returned.
 *
 * Algorithm: First, allocate memory for the symbol and its name, and ensure it was completed successfully. If not, return NULL
 * Then, copy the received name to the symbol's name, reset the properties, and return the symbol.
 */
Symbol *create_new_symbol(char *name){
    Symbol *newSymbol = (Symbol *)malloc(sizeof(Symbol)); /* allocate memory for the symbol variable */
    RETURN_IF_MEMORY_ALLOC_ERROR(newSymbol, NULL)

    newSymbol->name = (char *) malloc(sizeof(char) * MAX_SYMBOL_LENGTH); /* allocate memory for the name of the symbol */
    RETURN_IF_MEMORY_ALLOC_ERROR(newSymbol->name, NULL)

    strcpy(newSymbol->name, name);

    /* reset the fields that might not change */
    newSymbol->isData = false;
    newSymbol->isCode = false;
    newSymbol->isEntry = false;
    newSymbol->isExternal = false;

    return newSymbol; /* return the created symbol */
}

/* Input: a list of binary codes to free.
 * Output: returns nothing.
 *
 * Algorithm: Create a variable to store the remaining of the list. Then, until the received pointer is NULL, store in
 * the created node the rest of the list, free the current node, and then use the stored node to "return" to the list.
 */
void free_list(BinCodeNode *head){
    BinCodeNode *curNode = head; /* used to iterate through the received list */

    while (head != NULL){ /* go over the whole list */
        curNode = head->next; /* hold the remaining of the list */
        free(head->code); /* free the code property of the current node */
        free(head); /* free the current node */
        head = curNode; /* proceed to the next node */
    }
}