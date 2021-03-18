#include "tableTools.h"
#include "generals.h"
#include <string.h>
#include <stdlib.h>

#define RAND_HASH_NUM 31

/* Input: a pointer to a symbol.
 * Output: returns nothing.
 *
 * This function frees the dynamically allocated memory used by the received symbol.
 */
void free_symbol(Symbol *symbol);

/* Input: a string.
 * Output: hashes the string and returns the hash value.
 *
 * Algorithm: iterate through the letters of the string, and using their numeric value generate a hash value.
 * Return the modulo of the generated value by the table size, so that the value would be in the right range.
 */
unsigned int hash(char *s){
    unsigned int hashval;

    for (hashval = 0; *s != '\0'; s++){ /* iterate through the received string and determine the hash value */
        hashval = *s + RAND_HASH_NUM * hashval;
    }

    return hashval % TABLE_SIZE; /* return the generated hash, in the right range */
}

/* Input: a table and a key to search in it.
 * Output: returns the value - the cell - of the received key. If there isn't such a cell, NULL is returned.
 *
 * Algorithm: First, hash the received string to determine the location in the array, in which the key might be. Then,
 * iterate over all the cells in the current location, and for each check if its key is the searched one. If so, return
 * the cell. If the key was not found, return NULL.
 */
cell *lookup(char *s, Table *table) {
    cell *ret; /* the value to be returned */

    for (ret = table->storage[hash(s)]; ret != NULL; ret = ret->next){ /* iterate through different cells in the corresponding hash value */
        if (strcmp(s, ret->key) == 0){ /* check if the right cell was reached */
            return ret;
        }
    }

    return NULL; /* in case the key s is not in the table */

}

/* Input: a string as a key, a value, the table to which the row should be added.
 * Output: returns the created cell in the table.
 *
 * Algorithm: first, check if the value is already in the table. If it is, just update the value. If not,
 * allocate memory for the cell and edit its properties as needed. Before exiting, set the cell's value.
 * Returns NULL if something utterly failed, returns the created cell otherwise.
 */
cell *install(char *key, void *value, Table *table){
    cell *ret; /* the cell of the table to be returned */
    unsigned int hashval;

    if ((ret = lookup(key, table)) == NULL) { /* if the key isn't yet in the table */
        ret = (cell *) calloc(1, sizeof(cell)); /* allocate memory for the cell */
        ret->key = (char *)malloc(sizeof(char)*10); /* allocate memory for the name - the key */
        if (ret == NULL || (strcpy(ret->key, key)) == NULL){ /* if an allocation is failed */
            return NULL; /* indicate an error */
        }

        hashval = hash(key); /* hash the key to determine the right index in the array */
        ret->next = table->storage[hashval]; /* keep the previous data */
        table->storage[hashval] = ret; /* add the cell to the table */
    } else { /* if the key is already in the table */
        free((void *) ret->value); /* free the previous value */
    }

    if ((ret->value = value) == NULL){
        return NULL; /* return null if operation failed */
    }

    return ret;
}

/* Input: a table and an integer indicating whether the table contains symbols.
 * Output: returns nothing. Frees all the memory used by the table.
 *
 * Algorithm: iterate through every cell in the array of the table. For each cell, go through every row in the table, and
 * save the next one. If the table stores symbols, free its properties. Otherwise, free it right away. In any case,
 * free the name of the row, its key, and free the row itself.
 */
void free_table(Table *table, int is_symbol){
    int i; /* used in the for loop */
    cell *c; /* holds the current cell of the table */

    for (i = 0; i < TABLE_SIZE; i++) { /* go through the cells of the table's array */
        for (c = table->storage[i];  c != NULL ;) { /* iterate through every row */
            /* these variables are used to go through the rows in the current cell */
            cell *temp = c;
            c = c->next;

            if (is_symbol){ /* if the table stores symbols, use the special method to free it */
                free_symbol((Symbol *)temp->value);
            } else { /* else, free the value of the cell */
                free(temp->value);
            }

            free(temp->key); /* free the name of the cell, its key */
            free(temp); /* free the cell itself */
        }
    }
}

/* Input: a pointer to a symbol.
 * Output: returns nothing.
 *
 * Algorithm: first, ensure the symbol is valid and no errors occurred during the program execution. Then, free its name.
 * If it is an .extern symbol, iterate through its usages to free them as well. In any case, free the symbol itself.
 */
void free_symbol(Symbol *symbol){
    if (symbol == NULL || symbol->name == NULL){ /* ensure the properties were allocated successfully */
        return;
    }

    free(symbol->name); /* free the name of the symbol */

    /* if the symbol is a .extern one, iterate through its usages and free them all */
    if (symbol->isExternal == true){
        Usage *usage = symbol->usage; /* get the first usage */
        Usage *curUsage = usage; /* a variable to be used to go through the usages */

        while (usage != NULL){ /* free all the usages */
            curUsage = usage->next; /* hold the next usage */
            free(usage); /* free the current usage */
            usage = curUsage; /* proceed to the next usage */
        }
    }

    free(symbol); /* free the symbol itself */
}

