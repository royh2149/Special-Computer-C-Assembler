/* Input: an integer.
 * Output: returns the num in its binary representation as a string. If something failed, NULL is returned.
 *
 * This function converts a decimal number to a binary one, and returns it as a string.
 */
char *decimal_to_bin(int num);

/* Input: 4 integers representing the operation code, the funct, and the source & destination addressing methods.
 * Output: a string that represents the created word. If something failed, NULL is returned.
 *
 * This function encodes the received command and its addressing methods to binary, and returns NULL in case of failure.
 */
char *encode_command(unsigned int opcode, unsigned int funct, unsigned int src_addr_method, unsigned int dst_addr_method);

/* Input: a string representing a binary number.
 * Output: returns the received number in hexadecimal base. If something failed, NULL is returned.
 *
 * This function receives a binary number as a string and returns a string that represents this number in
 * hexadecimal base.
 */
char *bin_to_hex(char *bin);

/* Input: a string that represents a binary number.
 * Output: returns the received binary number as decimal.
 *
 * This function receives a string that represents a binary number, and returns its decimal value.
 */
int bin_to_decimal(char *bin);

/* Input: a decimal number.
 * Output: returns a char that represents the decimal number in hexadecimal base. If an invalid number was received, '0'
 * is returned.
 *
 * This function receives a number between 0 - 15 and returns a char that represents it in hexadecimal base.
 * If an out-of-range number was entered, the function returns '0'.
 */
char decimal_to_hex(int dec);