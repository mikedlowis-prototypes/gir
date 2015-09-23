#include "gir.h"

/* Main
 *****************************************************************************/
int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    gir_init((void*)&(int){0});
    gir_evalfile(stdin, ":> ");
    gir_deinit();
    return 0;
}

