#include "common.h"
#include "scanner.h"
#include "parser.h"

// place holder for configuration data structure
static struct {
    const char* initial_file;
} configuration;

static void init_configuration(int argc, char** argv) {
    // place holder for when the configuration module is built
    (void)argc;
    configuration.initial_file = argv[1];
}

static void init_things(int argc, char** argv) {

    init_configuration(argc, argv);
    init_errors(stderr);
    init_memory();
    init_scanner();
    init_parser(configuration.initial_file);
}

int main(int argc, char** argv) {

    init_things(argc, argv);
    return parse();
}