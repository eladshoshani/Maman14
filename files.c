
#include "assembler.h"

char *file_name; /* the name of the current file */

void open_files(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) { /* Scanning argv from the second argument which is the first file name to be checked */
        open_file(argv[i]);
    }
}

void open_file(char *name) {
    FILE *source_file;
    initialize_symbol_table_heads();
    /* Assigns memory for the name of the file and copies the name of the file, including the ".as" ending, to it */
    file_name = (char *) malloc(strlen(name) + 3); /* Allocate memory for the full file name (strlen(".as") = 3) */
    
    if (!file_name) { /* Allocates memory failed so we print an error message and exit the program */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
    
    strcpy(file_name, name);
    strcat(file_name, ".as"); /* file_name stores the file name as it is on the computer */
    
    /* Opens the file, if it exists, for reading: */
    source_file = fopen(file_name, "r");
    if (!source_file) { /* If the file wasn't found, or it isn't allowed for reading, the file pointer is NULL */
        fprintf(stderr, "Couldn't open file %s.\n", file_name);
        return;
    }
    
    /* Starts the first and second passes, and if they are finished successfully, writes the output files */
    if (!first_pass(source_file) && !second_pass(source_file))
        write_output_files(name);
    
    fclose(source_file); /* Closes the file after reading and frees the file_name string for the next file name */
    free(file_name);
}


FILE *get_file(char *name, char *file_ending) {
    FILE *file_pointer;
    char *out_name = (char *) malloc(strlen(name) + strlen(file_ending));
    if (!out_name) { /* Allocates memory failed so we print an error message and exit the program */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
    strcpy(out_name, name);
    strcat(out_name, file_ending);
    
    file_pointer = fopen(out_name, "w"); /* Opens the file, with writing permission */
    free(out_name); /* The file's name isn't needed anymore */
    return file_pointer;
}

void write_output_files(char *name) {
    /* Calling to the methods that write each output file, with the name of the given file: */
    write_ob_file(name);
    write_ent_file(name);
    write_ext_file(name);
}


void write_ob_file(char *name) {
    int i;
    FILE *ob_file = get_file(name, ".ob"); /* Gets the file with ".ob" ending and writing permission */
    fprintf(ob_file, "%d %d\n", ICF - START_OF_CODE_ADDRESS,
            DCF); /* Prints the number of instruction and data words */
    for (i = 0; i < ICF - START_OF_CODE_ADDRESS; i++) /* First, prints each instruction word */
        fprintf(ob_file, "%07d %06x\n", i + START_OF_CODE_ADDRESS, instruction_table[i]->word3.val);
    for (i = 0; i < DCF; i++) /* Prints each data word */
        fprintf(ob_file, "%07d %06x\n", i + ICF, data_table[i]->word3.val);
    fclose(ob_file); /* Frees the file pointer after use */
}


void write_ent_file(char *name) {
    FILE *file_pointer;
    symbol_table p = entry_table_head;
    
    if (!entry_table_head)
        return;
    
    file_pointer = get_file(name, ".ent"); /* Gets the file with ".ent" ending and writing permission */
    while (p) { /* Scanning the symbol_table */
        fprintf(file_pointer, "%s %07d\n", p->label_name, p->value);
        p = p->next;
    }
    fclose(file_pointer); /* Frees the file pointer after use */
}


void write_ext_file(char *name) {
    FILE *file_pointer;
    symbol_table p = refer_to_an_external_symbol_head;
    
    if (!refer_to_an_external_symbol_head)
        return;
    
    file_pointer = get_file(name, ".ext"); /* Gets the file with ".ext" ending and writing permission */
    while (p) { /* Scanning the symbol_table */
        fprintf(file_pointer, "%s %07d\n", p->label_name, p->value);
        p = p->next;
    }
    fclose(file_pointer); /* Frees the file pointer after use */
}

