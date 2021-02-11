
#include "common.h"
#include "scanner.h"
#include "local.h"

// when this is entered, a (/) has been seen. If the next character is a
// (/) or a (*), then we have a comment, otherwise we have a / operator.
static token_t comment_or_operator() {

    int ch = get_char();

    if(ch == '/') {
        // eat a sinlge line comment
        eat_single_line();
        return NONE_TOKEN;
    }
    else if(ch == '*') {
        // eat a multi line comment
        eat_multi_line();
        return NONE_TOKEN;
    }
    else {
        // not a comment, must be a / single-character operator
        unget_char(ch);
        return SLASH_TOKEN;
    }

    // probably an error for the parser to handle
    return END_OF_INPUT;
}

// Called by atexit()
static void destroy_scanner() {
    destroy_char_buffer(scanner_buffer);
    while(top != NULL)
        close_file();
}

/*
    Convert the given keyword string to a token. If it is not a keyword,
    then SYMBOL_TOKEN is returned. Note that this does not convert non-
    keywords to a token. Non-keywords cause SYMBOL_TOKEN to be returned
    as well.
*/
token_t str_to_token(const char* str) {

    // simple binary search
    token_t retv = SYMBOL_TOKEN;
    int start = 0, end = token_map_size - 1;
    int mid = (start + end) / 2;

    while(start <= end) {
        int spot = strcmp(str, token_map[mid].str);
        if(spot > 0)
            start = mid + 1;
        else if(spot < 0)
            end = mid - 1;
        else {
            retv = token_map[mid].tok;
            break;
        }

        mid = (start + end) / 2;
    }

    return retv;
}

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
            case END_FILE:
                file_flag++;
                tok = END_OF_FILE;
                finished++;
                break;
            case END_INPUT:
                tok = END_OF_INPUT;
                finished ++;
                break;
            case '\"':  // read a double quoted string
                tok = read_dquote();
                finished ++;
                break;
            case '\'': // read a single quoted string
                tok = read_squote();
                finished ++;
                break;
            case '/': // may have a comment or an operator
                tok = comment_or_operator();
                skip_ws();
                if(tok != NONE_TOKEN)
                    finished ++;
                // continue if it was a comment
                break;
            default:
                if(ch == END_FILE)
                    unget_char(ch);
                else if(isdigit(ch)) { // beginning of a number. Could be hex, dec, or float
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
                    warning("Unknown character, ignoring. (0x%02X) (%d) \'%c\'", ch, ch, ch);
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

    // this is contrived to look like a standard syntax error
    // calling this on an empty input stream will produce a wonky message
    FILE* fp = get_err_stream();
    fprintf(fp, "Syntax Error: %s: %d: %d: expected ",
                get_file_name(), get_line_no(), get_column_no());
    for(int i = 0; arr[i] != NONE_TOKEN; i++) {
        fprintf(fp, "%s", token_to_str(arr[i]));
        if(arr[i+1] != NONE_TOKEN)
            fprintf(fp, ",");
        fprintf(fp, " ");
    }
    fprintf(fp, "but got a %s.\n", token_to_str(token));
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
        syntax("expected a %s but got a %s.", token_to_str(tok), token_to_str(token));
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
void open_scanner_file(const char* fname) {

    nest_depth++;
    if(nest_depth > MAX_FILE_NESTING) {
        fatal_error("Maximum file nesting depth exceeded.");
        exit(1);
    }

    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        fatal_error("Cannot open input file: \"%s\": %s", fname, strerror(errno));
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
