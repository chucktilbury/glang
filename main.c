#include "common.h"
#include "scanner.h"
#include "parser.h"

static void init_things() {

    init_errors(stderr);
    init_memory();
    init_scanner();
    init_parser();
}

int main(int argc, char** argv) {

    init_things();
    return 0;
}