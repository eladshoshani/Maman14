
#include "assembler.h"

int line_number = 1; /* to track the lines in the file */

/* the data and instruction tables */
word *data_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];
word *instruction_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];

int IC /* Instruction counter */, DC /* Data counter */;
int ICF, DCF; /* the final values */

/* arrays to easily compare the operations, definitions and registers names */
char *operations_names[NUM_OPS] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
                                   "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
char *definitions_names[NUM_DEFS] = {".data", ".string", ".entry", ".extern"};

char *registers_names[NUM_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

int first_pass(FILE *source_file) {
    int error_flag = FALSE;
    char line[LINE_LEN];
    IC = 100, DC = 0; /* Initialize the variables */
    
    line[LINE_LEN - 2] = '\n'; /* to check later if the line length if too big */
    for (line_number = 1; fgets(line, LINE_LEN,
                                source_file); line_number++) {/* Scanning through each line of the file */
        if (handle_line_first_pass(line) == ERROR) /* parse every line */
            error_flag = TRUE;
    }
    
    /* line 18 in the first pass algorithm (saving the final values ​​of IC and DC) */
    ICF = IC;
    DCF = DC;
    
    increase_address_to_data(); /* line 19 */
    
    rewind(source_file); /* Rewinds the file for the second pass and returns a flag regarding a spotted error in the file */
    
    return error_flag;
}

int handle_line_first_pass(char line[]) {
    char *p = line;
    char label[LABEL_LEN];
    char arguments[LINE_LEN];
    int op_or_def;
    int symbol_flag = FALSE;
    
    if (line[LINE_LEN - 2] != '\0' && line[LINE_LEN - 2] != '\n') { /* check if the current line is too long */
        line[LINE_LEN - 2] = '\n'; /* to check the lengths of other lines */
        return print_error("The length of the line is greater than 80"); /* print the error message */
    }
    
    /* check for remarks and blank lines */
    if (*p == ';')
        return OK;
    p = skip_spaces(p);
    if (!(*p)) /* only spaces in the line */
        return OK;
    if (*p == ';') /* error - comment not aat the start of the line */
        return print_error("Start a comment sentence (with ';') that is not at the beginning of the line");
    
    label[0] = '\0'; /* Helps to know if there is a label at the beginning of the line */
    
    if (errors_in_line(p, label, &op_or_def, arguments) == ERROR)
        return ERROR;
    
    if (label[0])
        symbol_flag = TRUE; /* line 3 in the algorithm */
    
    if (op_or_def == DATA || op_or_def == STRING) { /* line 5 in the algorithm */
        if (symbol_flag) { /* line 6 in the algorithm (insert the symbol into the data symbol table) */
            if (add_to_table(&data_table_head, label, DC) == ERROR)
                return ERROR;
        }
        
        /* line 7 in the algorithm */
        if (op_or_def == DATA) {
            insert_numbers_to_data_table(arguments);
        }
        else { /* the definition is .string */
            insert_string_to_data_table(arguments);
        }
        
        return OK; /* line 7 - back to line 2 */
    }
    
    if (op_or_def == ENTRY) {
        if (symbol_flag) /* Prints a warning message on the label before entry */
            printf("Warning: in file \"%s\" in line %d: Label before entry or extern definitions.\n", file_name,
                   line_number);
        return OK; /* line 9 (+8)*/
    }
    
    
    if (op_or_def == EXTERN) { /* line 10 (+8) in the algorithm */
        if (symbol_flag) /* Prints a warning message on the label before entry */
            printf("Warning: in file \"%s\" in line %d: Label before entry or extern definitions.\n", file_name,
                   line_number);
        
        if (is_on_list(extern_table_head,
                       arguments)) /* all OK (there has been a statement of this label in extern already) */
            return OK;
        
        /* adding to the table: */
        if (add_to_table(&extern_table_head, arguments, 0) == ERROR)
            return ERROR;
        return OK;
    }
    
    /* This is an instruction line */
    if (symbol_flag) { /* line 11 in the algorithm */
        if (add_to_table(&code_table_head, label, IC) == ERROR)
            return ERROR;
    }
    
    
    /* lines 13-17 (Analysis of the operands of the instruction) */
    if (op_or_def <= LEA) {
        if (two_operands(op_or_def, arguments) == ERROR)
            return ERROR;
    }
    else if (op_or_def <= PRN) {
        if (one_operand(op_or_def, arguments) == ERROR)
            return ERROR;
    }
    else {
        zero_operands(op_or_def);
    }
    
    return OK; /* The parsing of the current line ended without errors */
}


