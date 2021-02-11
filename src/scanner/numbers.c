#include "common.h"
#include "scanner.h"
#include "local.h"


static token_t read_hex_number() {

    int ch;
    while(isxdigit(ch = get_char()))
        add_char_buffer(scanner_buffer, ch);

    return UNUM_TOKEN;
}

static token_t read_octal_number() {

    int ch;
    while(isdigit(ch = get_char())) {
        if(ch <= '7')
            add_char_buffer(scanner_buffer, ch);
        else {
            // eat the rest of the number and publish an error
            while(isdigit(ch = get_char()))
                add_char_buffer(scanner_buffer, ch);
            unget_char(ch);
            syntax("malformed octal number: %s", get_char_buffer(scanner_buffer));
            return ERROR_TOKEN;
        }
    }
    return ONUM_TOKEN;
}

// we can only enter here <after> we have seen a '.'
static token_t read_float_number() {

    int ch;
    while(isdigit(ch = get_char())) {
        add_char_buffer(scanner_buffer, ch);
    }

    // see if we are reading a mantisa
    if(ch == 'e' || ch == 'E') {
        add_char_buffer(scanner_buffer, ch);
        ch = get_char();
        if(ch == '+' || ch == '-') {
            add_char_buffer(scanner_buffer, ch);
            ch = get_char();
            if(isdigit(ch)) {
                add_char_buffer(scanner_buffer, ch);
            }
            else {
                unget_char(ch);
                syntax("malformed float number: %s", get_char_buffer(scanner_buffer));
                return ERROR_TOKEN;
            }
            while(isdigit(ch = get_char())) {
                add_char_buffer(scanner_buffer, ch);
            }
            unget_char(ch);
        }
        else if(isdigit(ch)) {
            add_char_buffer(scanner_buffer, ch);
            while(isdigit(ch = get_char())) {
                add_char_buffer(scanner_buffer, ch);
            }
        }
        else {
            // eat the rest of the number and publish an error
            while(isdigit(ch = get_char()))
                add_char_buffer(scanner_buffer, ch);
            unget_char(ch);
            syntax("malformed float number: %s", get_char_buffer(scanner_buffer));
            return ERROR_TOKEN;
        }
    }
    else
        unget_char(ch);

    return FNUM_TOKEN;
}

// When this is seen, we have a number. The character is passed as a
// parameter. could be a hex, decimal, or a float.
token_t read_number_top() {

    int ch = get_char();

    // first char is always a digit
    if(ch == '0') { // could be hex, octal, decimal, or float
        add_char_buffer(scanner_buffer, ch);
        ch = get_char();
        if(ch == 'x' || ch == 'X') {
            add_char_buffer(scanner_buffer, ch);
            return read_hex_number();
        }
        else if(ch == '.') {
            add_char_buffer(scanner_buffer, ch);
            return read_float_number();
        }
        else if(isdigit(ch)) { // is an octal number
            if(ch <= '7') {
                add_char_buffer(scanner_buffer, ch);
                return read_octal_number();
            }
            else {
                // it's a malformed number. eat the rest of it and post an error
                while(isdigit(ch = get_char()))
                    add_char_buffer(scanner_buffer, ch);
                unget_char(ch);
                syntax("malformed octal number: %s", get_char_buffer(scanner_buffer));
                return ERROR_TOKEN;
            }
        }
        else { // it's just a zero
            unget_char(ch);
            return INUM_TOKEN;
        }
    }
    else { // It's either a dec or a float.
        int finished = 0;
        add_char_buffer(scanner_buffer, ch);
        while(!finished) {
            ch = get_char();
            if(isdigit(ch))
                add_char_buffer(scanner_buffer, ch);
            else if(ch == '.') {
                add_char_buffer(scanner_buffer, ch);
                return read_float_number();
            }
            else {
                finished++;
                unget_char(ch);
                return INUM_TOKEN;
            }
        }
    }
    // If we reach here, then some syntax that is not covered has been submitted
    return ERROR_TOKEN;
}
