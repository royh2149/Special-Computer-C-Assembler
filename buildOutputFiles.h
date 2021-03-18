#include "firstScan.h"

#define OBJ_EXTENSION ".ob"
#define ENT_EXTENSION ".ent"
#define EXT_EXTENSION ".ext"

#define OBJ_VALUE_PADDING 4

/* Input: the list representing "code image", the list representing "data image", a pointer to the symbols table, the base filename,
 * along with the instructions and data counters.
 * Output: builds the output files: .ob, .ent (if entries were defined) and .ext (if there are external symbols). Returns 0 if
 * build completed successfully, 1 otherwise.
 *
 * This function receives the different data necessary to create the output files, and builds them. Returns 0 if all files
 * were built successfully, 1 otherwise.
 */
int build_output_files(BinCodeNode *codeList, BinCodeNode *dataList, Table *symbols_table, char *base_filename, int instSize, int dataSize);

/* Input: the list representing "code image", the list representing "data image", the base filename along with the instructions
 * and data counters.
 * Output: builds the object file. Returns 0 if completed successfully, 1 otherwise.
 *
 * This function receives the code and data images, builds the object file, and returns 0 if the task was completed
 * successfully. If not, 1 is returned.
 */
int build_object_file(BinCodeNode *codeList, BinCodeNode *dataList, char *base_filename, int instSize, int dataSize);

/* Input: an array containing pointers to the different symbols that are entries, its size and the base filename.
 * Output: builds the entry file. Returns 0 if completed successfully, 1 otherwise.
 *
 * This function receives an array of pointers to .entry symbols and its size, builds the .ent file, and returns 0 if the
 * task was completed successfully. If not, 1 is returned.
 */
int build_entry_file(Symbol *entries[], int size, char *base_filename);

/* Input: an array containing pointers to the different symbols that are externals, its size and the base filename.
 * Output: builds the extern file. Returns 0 if completed successfully, 1 otherwise.
 *
 * This function receives an array of pointers to .extern symbols and its size, builds the .ext file, and returns 0 if the
 * task was completed successfully. If not, 1 is returned.
 */
int build_extern_file(Symbol *externs[], int size, char *filename);