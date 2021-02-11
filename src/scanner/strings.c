#include "common.h"
#include "scanner.h"
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
