#ifndef __PARSER_H__
#define __PARSER_H__

typedef enum {
    PARSE_TOP,
    PARSE_ERROR,
    PARSE_IMPORT,
    PARSE_ENDING,
} parse_state_t;

void init_parser(const char* fname);
int parse();

#endif