/* error codes for the program to exit with */
#define MISSING_ARGS_ERROR 1
#define COMMANDS_TABLE_ERROR 2
#define MEMORY_ALLOC_ERROR 3

/* the required extension for input files */
#define INPUT_FILE_EXT ".as"

/* maximum length of a line in an input file - 80 chars and the '\n' and '\0' */
#define MAX_LINE_LENGTH 82

/* maximum length of a symbol (tag) name - 31 chars and the '\0' */
#define MAX_SYMBOL_LENGTH 32

/* the maximum length of a filename */
#define MAX_FILENAME_LENGTH 1001

/* define values for .string and .data instructions */
#define DATA_INST 1
#define STRING_INST 2

/* the sizes of the different fields of a word */
#define OPCODE_LENGTH 4
#define FUNCT_LENGTH 4
#define SRC_ADDR_METHOD_LENGTH 2
#define DST_ADDR_METHOD_LENGTH 2

/* the size of the word is as the above 4 fields size sum */
#define WORD_SIZE 12

/* a for-bits binary converts to a one char hexadecimal */
#define BIN_TO_HEX_CURRENCY 4

/* the memory address to which the program should be loaded */
#define CODE_LOADING_ADDRESS 100

/* the amount of registers in the machine */
#define NUM_OF_REGISTERS 8

/* used repeatedly to check whether memory allocation succeeded for x. If not, return the received y */
#define RETURN_IF_MEMORY_ALLOC_ERROR(x, y) if (x == NULL) { \
                                    return y; \
                                }

/* add new_node to the list after cur_node, and update cur_node to point at the current last node. Make sure new_node ends the list */
#define ADD_TO_LIST(cur_node, new_node) cur_node->next = new_node; \
                                        cur_node = cur_node->next; \
                                        new_node->next = NULL;

/* an enum that describes the addressing methods */
enum addr_method {IMMEDIATE=0, DIRECT=1, RELATIVE=2, IMD_REGISTER=3};

enum boolean {true=1, false=0};

/* a struct that represents a usage of an external symbol */
struct Usage{
    struct Usage *next;
    unsigned int value;
};

typedef struct Usage Usage;

/* a struct that represents a symbol in the Assembly language */
struct Symbol{
    char *name; /* the name of the symbol */
    unsigned int value; /* the value of the symbol - its address in decimal */
    enum boolean isCode; /* marks whether the symbol is defined before code */
    enum boolean isData; /* marks whether the symbol is .data or .string */
    enum boolean isEntry; /* marks whether the symbol is an entry */
    enum boolean isExternal; /* marks whether the symbol is an external variable */
    Usage *usage; /* stores addresses of usage for .extern symbols */
};

typedef struct Symbol Symbol;

/* an enum for the ARE property of binary code nodes */
enum ARE{A='A', R='R', E='E'};

/* a struct that represents a node in a list of binary codes */
struct BinCodeNode{
    struct BinCodeNode *next; /* the next node */
    char *code; /* the string that represents the binary code - the word value */
    unsigned int IC; /* the instruction counter value of the command */
    unsigned int L; /* the amount of words the command takes, or line number in case of an argument */
    enum ARE are; /* the A,R,E attribute */
};

typedef struct BinCodeNode BinCodeNode;