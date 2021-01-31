/*
    Scanner

    This module separates the input text into tokens and then returns the token.
*/
#include "common.h"

#include "scanner.h"
#include "char_buffer.h"
#define SCANNER_ROOT
#include "local.h"


void close_file() {

    if(top != NULL) {
        file_stack_t* fsp = top;
        top = top->next; // could make top NULL
        FREE(fsp->fname);
        fclose(fsp->fp);
        FREE(fsp);
    }
}

int get_char() {

    int ch;

    // defer closing the file until after the END_OF_FILE token has been read.
    if(file_flag) {
        file_flag = 0;
        close_file();
    }

    if(top != NULL) {
        ch = fgetc(top->fp);
        if(ch == EOF) {
            ch = END_FILE;
        }
        else if(ch == '\n') {
            top->line_no ++;
            last_col = top->col_no;
            top->col_no = 1;
        }
        else {
            top->col_no ++;
            //last_col = top->col_no;
        }
    }
    else {
        ch = END_INPUT;
    }
    return ch;
}

void unget_char(int ch) {

    if(top != NULL) {
        ungetc(ch, top->fp);
        if(ch == '\n') {
            top->line_no--;
            top->col_no = last_col;
        }
        else {
            if(top->col_no > 1)
                top->col_no --;
            else
                top->col_no = 1;
        }
    }
}

void skip_ws() {

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

// used for the scanner to recover from an error
void eat_until_ws() {

    int ch;
    while(!isspace(ch = get_char()))
        add_char_buffer(scanner_buffer, ch);
    unget_char(ch);
}

// Eat a single line comment after the initial '/' has been seen.
void eat_single_line() {

    int ch, finished = 0;

    while(!finished) {
        ch = get_char();
        if(ch == '\n' || ch == END_FILE) {
            //return NONE_TOKEN;
            unget_char(ch);
            break;
        }
    }
}

// Eat a multi line comment after the initial '/' has been seen.
void eat_multi_line() {

    int ch, state = 0;

    while(ch != END_OF_INPUT) {
        ch = get_char();
        switch(state) {
            case 0:
                if(ch == '*')
                    state = 1;
                //fprintf(stderr, "0");
                break;
            case 1:
                if(ch == '/')
                    state = 2;
                else if(ch != '*')
                    state = 0;
                //fprintf(stderr, "1");
                break;
            case 2:
                unget_char(ch);
                return;
        }
    }
}
