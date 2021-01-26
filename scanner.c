/*
    Scanner

    This module separates the input text into tokens and then returns the token.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "scanner.h"
#include "memory.h"
#include "char_buffer.h"

#define END_INPUT -1

typedef struct __file_stack {
    char* fname;
    FILE* fp;
    int line_no;
    int col_no;
    struct __file_stack* next;
} file_stack_t;

static int nest_depth = 0;
static file_stack_t* top = NULL;
static char_buffer_t scanner_buffer;

typedef struct {
    const char* str;
    token_t tok;
} token_map_t;

// this data structure must be in sorted order.
static token_map_t token_map[] = {
    {"and", AND_TOKEN},
    {"bool", BOOL_TOKEN},
    {"break", BREAK_TOKEN},
    {"case", CASE_TOKEN},
    {"class", CLASS_TOKEN},
    {"constructor", CONSTRUCTOR_TOKEN},
    {"continue", CONTINUE_TOKEN},
    {"create", CREATE_TOKEN},
    {"default", DEFAULT_TOKEN},
    {"destroy", DESTROY_TOKEN},
    {"destructor", DESTRUCTOR_TOKEN},
    {"dict", DICT_TOKEN},
    {"do", DO_TOKEN},
    {"else", ELSE_TOKEN},
    {"entry", ENTRY_TOKEN},
    {"equ", EQUALITY_TOKEN},
    {"except", EXCEPT_TOKEN},
    {"false", FALSE_TOKEN},
    {"float", FLOAT_TOKEN},
    {"for", FOR_TOKEN},
    {"gte", GTE_TOKEN},
    {"gt", GT_TOKEN},
    {"if", IF_TOKEN},
    {"import", IMPORT_TOKEN},
    {"inline", INLINE_TOKEN},
    {"int", INT_TOKEN},
    {"lte", LTE_TOKEN},
    {"list", LIST_TOKEN},
    {"lt", LT_TOKEN},
    {"map", MAP_TOKEN},
    {"neq", NEQ_TOKEN},
    {"not", NOT_TOKEN},
    {"or", OR_TOKEN},
    {"raise", RAISE_TOKEN},
    {"string", STRING_TOKEN},
    {"super", SUPER_TOKEN},
    {"switch", SWITCH_TOKEN},
    {"true", TRUE_TOKEN},
    {"try", TRY_TOKEN},
    {"uint", UINT_TOKEN},
    {"void", VOID_TOKEN},
    {"while", WHILE_TOKEN},
    {NULL, NONE_TOKEN},
};

#define TOKEN_MAP_SIZE   (sizeof(token_map)/sizeof(token_map_t))

static void close_file() {

    if(top != NULL) {
        file_stack_t* fsp = top;
        top = top->next; // could make top NULL
        FREE(fsp->fname);
        fclose(fsp->fp);
        FREE(fsp);
    }
}

static int get_char() {

    int ch;
    int finished = 0;

    while(!finished) {
        if(top != NULL) {
            ch = fgetc(top->fp);
            if(ch == '\n') {
                top->line_no ++;
                top->col_no = 0;
                finished ++;
            }
            else if(ch == EOF) {
                close_file();
            }
            else {
                finished ++;
                top->col_no ++;
            }
        }
        else {
            ch = END_INPUT;
            finished ++;
        }
    }
    return ch;
}

static void unget_char(int ch) {
    ungetc(ch, top->fp);
}

static void skip_ws() {

    int ch, finished = 0;

    while(!finished) {
        ch = get_char();
        if(!isspace(ch)) {
            unget_char(ch);
            finished++;
        }
        else if(ch == END_INPUT)
            finished++;
    }
    // next char in input stream is not blank.
}

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
        fprintf(stderr, "WARNING: invalid hex escape code in string: '%c' is not a hex digit. Ignored.\n", ch);
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
        fprintf(stderr, "WARNING: invalid octal escape code in string: '%c' is not a octal digit. Ignored.\n", ch);
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
        fprintf(stderr, "WARNING: invalid decimal escape code in string: '%c' is not a decimal digit. Ignored.\n", ch);
        unget_char(ch);
    }
    else {
        int val = (int)strtol(tbuf, NULL, 10);
        add_char_buffer_int(scanner_buffer, val);
    }
}

// When this is entered, a back-slash has been seen in a dquote string.
// could have an octal, hex, or character escape.
static int get_string_esc() {

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
    return 0;
}

// When this is entered, a dquote has been seen. Check to see if the string
// is continued or if it has ended.
static int get_string_end() {

    skip_ws();
    int ch = get_char();
    if(ch == '\"')
        return 0;   // keep copying string
    else {
        unget_char(ch);
        return 3;   // string has ended
    }
}

// when this is entered, a (") has been seen and discarded. This function
// performs escape replacements, but it does not do formatting. that takes
// place in the parser. If consecutive strings are encountered, separated
// only by white space, then it is a string continuance and is a multi line
// string.
static token_t read_dquote() {

    int finished = 0;
    int ch, state = 0; // state 0 is copying the string to the buffer
    token_t tok = QSTRG_TOK;

    while(!finished) {
        ch = get_char();
        switch(state) {
            case 0:     // copying string
                switch(ch) {
                    case '\\':
                        state = 1;
                        break;
                    case '\"':
                        state = 2;
                        break;
                    default:
                        add_char_buffer(scanner_buffer, ch);
                        break;
                }
                break;
            case 1:     // seen a backslash
                state = get_string_esc();
                break;
            case 2:     // seen a (") character
                state = get_string_end();
                break;
            case 3:     // quitting
                finished ++;
                break;
            default:    // should never happen
                fprintf(stderr, "FATAL ERROR: Internal scanner error: invalid state in read_dquote()\n");
                exit(1);
                break;
        }
    }

    return tok;
}

// when this is entered, a (') has been seen and discarded. This function
// copies whatever is found into a string, exactly as it is found in the
// source code. If consecutive strings are found separated only by white
// space, then it is a single string that is being scanned.
static token_t read_squote() {

    int finished = 0;
    int ch, state = 0; // state 0 is copying the string to the buffer
    token_t tok = QSTRG_TOK;

    while(!finished) {
        ch = get_char();
        switch(state) {
            case 0:     // copying string
                switch(ch) {
                    case '\'':
                        state = 1;
                        break;
                    default:
                        add_char_buffer(scanner_buffer, ch);
                        break;
                }
                break;
            case 1:     // seen a (') character
                skip_ws();
                int ch = get_char();
                if(ch == '\'')
                    state = 0;   // keep copying string
                else {
                    unget_char(ch);
                    return 2;   // string has ended
                }
                break;
            case 2:     // quitting
                finished ++;
                break;
            default:    // should never happen
                fprintf(stderr, "FATAL ERROR: Internal scanner error: invalid state in read_squote()\n");
                exit(1);
                break;
        }
    }

    return tok;
}

// when this is entered, a (/) has been seen. If the next character is a
// (/) or a (*), then we have a comment, otherwise we have a / operator.
static token_t comment_or_operator() {

    int ch = get_char();

    if(ch == '/') {
        // eat a sinlge line comment
        while('\n' != get_char()) {}
        return NONE_TOKEN;
    }
    else if(ch == '*') {
        // eat a multi line comment
        while(ch != END_OF_INPUT) {
            ch = get_char();
            if(ch == '*') {
                ch = get_char();
                if(ch == '/')
                    return NONE_TOKEN;
            }
        }
    }
    else {
        // not a comment, must be a / single-character operator
        unget_char(ch);
        return SLASH_TOKEN;
    }

    // probably an error for the parser to handle
    return END_OF_INPUT;
}

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
            fprintf(stderr, "SYNTAX: malformed octal number: %s\n", get_char_buffer(scanner_buffer));
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
        if(ch == '+' || ch == '-')
            add_char_buffer(scanner_buffer, ch);
        else if(isdigit(ch)) {
            while(isdigit(ch = get_char())) {
                add_char_buffer(scanner_buffer, ch);
            }
        }
        else {
            // eat the rest of the number and publish an error
            while(isdigit(ch = get_char()))
                add_char_buffer(scanner_buffer, ch);
            unget_char(ch);
            fprintf(stderr, "SYNTAX: malformed float number: %s\n", get_char_buffer(scanner_buffer));
            return ERROR_TOKEN;
        }
    }
    else
        unget_char(ch);

    return FNUM_TOKEN;
}

// When this is seen, we have a number. The character is passed as a
// parameter. could be a hex, decimal, or a float.
static token_t read_number_top() {

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
                fprintf(stderr, "SYNTAX: malformed octal number: %s\n", get_char_buffer(scanner_buffer));
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

// Read a word from the input and then find out if it's a keyword or a symbol.
static token_t read_word() {

    int c;
    while(isalnum(c = get_char())) {
        add_char_buffer(scanner_buffer, c);
    }
    unget_char(c);
    const char* find = get_char_buffer(scanner_buffer);

    // simple binary search
    int start = 0, end = TOKEN_MAP_SIZE - 1;
    int mid = (start + end) / 2;
    while(start <= end) {
        int spot = strcmp(find, token_map[mid].str);
        if(spot > 0)
            start = mid + 1;
        else if(spot < 0)
            end = mid - 1;
        else
            return token_map[mid].tok;

        mid = (start + end) / 2;
    }

    return SYMBOL_TOKEN;
}

// When this is entered, a punctuation character has been read. If it's a (_)
// then we have a symbol, otherwise, it's an operator.
static token_t read_punct(int ch) {

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
                fprintf(stderr, "WARNING: unrecognized character in input: '%c' (0x%02X). Ignored\n", ch, ch);
                return NONE_TOKEN;
                break;
        }
    }
    return ERROR_TOKEN; // should never happen
}

// Called by atexit()
static void destroy_scanner() {
    destroy_char_buffer(scanner_buffer);
}

/**************************
    Interface functions
*/

const char* token_to_str(token_t tok) {

    // this is the ONLY place this behemouth is expanded.
    return TOK_TO_STR(tok);
}

/*
    Create the data for the scanner. This must be called befoer any other
    scanner function.
*/
void init_scanner() {

    scanner_buffer = create_char_buffer();
    atexit(destroy_scanner);
}

/*
    Returns the token in the token structure provided.
*/
token_t get_tok() {

    int ch, finished = 0;
    token_t tok = NONE_TOKEN;

    skip_ws();
    init_char_buffer(scanner_buffer);

    while(!finished) {
        ch = get_char();
        switch(ch) {
            case END_INPUT:
                tok = END_OF_INPUT;
                finished ++;
                break;
            case '\"':  // read a double quoted string
                tok = read_dquote();
                if(tok != NONE_TOKEN)
                    finished ++;
                break;
            case '\'': // read a single quoted string
                tok = read_squote();
                if(tok != NONE_TOKEN)
                    finished ++;
                break;
            case '/': // may have a comment or an operator
                tok = comment_or_operator();
                if(tok != NONE_TOKEN)
                    finished ++;
                // continue if it was a comment
                break;
            default:
                if(isdigit(ch)) { // beginning of a number. Could be hex, dec, or float
                    unget_char(ch);
                    tok = read_number_top();
                    if(tok != NONE_TOKEN)
                        finished ++;
                }
                else if(isalpha(ch)) { // could be a symbol or a keyword
                    unget_char(ch);
                    tok = read_word();
                    if(tok != NONE_TOKEN)
                        finished++;
                }
                else if(ispunct(ch)) { // some kind of operator (but not a '/' or a quote)
                    tok = read_punct(ch);
                    if(tok != NONE_TOKEN)
                        finished ++;
                }
                else {
                    fprintf(stderr, "WARNING: Unknown character, ignoring. \'%c\' (0x%02X)\n", ch, ch);
                }
        }
    }
    return tok;
}

/*
    Retrieve a token and compare it against a token array. If the received
    token is not in the array, then issue a syntax error and return the
    ERROR_TOKEN. The array must be terminated with the NONE_TOKEN. (AKA 0)
*/
token_t expect_tok_array(token_t* arr) {

    token_t token = get_tok();
    for(int i = 0; arr[i] != NONE_TOKEN; i++) {
        if(token == arr[i])
            return token;
    }

    fprintf(stderr, "SYNTAX: expected ");
    for(int i = 0; arr[i] != NONE_TOKEN; i++) {
        fprintf(stderr, "%s", token_to_str(arr[i]));
        if(arr[i+1] != NONE_TOKEN)
            fprintf(stderr, ",");
        fprintf(stderr, " ");
    }
    fprintf(stderr, "but got a %s.\n", token_to_str(token));
    return ERROR_TOKEN;
}

/*
    Retrieve the next token and check it against the specified token type. If
    they do not match then create a syntax error and unget the characters that
    made up the token. If the type matches, then copy the token to the buf and
    return it. If the buf parameter is NULL, then do not try to copy to the
    buffer.
*/
token_t expect_tok(token_t tok) {

    token_t token = get_tok();
    if(token != tok) {
        fprintf(stderr, "SYNTAX: expected a %s but got a %s.\n", token_to_str(tok), token_to_str(token));
        return ERROR_TOKEN;
    }
    else
        return token;
}

/*
    Open a file. This pushes the file onto a file stack and causes get_tok() to
    begin scanning the file. When the file ends, then scanning resumes where it
    left off when a new file was opened. It is encumbent on the caller to make
    sure that open_file is called between tokens.
*/
void open_file(const char* fname) {

    nest_depth++;
    if(nest_depth > MAX_FILE_NESTING) {
        fprintf(stderr, "ERROR: Maximum file nesting depth exceeded.\n");
        exit(1);
    }

    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        fprintf(stderr, "ERROR: Cannot open input file: \"%s\": %s\n", fname, strerror(errno));
        exit(1);    // TODO: handle this error better than simply making it fatal.
    }

    file_stack_t* fstk = (file_stack_t*)CALLOC(1, sizeof(file_stack_t));
    fstk->fname = STRDUP(fname);
    fstk->fp = fp;
    fstk->line_no = 1;
    fstk->col_no = 1;

    if(top != NULL)
        fstk->next = top;
    top = fstk;
}

/*
    Return the name of the currently open file. If no file is open, then return
    the string "no file open".
*/
const char* get_file_name() {

    if(top != NULL)
        return top->fname;
    else
        return "no open file";
}

/*
    Return the current line number of the currently open file. If there is no
    file open then return -1.
*/
int get_line_no() {

    if(top != NULL)
        return top->line_no;
    else
        return -1;
}

/*
    Return the current collumn number of the currently open file. If there is
    no file open then return -1.
*/
int get_column_no() {

    if(top != NULL)
        return top->col_no;
    else
        return -1;
}

/*
    Return the token string.
*/
const char* get_tok_str() {
    return get_char_buffer(scanner_buffer);
}

