#include <stdio.h>
#include <stdlib.h>

#include "../../scanner.h"
#include "../../memory.h"
#include "../../errors.h"

static void print_line(token_t t) {
    printf("%s: %d: %d: token: %d %s '%s'\n",
            get_file_name(), get_line_no(), get_column_no(),
            t, token_to_str(t), get_tok_str()) ;
}

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "test file name required\n");
        exit(1);
    }

    token_t tok;
    init_memory();
    init_scanner();

    open_scanner_file(argv[1]);
    while(END_OF_INPUT != (tok = get_tok())) {
        print_line(tok);
        if(tok == IMPORT_TOKEN) {
            tok = get_tok();
            print_line(tok);
            open_scanner_file(get_tok_str());
        }
    }
}