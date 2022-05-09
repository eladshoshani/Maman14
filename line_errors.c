
#include "assembler.h"
#include <ctype.h>


int errors_in_line(char *line, char label_name[], int *op_or_def, char arguments[]) {
    char *token;
    char *arguments_part; /* to save the arguments parts of the line */
    int count_arguments = 0; /* the number of arguments in the line (after the operation or definition */
    
    /* check the label (if exists): */
    if (has_colon(line)) {
        /* have : in the line - the word before it must be the label (and the : is not inside a string) */
        token = strtok(line, ":");
        if (is_valid_label(token) == FALSE)
            return print_error("Invalid label");
        strcpy(label_name, token); /* the label is good - we will save it */
        token = strtok(NULL, " \t\n"); /* for operation or definition word */
    }
    else { /* have no label */
        token = strtok(line, " \t\n"); /* for operation or definition word */
    }
    
    if (!token) /* there is no operation or definition */
        return print_error("Missing operation or definition");
    
    /* check the operation or definition: */
    *op_or_def = operation_or_definition(token);
    if (*op_or_def == ERROR)
        return ERROR;
    
    /* The arguments treatment: */
    
    arguments_part = token + strlen(token) + 1;
    
    if (*op_or_def != STRING && check_commas(arguments_part, &count_arguments) == ERROR)
        return ERROR;
    
    /* commas are valid */
    
    if (*op_or_def != STRING) { /* copying the arguments part without the spaces */
        remove_spaces(arguments_part); /* we do not need any spaces in the argument part */
        
        strcpy(arguments, arguments_part); /* Saving the argument string */
    }
    
    switch (*op_or_def) {
        
        /* the second word is an operation with 2 operands */
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            if (count_arguments != 2) /* check if the number of arguments is 2 */
                return print_error("The number of arguments does not fit the operation");
            break;
            
            /* the second word is an operation with 1 operand */
        case CLR:
        case NOT:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case JSR:
        case RED:
        case PRN:
            if (count_arguments != 1) /* check if the number of arguments is 1 */
                return print_error("The number of arguments does not fit the operation");
            break;
            
            /* the second word is an operation without operands */
        case RTS:
        case STOP:
            if (count_arguments > 0) /* check extraneous text */
                return print_error("Extraneous text after end of command");
            break;
            
            /* the second word is a definition */
        case DATA:
            if (!count_arguments) /* no arguments */
                return print_error("Missing number after .data");
            
            token = strtok(NULL, ",");
            while (token != NULL) { /* check all the numbers */
                if (!is_number(token))
                    return print_error("Invalid number");
                token = strtok(NULL, ",");
            }
            break;
        case STRING:
            arguments_part = skip_spaces(arguments_part); /* skip the spaces at the beginning */
            if (is_valid_string(arguments_part) == ERROR)
                return ERROR;
            
            /* Temporarily saves the number of characters to be copied from arguments_part to arguments: */
            count_arguments = strrchr(arguments_part, '\"') - arguments_part + 1;
            
            strncpy(arguments, arguments_part,
                    (count_arguments)); /* coping only the string (with the ") */
            
            arguments[count_arguments] = '\0'; /* null character manually added */
            
            break;
        case ENTRY:
        case EXTERN:
            if (count_arguments != 1) { /* error - we will classify it */
                if (!count_arguments) /* no arguments */
                    return print_error("Missing label after .entry or .extern definition");
                return print_error("More than 1 argument after .entry or .extern definition");
            }
            
            if (is_valid_label(arguments_part) == FALSE) /* check the label after the .extern or .entry */
                return print_error("Invalid label after .entry or .extern definition");
            break;
    }
    
    return OK; /* everything is good with the line */
}


int print_error(char *the_error) {
    fprintf(stderr, "Error: in file \"%s\" in line %d: %s.\n", file_name, line_number, the_error);
    return ERROR;
}


int print_error_only_message(char *the_error) {
    fprintf(stderr, "%s.\n", the_error);
    return ERROR;
}

int has_colon(char *s) {
    int i;
    
    /* check for ':' before a " (because if ':' comes after " then it is considered inside a string): */
    for (i = 0; s[i] && s[i] != '\"'; i++) {
        if (s[i] == ':')
            return TRUE;
    }
    return FALSE;
}


int is_valid_label(char *name) {
    int i;
    if (!isalpha(name[0]))  /* The first character has to be a letter */
        return FALSE;
    if (strlen(name) > 31)  /* The maximum permissible length of the label is 31 */
        return FALSE;
    
    for (i = 0; name[i]; i++) {
        /* The other characters have to be letters or digits */
        if (!(isalpha(name[i]) || isdigit(name[i])))
            return FALSE;
    }
    /* The label cannot be an operation, or a register a definition */
    if (is_name_in(operations_names, NUM_OPS, name))
        return FALSE;
    if (is_name_in(definitions_names, NUM_DEFS, name))
        return FALSE;
    if (is_name_in(registers_names, NUM_REGISTERS, name))
        return FALSE;
    
    return TRUE; /* The label is good */
}


int is_name_in(char *arr[], int arr_size, char *name) {
    int i;
    for (i = 0; i < arr_size; i++) { /* Go through the names array */
        if (!strcmp(name, arr[i]))  /* the name is in the array */
            return TRUE;
    }
    return FALSE; /* the name is not un the array */
}

int operation_or_definition(char *name) {
    int i;
    
    /* check for operations: */
    for (i = 0; i < NUM_OPS; i++) {
        if (!strcmp(name, operations_names[i]))
            return i;
    }
    
    /* check for definitions: */
    for (i = 0; i < NUM_DEFS; i++) {
        if (!strcmp(name, definitions_names[i]))
            return i + DATA;
    }
    
    return print_error("Undefined operation or definition");
}

int is_number(char *s) {
    int i;
    if (s[0] == '-' || s[0] == '+')
        s++; /* we do not need the '+' or the '-' at the beginning of the number */
    for (i = 0; s[i]; i++) { /* Scans the number and checks that it is indeed all digits */
        if (!isdigit(s[i]))
            return FALSE;
    }
    return TRUE;
}

int check_commas(char *s, int *count_arguments) {
    int i;
    int in_word = 0, had_comma = 1; /* flags */
    
    s = skip_spaces(s);
    if (!(*s)) /* empty arguments part */
        return OK;
    if (s[0] == ',')
        return print_error("Invalid comma after operation or definition");
    
    /* Scanning the arguments part */
    for (i = 0; s[i]; i++) {
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
            in_word = 0;
            continue;
        }
        
        /* If there are subsequent cells that contain comma, there are multiple consecutive commas */
        if (s[i] == ',') {
            if (had_comma) /* multiple commas */
                return print_error("Multiple consecutive commas");
            in_word = 0;
            had_comma = 1;
        }
        else if (!in_word) { /* start a new word */
            if (!had_comma) /* missing comma between 2 word (only spaces between then */
                return print_error("Missing comma");
            in_word = 1;
            had_comma = 0;
            (*count_arguments)++;
        }
        /* were are in a word */
    }
    
    if (had_comma)
        return print_error("Invalid comma after last argument");
    
    return OK;
}

int is_valid_string(char *s) {
    int i;
    char *last = strrchr(s, '\"'); /* the last " in the string */
    
    if (s[0] != '\"') /* the first char must be " ) */
        return print_error("Missing \" before the string");
    
    for (i = 1; last[i]; i++) { /* check extraneous text after the string */
        if (!isspace(last[i]))
            return print_error("Missing \" after the string");
    }
    
    return OK;
}

char *skip_spaces(char *s) {
    int n;
    for (n = 0; s[n] && isspace(s[n]); n++) {} /* Count how many spaces there are at the beginning */
    return s + n;
}


void remove_spaces(char *str) {
    int i;
    /* To keep track of non-space character count */
    int count = 0;
    
    /* Traverse the given string. If current character
     is not space, then place it at index 'count++' */
    for (i = 0; str[i]; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            str[count++] = str[i]; /* here count is incremented */
    }
    
    str[count] = '\0'; /* finish the string */
}
