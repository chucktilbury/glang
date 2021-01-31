#include "common.h"
#include "scanner.h"
#include "char_buffer.h"
#include "local.h"

static void get_hex_escape() {

    char tbuf[9];   // hex escape has a maximum of 8 characters
    int idx, ch;

    memset(tbuf, 0, sizeof(tbuf));
    for(idx = 0; idx < (int)sizeof(tbuf); idx++) {
        ch = get_char();
        if(isxdigit(ch))
            tbuf[idx] = ch;
        else
            break;
    }

    if(strlen(tbuf) == 0) {
        warning("invalid hex escape code in string: '%c' is not a hex digit. Ignored.", ch);
        unget_char(ch);
    }
    else {
        int val = (int)strtol(tbuf, NULL, 16);
        add_char_buffer_int(scanner_buffer, val);
    }
}

static void get_octal_escape() {

    char tbuf[4];   // hex escape has a maximum of 3 characters
    int idx, ch;

    memset(tbuf, 0, sizeof(tbuf));
    for(idx = 0; idx < (int)sizeof(tbuf); idx++) {
        ch = get_char();
        if(ch >= '0' && ch <= '7')
            tbuf[idx] = ch;
        else
            break;
    }

    if(strlen(tbuf) == 0) {
        warning("invalid octal escape code in string: '%c' is not a octal digit. Ignored.", ch);
        unget_char(ch);
    }
    else {
        int val = (int)strtol(tbuf, NULL, 8);
        add_char_buffer(scanner_buffer, val);
    }
}

static void get_decimal_escape() {

    char tbuf[11]; // maximum of 10 characters in decimal escape
    int idx = 0, ch;

    memset(tbuf, 0, sizeof(tbuf));
    ch = get_char();
    // decimal escape may be signed
    if(ch == '+' || ch == '-') {
        tbuf[idx] = ch;
        idx ++;
    }

    for(; idx < (int)sizeof(tbuf); idx ++) {
        ch = get_char();
        if(isdigit(ch))
            tbuf[idx] = ch;
        else
            break;
    }

    if(strlen(tbuf) == 0) {
        warning("invalid decimal escape code in string: '%c' is not a decimal digit. Ignored.", ch);
        unget_char(ch);
    }
    else {
        int val = (int)strtol(tbuf, NULL, 10);
        add_char_buffer_int(scanner_buffer, val);
    }
}

// When this is entered, a back-slash has been seen in a dquote string.
// could have an octal, hex, or character escape.
static void get_string_esc() {

    int ch = get_char();
    switch(ch) {
        case 'x':
        case 'X': get_hex_escape(); break;
        case 'd':
        case 'D': get_decimal_escape(); break;
        case '0': get_octal_escape(); break;
        case 'n': add_char_buffer(scanner_buffer, '\n'); break;
        case 'r': add_char_buffer(scanner_buffer, '\r'); break;
        case 't': add_char_buffer(scanner_buffer, '\t'); break;
        case 'b': add_char_buffer(scanner_buffer, '\b'); break;
        case 'f': add_char_buffer(scanner_buffer, '\f'); break;
        case 'v': add_char_buffer(scanner_buffer, '\v'); break;
        case '\\': add_char_buffer(scanner_buffer, '\\'); break;
        case '\"': add_char_buffer(scanner_buffer, '\"'); break;
        case '\'': add_char_buffer(scanner_buffer, '\''); break;
        default: add_char_buffer(scanner_buffer, ch); break;
    }
}



// Look ahead up to either a ',' or a \n. If a ',' is discoverd first, then
// the string is continued. if a '\n' is discovered first, or any other
// character, then the string is ending. If the first character is a '/'
// then it is either a comment or a syntax error. A comment may appear after
// the comma, but before the continued string as well. Returns 0 if the string
// is continuing and 1 if the string is ending.
static int get_string_end(int ch) {

    int retv, c, finished = 0;
    //char_buffer_t cb = create_char_buffer();

    while(!finished) {
        c = get_char();
        if(!isspace(c)) {
            if(c == '\n') {
                unget_char(c);
                finished++;
                retv = 1;   // must have the ',' before the '\n'
            }
            else if(c == ',') {
                // definately continuing the line
                skip_ws();
                c = get_char();
                if(c == ch) {
                    // beginning of continuation
                    retv = 0;
                    finished++;
                }
                else {
                    // syntax error
                    syntax("invalid string continuation. Expected to start another string segment.");
                    finished++;
                    retv = 1;
                }
            }
            else if(c == '/') {
                // comment or operator
                c = get_char();
                if(!isspace(c)) {
                    if(c == '*')
                        eat_multi_line();
                    else if(c == '/')
                        eat_single_line();
                    else {
                        // unknown and unexpected char, let the parser handle it
                        unget_char(c);
                        finished++;
                        retv = 1;
                    }
                    // continue after the comment
                }
                else {
                    // it's an operator, let the parser deal with it
                    unget_char(c);
                    retv = 1;
                    finished++;
                }
            }
            else {
                // end of the string
                retv = 1;
                finished++;
            }
        }
        // else it is a space, so continue

        // c = get_char();
        // if(isspace(c) || c == '\n')
        //     add_char_buffer(cb, c);
        // else if(c == ch) {
        //     destroy_char_buffer(cb);
        //     return 1; // throw away the ch
        // }
        // else {
        //     unget_char(c);
        //     const char* buf = get_char_buffer(cb);
        //     // printf(">>>>>>>>>> ");
        //     // for(int x = 0; buf[x] != 0; x++)
        //     //     printf("%02X, ", buf[x]);
        //     // printf("\n");
        //     size_t len = strlen(buf);
        //     for(int i = len-1; i >= 0; i--)
        //         unget_char(buf[i]);
        //     destroy_char_buffer(cb);
        //     return 0;
        // }
    }
    return retv;
}

// When this is entered, a dquote has been seen. Check to see if the string
// is continued or if it has ended.
// static int get_string_end(int ch) {

//     // fix a bug. If a CR is read, then the line numbers are off.
//     if(check_char(ch))
//         return 0;   // keep copying string
//     return 1;       // string has ended
// }

// when this is entered, a (") has been seen and discarded. This function
// performs escape replacements, but it does not do formatting. that takes
// place in the parser. If consecutive strings are encountered, separated
// only by white space, then it is a string continuance and is a multi line
// string.
token_t read_dquote() {

    int finished = 0;
    int ch;
    token_t tok = QSTRG_TOKEN;

    while(!finished) {
        ch = get_char();
        switch(ch) {
            case '\\':
                get_string_esc();
                break;
            case '\"':
                if(get_string_end('\"'))
                    finished++;
                break;
            case '\n':
                syntax("line breaks are not allowed in a string.");
                eat_until_ws();
                return ERROR_TOKEN;
                break;
            default:
                add_char_buffer(scanner_buffer, ch);
                break;
        }
    }

    return tok;
}

// when this is entered, a (') has been seen and discarded. This function
// copies whatever is found into a string, exactly as it is found in the
// source code. If consecutive strings are found separated only by white
// space, then it is a single string that is being scanned.
token_t read_squote() {

    int finished = 0;
    int ch;
    token_t tok = QSTRG_TOKEN;

    while(!finished) {
        ch = get_char();
        switch(ch) {
            case '\'':
                if(get_string_end('\''))
                    finished++;
                break;
            case '\n':
                syntax("line breaks are not allowed in a string.");
                eat_until_ws();
                return ERROR_TOKEN;
                break;
            default:
                add_char_buffer(scanner_buffer, ch);
                break;
        }
    }

    return tok;
}

// when this is entered, a (/) has been seen. If the next character is a
// (/) or a (*), then we have a comment, otherwise we have a / operator.
token_t comment_or_operator() {

    int ch = get_char();

    if(ch == '/') {
        // eat a sinlge line comment
        eat_single_line();
        return NONE_TOKEN;
        // eat a sinlge line comment
        // int finished = 0;
        // while(!finished) {
        //     ch = get_char();
        //     if(ch == '\n' || ch == END_FILE)
        //         return NONE_TOKEN;
        // }
    }
    else if(ch == '*') {
        // eat a multi line comment
        eat_multi_line();
        return NONE_TOKEN;
        // int state = 0;
        // while(ch != END_OF_INPUT) {
        //     ch = get_char();
        //     switch(state) {
        //         case 0:
        //             if(ch == '*')
        //                 state = 1;
        //             //fprintf(stderr, "0");
        //             break;
        //         case 1:
        //             if(ch == '/')
        //                 state = 2;
        //             else if(ch != '*')
        //                 state = 0;
        //             //fprintf(stderr, "1");
        //             break;
        //         case 2:
        //             unget_char(ch);
        //             return NONE_TOKEN;

        //     }
        // }
    }
    else {
        // not a comment, must be a / single-character operator
        unget_char(ch);
        return SLASH_TOKEN;
    }

    // probably an error for the parser to handle
    return END_OF_INPUT;
}
