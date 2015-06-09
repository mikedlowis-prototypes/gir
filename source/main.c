#include <stdio.h>

/* Main
 *****************************************************************************/
int main(int argc, char** argv) {
    extern void world_init(void);
    extern void exec_file(FILE* file, const char* prompt);
    world_init();
    exec_file(stdin, ":> ");
    (void)argc;
    (void)argv;
    return 0;
}

