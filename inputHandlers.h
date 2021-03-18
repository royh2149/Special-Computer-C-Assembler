#include "commands.h"

#define ARG_SIZE 10

/* Input: a string representing a line of the assembly file.
 * Output: returns 1 if the line contains white spaces only or is a comment line, 0 otherwise.
 *
 * This function receives a line of text, and returns 1 ("true") if its an empty line or a comment line, 0 ("false") otherwise
 */
int is_empty_or_comment_line(char *line);

/* Input: receives 2 strings, one representing a line of text, and the other is a destination string.
 * Output: returns the amount of chars read.
 *         Extracts the next token (from the first no-space char until a char in {delims} is reached) and stores it in the destination string.
 *
 * This function receives a line of text and a destination char pointer (dest), and stores in dest the next token as explained earlier.
 * Returns the amount of chars read.
 * */
int get_token(char *line, char *dest, char *delims);

/* Input: receive a char an a string.
 * Output: returns 1 if c is in delims, 0 otherwise.
 *
 * This function receives a char and a string and returns 1 ("true") if the char is in delims, 0 ("false") otherwise.
 * A space in delims is considered as any white tabs, checked using isspace
 */
int is_char_in_delims(char c, char *delims);

/* Input: receives a string that represents a symbol, and a table in which the symbol must not be.
 * Output: returns 1 if the symbol is legal, 0 otherwise.
 *
 * This function receives a string and a table, and returns true if it is a legal symbol.
 * A legal symbol is at most 31 chars length, begins with a letter, not a command (not in the received table) nor register
 * or keyword, and contains letters and digits only.
 */
int legal_symbol(char *symbol, Table *table);

/* Input: a string.
 * Output: returns a non-zero value representing the instruction if the string is a data storing instruction, 0 otherwise.
 *
 * This function receives a string, and returns a non-zero value ("true") if it is a data storing instruction, and 0 ("false") otherwise
 */
int is_data_store_inst(char *token);

/* Input: a string representing an argument
 * Output: the enum value representing the address method.
 *
 * This function receives a string that represents an argument of a command, and returns its addressing method.
 */
enum addr_method get_addressing_method(char *arg);

/* Input: receives a string that represents a scalar.
 * Output: returns 0 if the scalar isn't a valid one, returns 1 otherwise.
 *
 * This function receives a scalar as a string, and returns 1 if it stands for a valid scalar. If not, 0 is returned.
 * A valid scalar might have a sign (+ or -), and except these contains digits only.
 */
int legal_scalar(char *scalar); /* receives a string that should represent a number; returns 0 if it isn't a valid number, 1 otherwise */

/* Input: the argument as a string, its addressing method and the commands table.
 * Output: returns 1 if the argument is valid, 0 otherwise.
 *
 * This function checks whether the argument is legal - if the method is IMMEDIATE, check if the scalar is legal,
 * if the method is IMD_REGISTER, check if the register is indeed available.
 */
int legal_arg(char *arg, enum addr_method method, Table *commands_table);

/* Input: a string.
 * Output: returns 1 if the string represents a register, 0 otherwise.
 *
 * This function receives a string, and returns 1 if it represents a name of an available register.
 * Else, 0 is returned.
 */
int is_register(char *str);

/* Input: a string.
 * Output: returns 1 if the string contains a space between 2 words, 0 otherwise.
 *
 * This function receives a string of text and returns 1 if it contains a space between 2 words. Otherwise, 0 is returned.
 */
int is_containing_space_between_words(char *c);

/* Input: a string and the commands table.
 * Output: returns 1 if the string is a keyword in our assembly language, 0 otherwise.
 *
 * This function receives a String and the commands table, and returns 1 if the string is either a command in the table
 * or other keyword in the language - entry, extern, data, string or a register name. Else, 0 is returned.
 */
int is_key_word(char *c, Table *commands_table);