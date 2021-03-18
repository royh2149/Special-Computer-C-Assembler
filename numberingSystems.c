#include "numberingSystems.h"
#include "generals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HEX_DIGITS 16

/* Input: an integer.
 * Output: returns the num in its binary representation as a string. If something failed, NULL is returned.
 *
 * Algorithm: first, allocate memory for the string and create the index variable for the loop. If memory allocation failed,
 * NULL is returned. Then, start from the end of the string and add the value of the current bit as a char to the string.
 * Shift the num right to proceed to the next bits, and at the end return the created string.
 */
char *decimal_to_bin(int num){
    char *bin = (char *)malloc(sizeof(char) * (WORD_SIZE+1)); /* allocate the result string and the terminating '\0' */
    int i; /* to be used in the for loop */
    unsigned int num1 = num;

    RETURN_IF_MEMORY_ALLOC_ERROR(bin, NULL) /* ensure memory allocation succeeded */

    for (i = WORD_SIZE - 1; i >= 0; i--){
        /* check if the current bit is 1. If so, add '1' to the string. Else, add '0' to the string */
        bin[i] = (num1 & 1) ? '1' : '0';
        num1 = num1 >> 1; /* proceed to the next bits */
    }

    bin[WORD_SIZE] = '\0'; /* mark end of string */

    return bin; /* return the created string */

}

/* Input: 4 integers representing the operation code, the funct, and the source & destination addressing methods.
 * Output: a string that represents the created word. If something failed, NULL is returned.
 *
 * Algorithm: first, allocate memory for the result string and create a variable to iterate through it and update its content later.
 * ensure memory allocation succeeded, and then, encode all the received attributes, truncate the irrelevant chars of them,
 * and copy them in the necessary order to bin using the itr variable. If an encoding failed, NULL is returned.
 */
char *encode_command(unsigned int opcode, unsigned int funct, unsigned int src_addr_method, unsigned int dst_addr_method){
    char *bin = (char *)malloc(sizeof(char) * (WORD_SIZE+1)); /* create the word */
    char *itr = bin; /* to be used to edit bin */
    char *dam, *sam, *fnc, *opc; /* strings to use to encode the command */

    RETURN_IF_MEMORY_ALLOC_ERROR(bin, NULL) /* ensure memory allocation succeeded */

    dam = decimal_to_bin(dst_addr_method); /* encode the destination address method */
    sam = decimal_to_bin(src_addr_method); /* encode the source address method */
    fnc = decimal_to_bin(funct); /* encode the funct */
    opc = decimal_to_bin(opcode); /* encode the opcode */

    /* ensure all encodings succeeded */
    RETURN_IF_MEMORY_ALLOC_ERROR(dam, NULL)
    RETURN_IF_MEMORY_ALLOC_ERROR(sam, NULL)
    RETURN_IF_MEMORY_ALLOC_ERROR(fnc, NULL)
    RETURN_IF_MEMORY_ALLOC_ERROR(opc, NULL)

    dam += WORD_SIZE - DST_ADDR_METHOD_LENGTH; /* skip the irrelevant chars of the destination address method encoding */
    sam += WORD_SIZE - SRC_ADDR_METHOD_LENGTH; /* skip the irrelevant chars of the source address method encoding */
    fnc += WORD_SIZE - FUNCT_LENGTH; /* skip the irrelevant chars of the funct encoding*/
    opc += WORD_SIZE - OPCODE_LENGTH; /* skip the irrelevant chars of the opcode encoding */

    /* construct the word */
    strncpy(itr, opc, OPCODE_LENGTH); /* copy the operation code to the result string */
    itr += OPCODE_LENGTH; /* proceed to the next chars to edit */
    strncpy(itr, fnc, FUNCT_LENGTH); /* copy the funct to the result string */
    itr += FUNCT_LENGTH; /* proceed to the next chars to edit */
    strncpy(itr, sam, SRC_ADDR_METHOD_LENGTH); /* copy the source address method to the result string */
    itr += SRC_ADDR_METHOD_LENGTH; /* proceed to the next chars to edit */
    strncpy(itr, dam, DST_ADDR_METHOD_LENGTH); /* copy the destination address method to the result string */

    bin[WORD_SIZE] = '\0'; /* mark end of string */
    return bin; /* return the generated word */
}

/* Input: a string representing a binary number.
 * Output: returns the received number in hexadecimal base. If something failed, NULL is returned.
 *
 * Algorithm: first, calculate the size of the string - the size of the received one divided by the size of chunk -
 * the currency is that 4 bits are equal to one hexadecimal digit. Create the result string and a string for the chunk.
 * If memory allocations failed, return NULL. Create a variable for the for loop, and one to hold the current decimal
 * value of the chunk. Then, iterate through every char in the result string. Store the current 4-bits chunk in chunk,
 * convert it to decimal, and set the value of the current char of the result string to its value in hexadecimal.
 * At the end, return the result string.
 */
char *bin_to_hex(char *bin){
    int size = strlen(bin)/BIN_TO_HEX_CURRENCY; /* determine the size of the hex number */
    char *hex = (char *)malloc(sizeof(char) * (size+1)); /* the result to be returned */
    char *chunk = (char *)malloc(sizeof(char) * (BIN_TO_HEX_CURRENCY+1)); /* the current "chunk" - the 4-bit chunk to convert to hex */
    int i, dec; /* to be used in the for loop, for the iteration and to hold the current decimal value of the chunk */


    /* ensure memory allocations succeeded */
    RETURN_IF_MEMORY_ALLOC_ERROR(hex, NULL)
    RETURN_IF_MEMORY_ALLOC_ERROR(chunk, NULL)

    for (i = 0; i < size; i++){ /* fill every char of the result */
        strncpy(chunk, bin, BIN_TO_HEX_CURRENCY); /* copy the current chunk of bits to chunk */
        chunk[BIN_TO_HEX_CURRENCY] = '\0'; /* mark end of chunk */
        dec = bin_to_decimal(chunk); /* get the decimal value of the current chunk */
        bin += BIN_TO_HEX_CURRENCY; /* proceed to the next chunk of bits for later */
        hex[i] = decimal_to_hex(dec); /* convert the number to hexadecimal base and add it to the result string */
    }

    hex[size] = '\0'; /* mark end of string */

    return hex; /* return the received number in hexadecimal base */


}

/* Input: a string that represents a binary number.
 * Output: returns the received binary number as decimal.
 *
 * Algorithm: create a result variable and reset it to zero. Also create a variable that holds the "value" of the current
 * bit (the most right is 1, the second is 2, etc.). then, begin from the end of the number (the last char) and for each
 * char multiply its value (0 or 1) by the value of the current index, and add it to the result. Update the index value
 * variable and continue to the next iteration. At the end, return the result.
 */
int bin_to_decimal(char *bin){
    int result = 0; /* the result to be returned */
    int currentIndexValue = 1; /* represents the value of the current index. The value of the most right bit is one */
    int i; /* to be used in the for loop */

    for (i = strlen(bin)-1; i >= 0; i--){ /* iterate through all the chars */
        result += ((bin[i] == '1') ? 1 : 0) * currentIndexValue; /* add to the result the value of the char times the value of the index */
        currentIndexValue *= 2; /* update the index value variable */
    }

    return result; /* return the decimal number */
}

/* Input: a decimal number.
 * Output: returns a char that represents the decimal number in hexadecimal base. If an invalid number was received, '0'
 * is returned.
 *
 * Algorithm: create an array of chars with the available hex digits. Then, check if the received number is in the valid range,
 * and if so return the matching char using the array. If the number wasn't appropriate, return '0'.
 */
char decimal_to_hex(int dec){
    char digits[HEX_DIGITS] = "0123456789ABCDEF";

    if (dec > 0 && dec < HEX_DIGITS){ /* ensure a valid decimal number was entered */
        return digits[dec]; /* return the appropriate digit */
    }

    return '0'; /* if an invalid decimal was entered, which a decimal not between 0 and 15, return 0 */
}
