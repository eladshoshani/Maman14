
#include "assembler.h"

/* the heads of the symbol table: */
symbol_table data_table_head;
symbol_table code_table_head;
symbol_table entry_table_head;
symbol_table extern_table_head;
symbol_table refer_to_an_external_symbol_head; /* in the second pass, line 6 in the algorithm (with this list we write the .ext file) */

void initialize_symbol_table_heads() {
    /* initializing all the heads to NULL */
    data_table_head = NULL;
    code_table_head = NULL;
    entry_table_head = NULL;
    extern_table_head = NULL;
    refer_to_an_external_symbol_head = NULL;
}

void allocate_memory_instruction_table(int index) {
    instruction_table[index] = (word *) calloc(1, sizeof(word)); /* allocate the memory */
    if (!instruction_table[index]) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
}

void allocate_memory_data_table(int index) {
    data_table[index] = (word *) calloc(1, sizeof(word)); /* allocate the memory */
    if (!data_table[index]) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
}

void allocate_memory_symbol_table(symbol_table *new_node, char *label_name) {
    (*new_node) = (symbol_table) malloc(sizeof(node)); /* allocate the memory */
    if (!(*new_node)) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
    
    (*new_node)->label_name = (char *) malloc(strlen(label_name) + 1); /* allocate the memory for the label name */
    if (!((*new_node)->label_name)) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
}

int add_to_table(symbol_table *head, char *label_name, int label_value) {
    symbol_table new_node, p;
    p = *head;
    
    allocate_memory_symbol_table(&new_node, label_name);
    
    if (check_for_in_the_list(head, label_name) == ERROR) /* the label is already on the list */
        return ERROR;
    
    if (head != &refer_to_an_external_symbol_head && is_on_list(*head, label_name))
        return OK;  /* More than one instance of the .extern or .entry with the same label - we don't need to do nothing */
    
    strcpy(new_node->label_name, label_name); /* Copying the symbols attributes */
    new_node->value = label_value;
    new_node->next = NULL;
    
    /* moving head to the last node in the list: */
    if (!(*head)) { /* the list is empty */
        (*head) = new_node;
        return OK;
    }
    else {  /* the head is not NULL */
        while (p->next) { /* moving to the last node */
            p = p->next;
        }
    }
    
    p->next = new_node; /* adding the node to the end of the list */
    
    return OK;
}


int is_on_list(symbol_table head, char *label_name) {
    while (head) { /* moving head to the last node in the list */
        if (!strcmp(head->label_name, label_name)) /* the name is on the list */
            return TRUE;
        head = head->next;
    }
    return FALSE;
}


int check_for_in_the_list(symbol_table *head, char *label_name) {
    /* check if the label is in the symbol table */
    int on_the_tables_already = is_on_list(data_table_head, label_name) || is_on_list(code_table_head, label_name);
    
    if (head == &data_table_head || head == &code_table_head) {
        /* the label is already on the list: */
        if (on_the_tables_already) {
            return print_error("Multiple definitions of the same label");
        }
    }
    else if (head == &entry_table_head) {
        /* the label is not already on the tables: */
        if (!on_the_tables_already)
            return print_error("The symbol is not in the symbol table (in .entry definition)");
        
        if (is_on_list(extern_table_head, label_name)) /* It is forbidden to declare a label in both entry and extern */
            return print_error("Label definition in both extern and entry");
    }
    else if (head == &extern_table_head && is_on_list(entry_table_head, label_name)) {
        /* It is forbidden to declare a label in both entry and extern */
        return print_error("Label definition in both extern and entry");
    }
    return OK;
}


void increase_address_to_data() {
    symbol_table p = data_table_head;
    while (p) { /* Scanning the symbol_data_table */
        p->value += ICF; /* increase the value */
        p = p->next;
    }
}

void insert_number_to_data_table(int num) {
    allocate_memory_data_table(DC); /* allocating memory for the word */
    data_table[DC]->word3.val = num; /* inserting the number to the data table */
    DC++;
}

void insert_numbers_to_data_table(char *numbers) {
    char *temp; /* just for using strtol */
    char *token;
    token = strtok(numbers, ",");
    
    while (token != NULL) { /* Go through all the numbers */
        if (token[0] == '+')
            token++;
        insert_number_to_data_table(strtol(token, &temp, 10)); /* insert each number to the table */
        token = strtok(NULL, ","); /* passing to the nest number */
    }
}

void insert_string_to_data_table(char *s) {
    int i;
    for (i = 1; s[i + 1]; i++) { /* scan the string without the first and last char (they are " ) */
        insert_number_to_data_table(s[i]); /* insert all the chars to the data table */
    }
    insert_number_to_data_table('\0'); /* the last char in a string is '\0' and we need to enter him as well */
}
