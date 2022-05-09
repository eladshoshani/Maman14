
#include "assembler.h"

int second_pass(FILE *source_file) {
    int error_flag = FALSE;
    char line[LINE_LEN];
    IC = 100; /* Initialize the variable */
    
    for (line_number = 1; fgets(line, LINE_LEN,
                                source_file); line_number++) { /* Scanning through each line of the file */
        if (second_pass_handle_line(line) == ERROR)
            error_flag = TRUE; /* There was an error */
    }
    
    return error_flag;
}


int second_pass_handle_line(char line[]) {
    char *p = line;
    char arguments[LINE_LEN];
    int op_or_def;
    
    /* check for remarks and blank lines */
    if (*p == ';')
        return OK;
    p = skip_spaces(p);
    if (!(*p)) /* only spaces in the line */
        return OK;
    
    parse_line_second_pass(p, &op_or_def, arguments);
    
    /* line 3 */
    if (op_or_def == DATA || op_or_def == STRING || op_or_def == EXTERN)
        return OK;
    
    /* lines 4-5 : */
    if (op_or_def == ENTRY) {
        /* adding the label after the .entry to  entry_table_head, we will find the label_value in the add_to_table method */
        add_to_table(&entry_table_head, arguments, find_label_value(arguments));
        return OK;
    }
    
    /* line 6 :*/
    if (op_or_def >= RTS) { /* no operands */
        IC++; /* for the only word in the line */
        return OK;
    }
    
    if (complete_binary_encoding(arguments) == ERROR)
        return ERROR;
    
    return OK;
}

int complete_binary_encoding(char *arguments) {
    int symbol_value;
    char *current_operand = strtok(arguments, ",");
    int addressing_method;
    
    IC++; /* skip the first word */
    
    while (current_operand) { /* the loop will run maximum 2 times (the maximum number of operands) */
        addressing_method = get_address_method(current_operand);
        
        if (addressing_method == IMMEDIATE_ADDRESS) {
            IC++;  /* addressing methods 0 handled in the first pass and took one word */
        }
        
        if (addressing_method == DIRECT_ADDRESS) {
            
            allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS); /* allocate memory for the cell */
            
            symbol_value = find_label_value(current_operand);
            if (symbol_value == ERROR)
                return print_error("Indefinite label operand");
            
            if (is_on_list(extern_table_head,
                           current_operand)) { /* This is an address that represents a line in another source file of the program */
                instruction_table[IC - START_OF_CODE_ADDRESS]->word2.e = 1;
                /* Adds the relevant information address to a list of symbols that refer to an external symbol : */
                add_to_table(&refer_to_an_external_symbol_head, current_operand, IC);
            }
            else { /* This is an address that represents a line in the current source file */
                instruction_table[IC - START_OF_CODE_ADDRESS]->word2.r = 1;
            }
            
            instruction_table[IC - START_OF_CODE_ADDRESS]->word2.val = symbol_value; /* the address of the symbol */
            IC++;
        }
        
        if (addressing_method == RELATIVE_ADDRESS) {
            
            allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS);  /* allocate memory for the cell */
            
            current_operand++; /* we do not need the & at the beginning of the operand */
            
            instruction_table[IC - START_OF_CODE_ADDRESS]->word2.a = 1;
            
            symbol_value = find_label_value(current_operand);
            if (symbol_value == ERROR)
                return print_error("Indefinite label operand");
            
            if (is_on_list(extern_table_head, current_operand)) { /* forbidden in relative addressing */
                return print_error("Using a label defined in another source file, in a relative addressing method");
            }
            
            instruction_table[IC - START_OF_CODE_ADDRESS]->word2.val =
                    symbol_value - (IC - 1); /* the distance between the labels (the current label is on IC - 1) */
            IC++;
        }
        
        /* addressing methods 0,3 handled in the first pass */
        
        current_operand = strtok(NULL, ",");
    }
    
    return OK; /* No errors found and the additional words were successfully added */
    
}


int find_label_value(char *label_name) {
    symbol_table p = code_table_head;
    while (p) { /* search for the label in the symbol table */
        if (!strcmp(p->label_name, label_name)) /* find the label */
            return p->value;
        p = p->next;
    }
    
    /* not in code_table_head */
    
    p = data_table_head;
    while (p) { /* search for the label in the symbol table */
        if (!strcmp(p->label_name, label_name)) /* find the label */
            return p->value;
        p = p->next;
    }
    
    if (is_on_list(extern_table_head, label_name))
        return 0; /* the address is 0 */
    
    return ERROR; /* did not find the label in the symbol table */
    
}

/* Syntax normal line processing */
void parse_line_second_pass(char *line, int *op_or_def, char arguments[]) {
    char *token;
    char *arguments_part;
    
    /* skip the label (if exist): (line 2 in the algorithm) */
    token = strchr(line, ':');
    if (token != NULL)
        line = token + 1;
    
    token = strtok(line, " \t\n");
    
    /* check the second word: */
    *op_or_def = operation_or_definition(token);
    
    /* handle the arguments: */
    arguments_part = token + strlen(token) + 1;
    remove_spaces(arguments_part); /* we do not need any spaces in the argument part */
    strcpy(arguments, arguments_part); /* Saving the argument string */
}
