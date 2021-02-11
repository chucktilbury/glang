#include "common.h"

#include "scanner.h"
#include "parser.h"
#include "symbols.h"

/**
 * When this is entered, we are parsing a function declaration and the name has
 * been read. The opening required '(' has been read and discarded. This func
 * reads a list of the parameter types, the closing ')' and a ';'.
 *
 */
static finish_func_decl(symbol_t* sym) {

}

/**
 * When this is entered, the class name has been stored and the deco_buffer has
 * the name in it. The opening '{' has been read and discarded.
 *
 * Only a data definition or a method declaration are accepted. They may have
 * an optional scope operator of public, private, or protected. Private is the
 * default.
 *
 * Example of data definition:
 *  public int var_name // symbol: $class_name@var_name
 *  int var_name
 *
 * Example of a method declaration:
 *  protected string meth_name(int, dict, string)
 *  int meth_name(int)  // symbol: $class_name$meth_name@int
 *  float meth_name()
 *
 */
static parse_state_t parse_class_body() {

    symbol_t sym;
    token_t tok = get_tok();
    int finished = 0;

    // optional scope spec; default is private
    sym.scope = SCOT_PRIVATE;
    switch(tok) {
        case PUBLIC_TOKEN:
            sym.scope = SCOT_PUBLIC;
            tok = get_tok();
            break;
        case PRIVATE_TOKEN:
            sym.scope = SCOT_PRIVATE;
            tok = get_tok();
            break;
        case PROTECTED_TOKEN:
            sym.scope = SCOT_PROTECTED;
            tok = get_tok();
            break;
    }

    // get the required assign type
    switch(tok) {
        case INT_TOKEN:
            sym.assign_type = ASST_INT;
            break;
        case UINT_TOKEN:
            sym.assign_type = ASST_UINT;
            break;
        case FLOAT_TOKEN:
            sym.assign_type = ASST_FLOAT;
            break;
        case BOOL_TOKEN:
            sym.assign_type = ASST_BOOL;
            break;
        case DICT_TOKEN:
            sym.assign_type = ASST_DICT;
            break;
        case MAP_TOKEN:
            sym.assign_type = ASST_MAP;
            break;
        case LIST_TOKEN:
            sym.assign_type = ASST_LIST;
            break;
        case SYMBOL_TOKEN:
            sym.assign_type = ASST_CLASS;
            break;
        default:
            syntax("expected type specifier but got %s", token_to_str(tok));
            return PARSE_ERROR;
    }

    // next token must be a SYMBOL_TOKEN
    tok = expect_tok(SYMBOL_TOKEN);
    if(tok != ERROR_TOKEN)
        deco_add_name(get_tok_str());
    else
        return PARSE_ERROR;

    // Note that data assignment is not allowed in a class decalartion.
    // If the next token is a ';' then it's a data declaration and we are done.
    // If the next token is a '(' then it a func declaration and we have to get
    // the types.
    tok = get_tok();
    switch(tok) {
        case SEMIC_TOKEN:
            return PARSE_TOP;
            break;
        case OPAR_TOKEN:
            return finish_func_decl(&sym);
            break;
        default:
            syntax("expected a ';' or a '(' but got a %s", token_to_str(tok));
            return PARSE_ERROR;
    }

    return PARSE_ERROR; // should never happen
}

/**
 * Fill out the symbol type data and save the class name.
 */
static parse_state_t close_class_name() {

    symbol_t symb;

    symb.name_type = NAMET_CLASS;
    symb.assign_type = ASST_CLASS;
    symb.scope = SCOT_PUBLIC;
    if(add_symbol(&symb) != SYM_NO_ERROR) {
        syntax("name '%s' has already been defined", undecorate_name());
        return PARSE_ERROR;
    }
    return PARSE_TOP;
}

/**
 * Top level interface. When this is entered, the 'class' keyword has been read
 * and the class name, followed by the class definition, is expected.
 *
 * This accepts a string like:
 *  name(name) { // $name$name
 *  name() {    // $name
 *  name {      // $name
 *
 */
parse_state_t class_definition() {

    token_t tok = expect_tok(SYMBOL_TOKEN);
    if(tok == ERROR_TOKEN)
        return PARSE_ERROR;

    // add the name
    deco_add_name(get_tok_str());

    // the next token has to be a '(' or a '{'
    tok = get_tok();
    if(tok == OPAR_TOKEN) {
        tok = get_tok();
        if(tok == SYMBOL_TOKEN) {
            deco_add_name(get_tok_str());
            tok = get_tok();
            if(tok != CPAR_TOKEN) {
                syntax("expected a ')' but got a %s", token_to_str(tok));
                return PARSE_ERROR;
            }

            // go to the class body
            tok = get_tok();
            if(tok == OCUR_TOKEN) {
                if(close_class_name() == PARSE_ERROR) {
                    return PARSE_ERROR;
                }
                return parse_class_body();
            }
            else {
                syntax("expected a '{' but got a %s", token_to_str(tok));
                return PARSE_ERROR;
            }
        }
        else if(tok == CPAR_TOKEN) {
            tok = get_tok();
            if(tok == OCUR_TOKEN) {
                if(close_class_name() == PARSE_ERROR) {
                    return PARSE_ERROR;
                }
                return parse_class_body();
            }
            else {
                syntax("expected a '{' but got a %s", token_to_str(tok));
                return PARSE_ERROR;
            }
        }
        else {
            syntax("expected a '(' or a '{' but got a %s", token_to_str(tok));
            return PARSE_ERROR;
        }
    }
    else if(tok == OCUR_TOKEN) {
        if(close_class_name() == PARSE_ERROR) {
            return PARSE_ERROR;
        }
        return parse_class_body();
    }
    else {
        syntax("expected base class name or the class body, but got %s", token_to_str(tok));
        return PARSE_ERROR;
    }
}
