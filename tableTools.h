/* acknowledgment: the functions are profoundly inspired by section 6.6 in the book "The C programming language"
 * by Dennis M. Ritchie and Brian W. Kernighan */

#define TABLE_SIZE 50

/* a cell in the table */
struct cell{
    struct cell *next; /* to hold the next cell in case of duplicate items with the same hash value */
    char *key; /* the key of the cell, to be searched by */
    void *value; /* to hold the value of the key - it will be a struct */
};

typedef struct cell cell;

/* the table - an array of cells */
struct Table{
    cell *storage[TABLE_SIZE];
};

typedef struct Table Table;

/* Input: a string.
 * Output: hashes the string and returns the hash value.
 *
 * This function hashes a received string based on the hash size to store it in a table.
 */
unsigned int hash(char *s);

/* Input: a table and a key to search in it.
 * Output: returns the value - the cell - of the received key. If there isn't such a cell, NULL is returned.
 *
 * This function receives a table and a key to search in it, and returns the corresponding cell. If the key is not
 * found, NULL is returned.
 */
cell *lookup(char *s, Table *table);

/* Input: a string as a key, a value, the table to which the row should be added.
 * Output: returns the created cell in the table.
 *
 * This function adds a cell to the table. Returns NULL if something utterly failed, returns the created cell otherwise.
 */
cell *install(char *key, void *value, Table *table);

/* Input: a table and an integer indicating whether the table contains symbols.
 * Output: returns nothing. Frees all the memory used by the table.
 *
 * This function receives a table and free all the dynamically-allocated memory it uses.
 */
void free_table(Table *table, int is_symbol);