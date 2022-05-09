

/* NOT PART OF THE MAMAN */

#include "assembler.h"


void printBin(word *x) {
    unsigned int mask = 1 << (24 - 1);
    while (mask) {
        if ((x->word3.val & mask) == 0)
            printf("0");
        else
            printf("1");
        mask >>= 1;
    }
    printf("\n");
}

void print_symbol_list(symbol_table head) {
    if (head == NULL) {
        printf("no values\n");
        return;
    }
    while (head) {
        printf("name: %s , address: %d\n", head->label_name, head->value);
        head = head->next;
    }
}

void print_data_table() {
    int i;
    for (i = 0; i < DCF; i++) {
        if (!data_table[i])
            printf("?\n");
        else
            printBin(data_table[i]);
    }
}

void print_instruction_table() {
    int i;
    for (i = 0; i < ICF - 100; i++) {
        if (!instruction_table[i])
            printf("?\n");
        else
            printBin(instruction_table[i]);
    }
}


//int main(int argc, char *argv[]) {
//    printf("!!!!\n");
//
//    FILE *source_file = stdin;
//    int error_flag = FALSE;
//    char line[LINE_LEN];
//    IC = 100, DC = 0; /* Initialize the variables */
//
//    printf("please enter:\n");
//    for (line_number = 1; fgets(line, LINE_LEN,
//                                source_file) &&
//                          line[0] != '@'; line_number++) {/* Scanning through each line of the file */
//
//        if (handle_line_first_pass(line) == ERROR) /* parse every line */
//            error_flag = TRUE;
//
//        printf("line = %d , IC = %d , DC = %d , please enter:\n", line_number, IC, DC);
//    }
//
//    ICF = IC;
//    DCF = DC;
//
//    increase_address_to_data(); /* line 19 */
//
//    printf("error? %d\n", error_flag);
//
//    printf("data:\n");
//    print_symbol_list(data_table_head);
//    printf("code:\n");
//    print_symbol_list(code_table_head);
//    printf("entry:\n");
//    print_symbol_list(entry_table_head);
//    printf("extern:\n");
//    print_symbol_list(extern_table_head);
//    printf("refer_to_extern:\n");
//    print_symbol_list(refer_to_an_external_symbol_head);
//
//    printf("instruction_table:\n");
//    print_instruction_table();
//
//    printf("data_table:\n");
//    print_data_table();
//
//    printf("start second pass:\n");
//
//    IC = 100, DC = 0; /* Initialize the variables */
//    for (line_number = 1; fgets(line, LINE_LEN,
//                                source_file) &&
//                          line[0] != '@'; line_number++) { /* Scanning through each line of the file */
//        if (second_pass_handle_line(line) == ERROR)
//            error_flag = TRUE; /* There was an error */
//
//        printf("line = %d , IC = %d , DC = %d , please enter:\n", line_number, IC, DC);
//    }
//
//    printf("error? %d\n", error_flag);
//
//    printf("data:\n");
//    print_symbol_list(data_table_head);
//    printf("code:\n");
//    print_symbol_list(code_table_head);
//    printf("entry:\n");
//    print_symbol_list(entry_table_head);
//    printf("extern:\n");
//    print_symbol_list(extern_table_head);
//    printf("refer_to_extern:\n");
//    print_symbol_list(refer_to_an_external_symbol_head);
//
//    printf("instruction_table:\n");
//    print_instruction_table();
//
//    printf("data_table:\n");
//    print_data_table();
//
//
//    return 0;
//}


