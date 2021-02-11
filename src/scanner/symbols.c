#include "common.h"
#include "scanner.h"
#include "local.h"

// Read a word from the input and then find out if it's a keyword or a symbol.
token_t read_word() {

    int c;
    int finished = 0;

    while(!finished) {
        c = get_char();
        if(isalnum(c) || c == '_')
            add_char_buffer(scanner_buffer, c);
        else {
            finished++;
        }
    }
    unget_char(c);
    const char* find = get_char_buffer(scanner_buffer);

    return str_to_token(find);
}

// When this is entered, a punctuation character has been read. If it's a (_)
// then we have a symbol, otherwise, it's an operator.
token_t read_punct(int ch) {

    if(ch == '_') {
        unget_char('_');
        // wasteful binary search for something that is certinly a symbol
        return read_word();
    }
    else {
        switch(ch) {
            // single character operators
            case '*': return MUL_TOKEN; break;
            case '%': return MOD_TOKEN; break;
            case ',': return COMMA_TOKEN; break;
            case ';': return SEMIC_TOKEN; break;
            case ':': return COLON_TOKEN; break;
            case '[': return OSQU_TOKEN; break;
            case ']': return CSQU_TOKEN; break;
            case '{': return OCUR_TOKEN; break;
            case '}': return CCUR_TOKEN; break;
            case '(': return OPAR_TOKEN; break;
            case ')': return CPAR_TOKEN; break;
            case '.': return DOT_TOKEN; break;
            case '|': return OR_TOKEN; break; // comparison
            case '&': return AND_TOKEN; break; // comparison

            // could be single or double
            case '=': {
                    int c = get_char();
                    if(c == '=')
                        return EQUALITY_TOKEN;
                    else {
                        unget_char(c);
                        return EQU_TOKEN;
                    }
                }
                break;
            case '<': {
                    int c = get_char();
                    if(c == '=')
                        return LTE_TOKEN;
                    else if(c == '>')
                        return NEQ_TOKEN;
                    else {
                        unget_char(c);
                        return LT_TOKEN;
                    }
                }
                break;
            case '>': {
                    int c = get_char();
                    if(c == '=')
                        return GTE_TOKEN;
                    else {
                        unget_char(c);
                        return GT_TOKEN;
                    }
                }
                break;
            case '-': {
                    int c = get_char();
                    if(c == '-')
                        return DEC_TOKEN;
                    else {
                        unget_char(c);
                        return SUB_TOKEN;
                    }
                }
                break;
            case '+': {
                    int c = get_char();
                    if(c == '+')
                        return INC_TOKEN;
                    else {
                        unget_char(c);
                        return ADD_TOKEN;
                    }
                }
                break;
            case '!': {
                    int c = get_char();
                    if(c == '=')
                        return NEQ_TOKEN;
                    else {
                        unget_char(c);
                        return NOT_TOKEN;
                    }
                }
                break;

            // these are not recognized
            default:
                warning("unrecognized character in input: '%c' (0x%02X). Ignored.", ch, ch);
                return NONE_TOKEN;
                break;
        }
    }
    return ERROR_TOKEN; // should never happen
}
