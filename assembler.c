#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firstScan.h"

int main(int argc, char *argv[]) {

    FILE *fp; /* holds the current file */
    char *filename = (char *) malloc(sizeof(char) * MAX_FILENAME_LENGTH);

    /* the table to hold the different commands and their properties. Use static to automatically initialize the array */
    Table *commands_table = (Table *)malloc(sizeof(Table));

    RETURN_IF_MEMORY_ALLOC_ERROR(filename, MEMORY_ALLOC_ERROR)
    RETURN_IF_MEMORY_ALLOC_ERROR(commands_table, MEMORY_ALLOC_ERROR)

    /* fill the commands table with all the commands, and ensure is completed successfully */
    if (fill_table(commands_table) != 0){
        printf("Commands table creation failed. Terminating...\n"); /* inform the user */
        exit(COMMANDS_TABLE_ERROR); /* exit the program and indicate an error */
    }

    if (argc < 2){ /* check enough arguments were provided */
        printf("Too few arguments!\nUsage: assembler file1 file2....\n"); /* inform the user */
        exit(MISSING_ARGS_ERROR); /* exit the program and indicate an error */
    }

    while (--argc){ /* assemble every file provided */
        /* allocate memory for the filenames, ensure it succeeded */
        char *base = (char *)malloc(sizeof(char) * strlen(argv[argc]));
        RETURN_IF_MEMORY_ALLOC_ERROR(base, MEMORY_ALLOC_ERROR)

        strcpy(base, argv[argc]);
        strcpy(filename, argv[argc]);
        filename = strcat(filename, INPUT_FILE_EXT); /* construct the filename - the base provided and the necessary extension */
        filename[strlen(filename)] = '\0'; /* mark end of string */

        fp = fopen(filename, "r"); /* open the file in read mode */

        if (fp == NULL){ /* ensure the file was opened properly */
            printf("Error opening file: %s\n", filename); /* inform the user */
            continue; /* skip further processing, continue to the next file provided */
        }

        if (first_scan(fp, commands_table, base) != 0){ /* start the assembling process, check if errors occurred */
            printf("Error compiling file: %s\n", filename);
        } else{
            printf("Compilation completed successfully for %s\n", filename);
        }

        fclose(fp); /* close the file */
        free(base); /* free the dynamically allocated variable */
    }

    /* free the dynamically allocated variables */
    free_table(commands_table, 0);
    free(commands_table);
    free(filename);

    return 0;
}
