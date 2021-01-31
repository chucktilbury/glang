#include "common.h"
#include "scanner.h"
#include "parser.h"

// note that longer vars with the same leading letters need to appear before shorter ones.
BEGIN_CONFIG
    CONFIG_NUM("-v", "VERBOSE", "Set the verbosity from 0 to 50", 0, 0, 0)
    CONFIG_STR("-o", "OUTFILE", "Specify the file name to output", 0, "output.bc", 1)
    CONFIG_LIST("-i", "FPATH", "Specify directories to search for imports", 0, ".:include", 0)
    CONFIG_BOOL("-D", "DFILE_ONLY", "Output the dot file only. No object output", 0, 0, 0)
    CONFIG_STR("-d", "DUMP_FILE", "Specify the file name to dump the AST into", 0, "ast_dump.dot", 1)
END_CONFIG

static void init_things(int argc, char** argv) {

    init_memory();
    configure(argc, argv);
    init_errors(stderr);
    init_scanner();
    init_parser();
}

int main(int argc, char** argv) {

    int retv;

    init_things(argc, argv);
    for(char* str = iterate_config("INFILES"); str != NULL; str = iterate_config("INFILES")) {
        retv = parse(str);
        if(retv != 0)
            break;
    }

    return retv;
}
