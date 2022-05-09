
/* Assembler. A program that accepts names of source files in assembly language as
 * command line arguments and creates ob, ent and ext files for them.
 * The program checks for errors in the source files it receives and prints error messages accordingly,
 * if errors are found in a particular file, the ob, ent and ext files will not be generated for it.
 *
 * authors: Elad Shoshani , Yuval Dafna */


void open_files(int argc, char *argv[]);

/* The main method - operates the assembler */
int main(int argc, char *argv[]) {
    open_files(argc, argv);
    return 0;
}