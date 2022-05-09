
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*-----------Constants-----------*/

#define START_OF_CODE_ADDRESS 100 /* The address of the first word in the code */
#define LINE_LEN 82 /* The maximal length of an assembly code line */
#define LABEL_LEN 32 /* The maximal length of label (including '\0' at the end) */
#define MAX_INSTRUCTION_AND_DATA_TABLE_LEN 500 /* The maximal length of the machine code */

#define NUM_OPS 16 /* Number of operation in the assembly language */
#define NUM_DEFS 4 /* Number of definitions in the assembly language */
#define NUM_REGISTERS 8 /* Number of registers in the assembly language */

enum bool { /* boolean constants */
    FALSE = 0, TRUE
};

enum status {
    ERROR = -1, OK = 0
};

enum reserved_words_num { /* Matching the operations and definitions in the assembly language to numbers */
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, JSR, RED, PRN, RTS, STOP,
    DATA, STRING, ENTRY, EXTERN
};

enum addressing_methods { /* Constants for classification addressing methods */
    IMMEDIATE_ADDRESS, DIRECT_ADDRESS, RELATIVE_ADDRESS, DIRECT_REGISTER_ADDRESS
};


/*----------structs----------*/

/* types of words (all of them will set up the word we will use) */
struct word1 {
    unsigned int e: 1;
    unsigned int r: 1;
    unsigned int a: 1;
    unsigned int funct: 5;
    unsigned int destination_register: 3;
    unsigned int destination_address: 2;
    unsigned int source_register: 3;
    unsigned int source_address: 2;
    unsigned int opcode: 6;
};

struct word2 {
    unsigned int e: 1;
    unsigned int r: 1;
    unsigned int a: 1;
    unsigned int val: 21;
};

struct word3 {
    unsigned int val: 24;
};

typedef union { /* This struct represents a word in the assembly code */
    struct word1 word1;
    struct word2 word2;
    struct word3 word3;
} word;


typedef struct node { /* linked list */
    struct node *next;
    char *label_name;
    int value;
} node;

typedef struct node *symbol_table; /* Pointer to a node in a linked list */

typedef struct { /* In order to create an array that holds each operation name and the numbers that matches it */
    int opcode;
    int funct;
} operation;

/*----------variables----------*/

extern char *file_name /* The name of the current file */;

extern int line_number; /* The line number in the current file */
extern int IC /* Instruction counter */, DC /* Data counter */;
extern int ICF, DCF; /* the final values */

/* arrays to easily compare the operations, definitions and registers names */
extern char *operations_names[NUM_OPS];
extern char *definitions_names[NUM_DEFS];
extern char *registers_names[NUM_REGISTERS];

/* The heads of the lists that contain the symbols: */
extern symbol_table data_table_head;
extern symbol_table code_table_head;
extern symbol_table entry_table_head;
extern symbol_table extern_table_head;
extern symbol_table refer_to_an_external_symbol_head;

/* the data and instruction tables */
extern word *data_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];
extern word *instruction_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];


/*-------------statements on functions-------------*/


/*----------first pass----------*/

/**
 * Preforms the first pass on the given source file, searching for most of the syntax errors,
 * and translating some of the .as file into machine code.
 *
 * @param source_file - a pointer to the given file
 * @return TRUE or FALSE, according to the correctness of the file's syntax (FALSE if no error is found)
 */
int first_pass(FILE *source_file);

/**
 * Executes the first pass algorithm on the current line, searching for the syntax errors in the line,
 * and translating some of the .as file into machine code.
 *
 * @param line the current line
 * @return ERROR or OK, according to the correctness of the line
 */
int handle_line_first_pass(char line[]);


/*----------line errors----------*/

/**
 * A function that receives a line, divides it into parts (for example label_name and arguments)
 * and checks if it has syntax errors, if there are errors printed error message accordingly.
 *
 * @param line The line we check errors in (and parse)
 * @param label_name The label name to be passed to the first pass function
 * @param op_or_def A number representing the operation or definition in the line
 * @param arguments The arguments string to be passed to the first pass function
 * @return ERROR if the line is with errors and OK otherwise
 */
int errors_in_line(char *line, char label_name[], int *op_or_def, char arguments[]);

/**
 *  A function that receives a string and returns a pointer to the first non-white character in the string
 * @param s the given string
 * @return a pointer to the first non-white character in the string
 */
char *skip_spaces(char *s);

/**
 * Function to remove all spaces from a given string
 * @param str the string we want to remove the spaces from
 */
void remove_spaces(char *str);

/**
 * Checks if the given label is valid or not
 * @param name the name of the possible label
 * @return TRUE if the label is valid and FALSE otherwise
 */
int is_valid_label(char *name);

/**
 * checks if name is in the given array
 * @param arr the names array
 * @param arr_size how many cells arr have
 * @param name the name we are looking for in arr
 * @return TRUE if the name is in the given array and FALSE otherwise
 */
int is_name_in(char *arr[], int arr_size, char *name);

/**
 * Prints to stderr a custom error message plus the file name and the line number where the error is
 * @param the_error The error message to be printed
 * @return the value ERROR
 */
int print_error(char *the_error);

/**
 * Prints to stderr a custom error message (without file name and line number)
 * @param the_error The error to be printed
 * @return the value ERROR
 */
int print_error_only_message(char *the_error);

/**
 * Classified if the given name (which supposed to be an operation or definition) is valid or not
 * @param name the name of the possible operation or definition
 * @return ERROR if the word is not an operation or definition, and a number representing the name otherwise
 */
int operation_or_definition(char *name);

/**
 * Checks whether the given string is an integer
 * @param s the given string
 * @return TRUE if s is an integer and FALSE otherwise
 */
int is_number(char *s);

/**
 * A function that checks commas errors in the arguments part in a line.
 * In addition, the function counts the number of arguments
 * if there is an error we print an appropriate error message
 *
 * @param s The string we check (the arguments)
 * @param count_arguments count how many arguments there are in this variable
 * @return ERROR if we found a comma error and OK otherwise
 */
int check_commas(char *s, int *count_arguments);

/**
 * Checks whether the part of the line after the .string definition is a valid string
 * In addition if the string is not valid then an appropriate error message is printed
 *
 * @param s the part of the line after the .string definition
 * @return ERROR if s is not valid and OK otherwise
 */
int is_valid_string(char *s);

/**
 * check if the line has ':' in it (and that the ':' is not inside a string)
 * @param s the line
 * @return TRUE if we found ':' in the line (not inside a string) and FALSE otherwise
 */
int has_colon(char *s);

/*----------second pass----------*/

/**
 * Preforms the second pass on the given file, searching for the rest of the errors.
 * mainly responsible for extracting label information from the symbol table and inserting it into the machine code.
 *
 * @param source_file - a pointer to the current file
 * @return TRUE or FALSE, according to the correctness of the file's syntax (FALSE if no error is found)
 */
int second_pass(FILE *source_file);

/**
 * Executes the second pass algorithm on the current line,
 * searching for additional errors (especially those related to labels),
 * and build the rest of the machine code.
 *
 * @param line the current line
 * @return ERROR or OK, according to the correctness of the line
 */
int second_pass_handle_line(char line[]);

/**
 * complete the words from the current arguments (The ones that was not built in the first pass).
 *
 * @param arguments the arguments part in the current line
 * @return ERROR if an error is found (for example incorrect use of addressing method), and OK otherwise
 */
int complete_binary_encoding(char *arguments);

/**
 * The method searches in the symbol table for the value of a specific label.
 *
 * @param label_name The name of the label we are looking for
 * @return the label value or ERROR if we did not find the label in the symbol table
 */
int find_label_value(char *label_name);

/**
 * the function "divides" the line into its parts (like the arguments parts).
 *
 * @param line The line we check errors in (and parse)
 * @param op_or_def A number representing the operation or definition in the line
 * @param arguments The arguments string to be passed to the first pass function
 */
void parse_line_second_pass(char *line, int *op_or_def, char arguments[]);

/*----------tables----------*/

/**
 * initializes the symbol tables heads to NULL
 */
void initialize_symbol_table_heads();

/**
 * allocate memory for a specific cell in the instruction_table
 * @param index the index of the cell
 */
void allocate_memory_instruction_table(int index);

/**
 * allocate memory for a specific cell in the data_table
 * @param index the index of the cell
 */
void allocate_memory_data_table(int index);

/**
 * allocate memory for a new symbol table node
 *
 * @param node the new node we want to allocate memory for
 * @param label_name the label name
 */
void allocate_memory_symbol_table(symbol_table *node, char *label_name);

/**
 * A function that inserts a label to a list (given head of the list)
 *
 * @param head The head of the list
 * @param label_name the label name we want to insert into the list
 * @param label_value the address of the label
 * @return Error if an error occurred and OK if we added the label successfully
 */
int add_to_table(symbol_table *head, char *label_name, int label_value);

/**
 * Checks whether a given label name is on a list whose head is given
 *
 * @param head The head of the list
 * @param label_name the label name
 * @return True if the label is in the list and False otherwise
 */
int is_on_list(symbol_table head, char *label_name);

/**
 * Checks whether the label we are trying add to a symbol table list is not already on the list,
 * and whether it is "valid" to insert the label or is it an error, if there is an error, an error message is printed.
 *
 * @param head the head of the list we are trying add the label to
 * @param label_name the label name
 * @return ERROR if there is an error and OK otherwise
 */
int check_for_in_the_list(symbol_table *head, char *label_name);

/**
 * Increases the address of each symbol with type of data by ICF (line 19 in first_pass algorithm)
 */
void increase_address_to_data();

/**
 * Enter a number to the data table
 * @param num the number we want to enter
 */
void insert_number_to_data_table(int num);

/**
 * Enter a list of numbers to the data table
 * @param numbers a string of all the number separated by commas only
 */
void insert_numbers_to_data_table(char *numbers);

/**
 * insert every char to in the string to the data table
 * @param s the string (with the " at the beginning and at the end)
 */
void insert_string_to_data_table(char *s);

/*----------arguments----------*/

/**
 * Creating machine code for an operation with zero operands (only one word)
 *
 * @param operation a number which represents the operation (can be only RTS or STOP)
 * @return ERROR or OK, according to the correctness of the operand and addressing method
 */
void zero_operands(int operation);

/**
 * Creating machine code for an operation with one operand (only first word and word of immediate address)
 * @param operation a number which represents the operation
 * @param operand the operand
 * @return ERROR or OK, according to the correctness of the operand and addressing method
 */
int one_operand(int operation, char *operand);

/**
 * Creating machine code for an operation with 2 operands (only first word and words of immediate address)
 * @param operation a number which represents the operation
 * @param arguments the 2 operands separated by commas only
 * @return ERROR or OK, according to the correctness of the operands and addressing method
 */
int two_operands(int operation, char *arguments);

/**
 * Gets the addressing method of the operand, stored in the argument parameter
 *
 * @param argument the given operand in the instruction line
 * @return the number of the addressing method, according to the addressing_methods enum,
 *         or ERROR if it's given an invalid operand
 */
int get_address_method(char *argument);

/**
 * A function that receives a register's name and returns its number
 *
 * @param reg the register name
 * @return the register number, or ERROR if reg is not one of the registers
 */
int which_register(char *reg);


/*----------files----------*/

/**
 * Function that runs the assembler on all the files received as command line arguments
 *
 * @param argc the number of files
 * @param argv an array with the files names (without the .as)
 */
void open_files(int argc, char *argv[]);

/**
 * Function that runs the assembler on the file with the name it receives
 *
 * @param name the name of the current file (without the .as)
 */
void open_file(char *name);

/**
 * Gets a file pointer (with reading permission with the given name end ending)
 *
 * @param name the file's name
 * @param ending the file's ending (.as for example)
 * @return a  reading permission file pointer to the file
 */
FILE *get_file(char *file_name, char *file_ending);

/**
 * Writes the output files for a given file (we use this function only if no errors were found in the file)
 *
 * @param name the name of the given file, without ".as" ending
 */
void write_output_files(char *name);

/**
 * Writes the ".ob" file
 * @param name - the name of the current file, without ".as" ending
 */
void write_ob_file(char *name);

/**
 * Writes the ".ent" file
 * @param name - the name of the current file, without ".as" ending
 */
void write_ent_file(char *name);

/**
 * Writes the ".ext" file
 * @param name - the name of the current file, without ".as" ending
 */
void write_ext_file(char *name);
