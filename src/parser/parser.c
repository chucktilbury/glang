#include "common.h"

#include "scanner.h"
#include "memory.h"
#include "char_buffer.h"
#include "parser.h"
#include "symbols.h"

#include "class_definition.h"
#include "method_definition.h"

// Check the configuration to find the file to be imported.
static const char* find_import(const char* fname) {
    // place holder for after getting the configuration module built.
    return fname;
}

// Open a file for import. Only read the class definitions to acquire
// the symbols and type information.
static parse_state_t import_statement() {

    token_t tok = expect_tok(QSTRG_TOKEN);
    if(tok == QSTRG_TOKEN) {
        open_scanner_file(find_import(get_tok_str()));
        return PARSE_TOP;
    }

    return PARSE_ERROR;
}

// Eat the rest of this block without detecting errors in an attempt to
// get resynchronized.
static void eat_block() {

    // might want to adjust this for better error handling or detection.
    int finished = 0;

    while(!finished) {
        int tok = get_tok();
        if(tok == OCUR_TOKEN || tok == CCUR_TOKEN)
            finished++;
    }
}

static void uninit_parser() {}

/*
    Create the data structures and open the initial file.
*/
void init_parser(const char* fname) {

    // Create the symbol table and ant other data structures.
    init_symbol_table();

    if(fname != NULL) {
        open_scanner_file(fname);
    }
    else
        command_error("no input file was specified");

    atexit(uninit_parser);
}

/*
    This is the main entry point to the parser. It expects that a file will be
    open before it's called. All of the emit and other functions are called
    from here.

    At the top level, only class definitions, method definitions, and import
    statements are
*/
int parse() {

    int finished = 0;
    token_t tok;
    parse_state_t state = PARSE_TOP;

    while(!finished) {
        tok = get_tok();
        switch(state) {
            case PARSE_TOP:
                switch(tok) {
                    case CLASS_TOKEN:
                        state = class_definition();
                        break;
                    case SYMBOL_TOKEN:
                        state = method_definition();
                        break;
                    case IMPORT_TOKEN:
                        state = import_statement();
                        break;
                    case ERROR_TOKEN:
                        state = PARSE_ERROR;
                        break;
                    case END_OF_INPUT:
                        state = PARSE_ENDING;
                        break;
                    default:
                        syntax("expected 'class', 'symbol', or 'import' but got a %s", token_to_str(tok));
                        break;
                }
                break;
            case PARSE_ERROR:
                eat_block();
                state = PARSE_TOP;
                break;
            case PARSE_ENDING:
                finished ++;
                break;
        }
    }
    return get_num_errors();
}