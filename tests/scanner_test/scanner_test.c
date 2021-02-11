
#include "common.h"
#include "scanner.h"


BEGIN_CONFIG
    CONFIG_NUM("-v", "VERBOSE", "Set the verbosity from 0 to 50", 0, 0, 0)
    CONFIG_LIST("-i", "FPATH", "Specify directories to search for imports", 0, ".:include", 0)
END_CONFIG


static void print_line(token_t t) {
    printf("%s: %d: %d: token: %d %s '%s'\n",
            get_file_name(), get_line_no(), get_column_no(),
            t, token_to_str(t), get_tok_str()) ;
}

int main(int argc, char** argv) {

    token_t tok;
    init_memory();
    init_scanner();
    configure(argc, argv);

    for(char* str = iterate_config("INFILES"); str != NULL; str = iterate_config("INFILES")) {
        open_input_file(str);
        while(END_OF_INPUT != (tok = get_tok())) {
            print_line(tok);
            if(tok == IMPORT_TOKEN) {
                tok = get_tok();
                print_line(tok);
                open_input_file(get_tok_str());
            }
        }
        print_line(tok);
    }

}
