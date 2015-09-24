#include "atf.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    RUN_EXTERN_TEST_SUITE(LobbyTests);
    RUN_EXTERN_TEST_SUITE(NilTests);
    RUN_EXTERN_TEST_SUITE(BoolTests);
    RUN_EXTERN_TEST_SUITE(TrueTests);
    RUN_EXTERN_TEST_SUITE(FalseTests);
    RUN_EXTERN_TEST_SUITE(NumTests);
    RUN_EXTERN_TEST_SUITE(StringTests);
    RUN_EXTERN_TEST_SUITE(SymbolTests);
    RUN_EXTERN_TEST_SUITE(ListTests);
    RUN_EXTERN_TEST_SUITE(ArrayTests);
    RUN_EXTERN_TEST_SUITE(MapTests);
    RUN_EXTERN_TEST_SUITE(SetTests);
    RUN_EXTERN_TEST_SUITE(BlockTests);
    return PRINT_TEST_RESULTS();
}
