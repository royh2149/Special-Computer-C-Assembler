#include "buildOutputFiles.h"
#include "numberingSystems.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* the format for the values addresses - four zeros padding */
#define VALUE_PADDING "%04d"
#define UNSIGNED_VALUE_PADDING "%04u"

/* check if the memory allocation for the array was successful - if not, inform the user, free both and return 1 */
#define CHECK_ARR_ALLOC(x) if (x == NULL){ \
                                printf("Memory allocation error! File creation failed.\n"); \
                                free(externals); \
                                free(entries); \
                                return 1; \
                            }

/* check whether the file was opened\created successfully - if not, inform about the error and return 1 */
#define CHECK_FILE_OPEN(x) if (outputFile == NULL){ \
                                printf("Error creating object file for %s!\n", filename); \
                                return 1; \
                            }

/* Input: the list representing "code image", the list representing "data image", a pointer to the symbols table, the base filename,
 * along with the instructions and data counters.
 * Output: builds the output files - .ob, .ent (if entries were defined) and .ext (if there are external symbols). Returns 0 if
 * build completed successfully, 1 otherwise.
 *
 * Algorithm: first, allocate memory for the externals and entries array, and ensure it succeeded. Then, create the object
 * files and return 1 if an error occurred, after freeing the arrays. Then, iterate through all the symbols in the table,
 * and fill the externals and entries arrays. At last, create both the .ent file and .ext file - only if needed! If an
 * error occurred, return 1 - otherwise, 0 is returned.
 */
int build_output_files(BinCodeNode *codeList, BinCodeNode *dataList, Table *symbols_table, char *base_filename, int instSize, int dataSize){
    int ext_size = 1; /* current size of the array of .extern symbols */
    int ent_size = 1; /* current size of the array of .entry symbols */

    int i; /* for the for loop */
    cell *c;

    /* allocate memory for the .extern and .entry symbols arrays */
    Symbol **externals = (Symbol **)malloc(sizeof(Symbol *) * ext_size);
    Symbol **entries = (Symbol **)malloc(sizeof(Symbol *) * ent_size);

    /* ensure all memory allocations succeeded */
    CHECK_ARR_ALLOC(externals)
    CHECK_ARR_ALLOC(entries)

    /* build the object file. If operation failed, return 1 */
    if (build_object_file(codeList, dataList, base_filename, instSize, dataSize) != 0){
        free(externals);
        free(entries);
        return 1;
    }

    for (i = 0; i < TABLE_SIZE; i++) {
        for (c = symbols_table->storage[i];  c != NULL ; c = c->next) { /* iterate through all the symbols */
            Symbol *symbol = (Symbol *)c->value; /* get the current symbol */

            if (symbol->isEntry == true){ /* check if the current symbol is a .entry one */
                entries[ent_size-1] = symbol; /* add the symbol to the array */
                ent_size++; /* update the size of the entries array */

                entries = (Symbol **)realloc(entries,sizeof(Symbol *) * ent_size); /* reallocate memory for more entries */
                CHECK_ARR_ALLOC(entries) /* ensure memory allocation succeeded */
            }

            if (symbol->isExternal == true){ /* check if the current symbol is a .extern one */
                externals[ext_size-1] = symbol; /* add the symbol to the array */
                ext_size++; /* update the size of the externals array */

                externals = (Symbol **)realloc(externals, sizeof(Symbol *) * ext_size); /* reallocate memory for more entries */
                CHECK_ARR_ALLOC(externals) /* ensure memory allocation succeeded */
            }
        }
    }

    /* build the entries file only if there are .entry symbols */
    if (ent_size > 1 && build_entry_file(entries, ent_size, base_filename) != 0){ /* build the entries file. If operation failed, return 1 */
        return 1;
    }

    /* build the externals file only if there are .extern symbols */
    if (ext_size > 1 && build_extern_file(externals, ext_size, base_filename) != 0){ /* build the externals file. If operation failed, return 1 */
        return 1;
    }

    /* free the dynamically allocated arrays */
    free(externals);
    free(entries);

    return 0; /* if we arrived here, all went well and we return 0 */
}

/* Input: the list representing "code image", the list representing "data image", the base filename along with the
 * instructions and data counters.
 * Output: builds the object file. Returns 0 if completed successfully, 1 otherwise.
 *
 * Algorithm: first, allocate memory for the filename and construct it (after ensuring memory allocation didn't fail) -
 * concatenate the "base" name with the .ob extension. Then, open the file, and return 1 if an error occurred.
 * Write the size of the instructions image and data image at the head of the file, and then each of the instructions
 * in the required format. Then, in the same format, write the data image to the file. Before returning 0, close the file
 * and free the string.
 */
int build_object_file(BinCodeNode *codeList, BinCodeNode *dataList, char *base_filename, int instSize, int dataSize){
    char *filename = (char *)malloc(sizeof(char) * strlen(base_filename)); /* allocate memory for the filename */
    FILE *outputFile;

    RETURN_IF_MEMORY_ALLOC_ERROR(filename, 1) /* ensure memory allocation succeeded */

    /* construct the filename and create the file */
    strcpy(filename, base_filename);
    outputFile = fopen(strcat(filename, OBJ_EXTENSION), "w"); /* create the object file in write mode */

    CHECK_FILE_OPEN(outputFile) /* ensure file created successfully */

    /* write the size of the instructions image and data image at the head of the file */
    fprintf(outputFile, "\t%d %d\n", instSize, dataSize); /* write to the file the sizes of the instructions and data images */

    while (codeList != NULL){ /* write all the code to the file */
        fprintf(outputFile, VALUE_PADDING " %s %c\n", codeList->IC, bin_to_hex(codeList->code), codeList->are);
        codeList = codeList->next; /* continue to the next node */
    }


    while (dataList != NULL){ /* write all the data to the file */
        fprintf(outputFile, VALUE_PADDING " %s %c\n", dataList->IC, bin_to_hex(dataList->code), dataList->are);
        dataList = dataList->next; /* continue to the next node */
    }

    fclose(outputFile); /* close the file */
    free(filename); /* free the string */

    return 0; /* if the code arrived here, it means all went successfully so we return 0 */
}

/* Input: an array containing pointers to the different symbols that are entries, its size and the base filename.
 * Output: builds the entry file. Returns 0 if completed successfully, 1 otherwise.
 *
 * Algorithm: first, allocate memory for the filename and construct it (after ensuring memory allocation didn't fail) -
 * concatenate the "base" name with the .ent extension. Then, open the file, and return 1 if an error occurred. Then,
 * iterate through the received array, and for each symbol write a line containing its name and value. Before returning 0,
 * close the file and free the string.
 */
int build_entry_file(Symbol *entries[], int size, char *base_filename){
    char *filename = (char *)malloc(sizeof(char) * strlen(base_filename)); /* allocate memory for the filename */
    FILE *outputFile;
    int i; /* for the for loop */

    RETURN_IF_MEMORY_ALLOC_ERROR(filename, 1) /* ensure memory allocation succeeded */

    /* construct the filename and create the file */
    strcpy(filename, base_filename);
    outputFile = fopen(strcat(filename, ENT_EXTENSION), "w"); /* create the entries file in write mode */

    CHECK_FILE_OPEN(outputFile) /* ensure file created successfully */

    for (i = 0; i < size - 1; i++) { /* write all the entries to the file */
        /* write the name of the entry symbol and the value (address) to the file */
        fprintf(outputFile, "%s " VALUE_PADDING "\n", entries[i]->name, entries[i]->value);
    }

    fclose(outputFile); /* close the file */
    free(filename); /* free the string */

    return 0; /* if the code arrived here, it means all went successfully so we return 0 */
}

/* Input: an array containing pointers to the different symbols that are externals, its size and the base filename.
 * Output: builds the extern file. Returns 0 if completed successfully, 1 otherwise.
 *
 * Algorithm: first, allocate memory for the filename and construct it (after ensuring memory allocation didn't fail) -
 * concatenate the "base" name with the .ext extension. Then, open the file, and return 1 if an error occurred. Then,
 * iterate through the received array, and for each symbol write all its usages: each usage in a separate line,
 * containing the name of the symbol and the address in which it is used. Before returning 0, close the file and free the string.
 */
int build_extern_file(Symbol *externs[], int size, char *base_filename){
    char *filename = (char *)malloc(sizeof(char) * strlen(base_filename)); /* allocate memory for the filename */
    FILE *outputFile;
    int i; /* for the for loop */

    RETURN_IF_MEMORY_ALLOC_ERROR(filename, 1) /* ensure memory allocation succeeded */

    /* construct the filename and create the file */
    strcpy(filename, base_filename);
    outputFile = fopen(strcat(filename, EXT_EXTENSION), "w"); /* create the externs file in write mode */

    CHECK_FILE_OPEN(outputFile) /* ensure file created successfully */

    for (i = 0; i < size - 1; i++) { /* write all the entries to the file */
        Usage *usage = externs[i]->usage->next; /* get the first usage */

        /* write the name of the extern symbol and the values (addresses) in which is it used to the file */
        while (usage != NULL){
            fprintf(outputFile, "%s " UNSIGNED_VALUE_PADDING "\n", externs[i]->name, usage->value);
            usage = usage->next; /* continue to the next usage */
        }
    }

    fclose(outputFile); /* close the file */
    free(filename); /* free the string */

    return 0; /* if the code arrived here, it means all went successfully so we return 0 */
}