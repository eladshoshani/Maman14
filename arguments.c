
#include "assembler.h"

/* an array that holds each operation name and the numbers that matches it */
operation operations[] = {{0,  0},
                          {1,  0},
                          {2,  1},
                          {2,  2},
                          {4,  0},
                          {5,  1},
                          {5,  2},
                          {5,  3},
                          {5,  4},
                          {9,  1},
                          {9,  2},
                          {9,  3},
                          {12, 0},
                          {13, 0},
                          {14, 0},
                          {15, 0},
};


int two_operands(int operation, char *arguments) {
    char *temp; /* just for using strtol */
    char *current_operand = strtok(arguments, ",");
    int addressing_method = get_address_method(current_operand);
    int L = 0; /* how many words */
    
    if (addressing_method == ERROR)
        return ERROR;
    
    if (addressing_method == RELATIVE_ADDRESS) {
        /* all of the operations with 2 operands do not support relative addressing method */
        return print_error("Invalid addressing method");
    }
    
    allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS);
    
    instruction_table[IC - START_OF_CODE_ADDRESS]->word3.val = 0; /* reset the cell */
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.a = 1;
    instruction_table[IC -
                      START_OF_CODE_ADDRESS]->word1.opcode = operations[operation].opcode; /* put the opcode in the word */
    instruction_table[IC -
                      START_OF_CODE_ADDRESS]->word1.funct = operations[operation].funct; /* put the funct in the word */
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.source_address = addressing_method;
    
    
    switch (operation) {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
            if (addressing_method == DIRECT_REGISTER_ADDRESS) { /* operand is register */
                instruction_table[IC - START_OF_CODE_ADDRESS]->word1.source_register = which_register(
                        current_operand); /* set the source register */
                break;
            }
            
            if (addressing_method == IMMEDIATE_ADDRESS) {
                L++; /* immediate addressing - one more word */
                allocate_memory_instruction_table(IC + L - START_OF_CODE_ADDRESS);
                instruction_table[IC + L - START_OF_CODE_ADDRESS]->word2.a = 1;
                
                /* we don't need the # , finding the value with strtol */
                instruction_table[IC + L - START_OF_CODE_ADDRESS]->word2.val = strtol(current_operand + 1, &temp, 10);
                break;
            }
            
            L++; /* direct addressing method - one more word */
            break;
        case LEA:
            if (addressing_method != DIRECT_ADDRESS) {
                return print_error("Invalid addressing method");
            }
            L++; /* direct addressing method - one more word */
    }
    
    /*----------the second operand----------*/
    
    current_operand = strtok(NULL, ",");
    addressing_method = get_address_method(current_operand);
    
    if (addressing_method == ERROR)
        return ERROR;
    
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.destination_address = addressing_method;
    
    if (addressing_method == RELATIVE_ADDRESS) {
        /* all of the operations with 2 operands do not support relative addressing method */
        return print_error("Invalid addressing method");
    }
    
    if (operation == CMP && addressing_method == IMMEDIATE_ADDRESS) {
        L++; /* immediate addressing - one more word */
        allocate_memory_instruction_table(IC + L - START_OF_CODE_ADDRESS);
        instruction_table[IC + L - START_OF_CODE_ADDRESS]->word2.a = 1;
        
        /* we don't need the # , finding the value with strtol */
        instruction_table[IC + L - START_OF_CODE_ADDRESS]->word2.val = strtol(current_operand + 1, &temp, 10);
        
        IC += L + 1; /* moving IC to the next available cell */
        return OK;
    }
    
    if (addressing_method == DIRECT_REGISTER_ADDRESS) {
        instruction_table[IC - START_OF_CODE_ADDRESS]->word1.destination_register = which_register(
                current_operand); /* set the destination register */
        
        IC += L + 1; /* moving IC to the next available cell */
        return OK;
    }
    
    
    L++; /* direct addressing method - one more word */
    
    IC += L + 1; /* moving IC to the next available cell */
    return OK;
    
}

int one_operand(int operation, char *operand) {
    char *temp; /* just for using strtol */
    int addressing_method = get_address_method(operand);
    
    if (addressing_method == ERROR)
        return ERROR;
    
    allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS);
    instruction_table[IC - START_OF_CODE_ADDRESS]->word3.val = 0; /* reset the cell */
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.a = 1;
    instruction_table[IC -
                      START_OF_CODE_ADDRESS]->word1.opcode = operations[operation].opcode; /* put the opcode in the word */
    instruction_table[IC -
                      START_OF_CODE_ADDRESS]->word1.funct = operations[operation].funct; /* put the funct in the word */
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.destination_address = addressing_method;
    
    
    switch (operation) {
        case CLR:
        case NOT:
        case INC:
        case DEC:
        case RED:
        case PRN:
            if (addressing_method == DIRECT_REGISTER_ADDRESS) { /* operand is register */
                instruction_table[IC - START_OF_CODE_ADDRESS]->word1.destination_register = which_register(
                        operand); /* set the destination register */
                break;
            }
            
            if (operation == PRN && addressing_method == IMMEDIATE_ADDRESS) { /* prn allowing immediate addressing */
                IC++; /* immediate addressing - one more word */
                operand++; /* we don't need the # */
                allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS);
                instruction_table[IC - START_OF_CODE_ADDRESS]->word2.a = 1;
                instruction_table[IC - START_OF_CODE_ADDRESS]->word2.val = strtol(operand, &temp, 10);
                break;
            }
            
            if (addressing_method != DIRECT_ADDRESS) {
                return print_error("Invalid addressing method");
            }
            
            IC++; /* direct addressing method - one more word */
            
            break;
        case JMP:
        case BNE:
        case JSR:
            if (addressing_method == RELATIVE_ADDRESS ||
                addressing_method == DIRECT_ADDRESS) { /* valid addressing methods */
                IC++; /* direct or relative addressing method - one more word */
                break;
            }
            return print_error("Invalid addressing method");
    }
    
    
    IC++; /* for the first word */
    return OK;
}

void zero_operands(int operation) {
    allocate_memory_instruction_table(IC - START_OF_CODE_ADDRESS);
    instruction_table[IC - START_OF_CODE_ADDRESS]->word3.val = 0; /* reset the cell */
    instruction_table[IC - START_OF_CODE_ADDRESS]->word1.a = 1;
    instruction_table[IC -
                      START_OF_CODE_ADDRESS]->word1.opcode = operations[operation].opcode; /* put the opcode in the word */
    IC++;
}


int get_address_method(char *argument) {
    if (argument[0] == '#') {
        if (!is_number(argument + 1))
            return print_error("Invalid number in immediate addressing method");
        return IMMEDIATE_ADDRESS; /* the number of the addressing method */
    }
    
    if (argument[0] == '&') {
        if (!is_valid_label(argument + 1))
            return print_error("Invalid label in relative addressing method");
        return RELATIVE_ADDRESS; /* the number of the addressing method */
    }
    
    if (is_name_in(registers_names, NUM_REGISTERS, argument)) /* register name */
        return DIRECT_REGISTER_ADDRESS;
    
    /* we are in direct addressing method (number 1) */
    if (!is_valid_label(argument))
        return print_error("Invalid label in direct addressing method");
    
    return DIRECT_ADDRESS;
    
}

int which_register(char *reg) {
    int i;
    for (i = 0; i < NUM_REGISTERS; i++) { /* scan the registers names array */
        if (!strcmp(reg, registers_names[i]))  /* the name is in the array - we found! */
            return i;
    }
    return ERROR; /* reg is not a register */
}
