#include "common.h"

#include "scanner.h"
#include "parser.h"
#include "char_buffer.h"
#include "symbols.h"

/*
    Top level interface. When this is entered, the 'class' keyword has been
    read and the class name, followed by the class definition, is expected.
*/
parse_state_t class_definition() {

    token_t tok = expect_tok(SYMBOL_TOKEN);
    if(tok == ERROR_TOKEN) {

    }

    return PARSE_ERROR;
}