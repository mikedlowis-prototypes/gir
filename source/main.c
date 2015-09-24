#include "gir.h"

/* Main
 *****************************************************************************/
int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    GirCtx* ctx = gir_init();
    gir_evalfile(stdin, ":> ");
    gir_deinit(ctx);
    return 0;
}

