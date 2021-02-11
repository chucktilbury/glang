
#ifndef __LOCAL_H__
#define __LOCAL_H__

typedef struct __file_stack {
    char* fname;
    FILE* fp;
    int line_no;
    int col_no;
    struct __file_stack* next;
} file_stack_t;

typedef struct {
    const char* str;
    token_t tok;
} token_map_t;

#ifdef SCANNER_ROOT
int nest_depth = 0;
file_stack_t* top = NULL;
char_buffer_t scanner_buffer;
int file_flag = 0;
int last_col;

/*
    this data structure must be in sorted order.
    To update, see the file keywordlist.txt in ./tests
    1. Add the keywords with the token.
    2. cat keywordlist.txt | sort
    3. copy/paste the result here.
*/
token_map_t token_map[] = {
    {"and", AND_TOKEN},
    {"as", AS_TOKEN},
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
    {"list", LIST_TOKEN},
    {"lte", LTE_TOKEN},
    {"lt", LT_TOKEN},
    {"map", MAP_TOKEN},
    {"namespace", NAMESPACE_TOKEN},
    {"neq", NEQ_TOKEN},
    {"not", NOT_TOKEN},
    {"or", OR_TOKEN},
    {"private", PRIVATE_TOKEN},
    {"protected", PROTECTED_TOKEN},
    {"public", PUBLIC_TOKEN},
    {"raise", RAISE_TOKEN},
    {"return", RETURN_TOKEN},
    {"string", STRING_TOKEN},
    {"super", SUPER_TOKEN},
    {"switch", SWITCH_TOKEN},
    {"true", TRUE_TOKEN},
    {"try", TRY_TOKEN},
    {"uint", UINT_TOKEN},
    {"void", VOID_TOKEN},
    {"while", WHILE_TOKEN},
};

const size_t token_map_size = (sizeof(token_map)/sizeof(token_map_t));

#else
extern int nest_depth;
extern file_stack_t* top;
extern char_buffer_t scanner_buffer;
extern int file_flag;
extern int last_col;
extern token_map_t token_map[];
extern const size_t token_map_size;
#endif

// defined in interface.c (see ../include/scanner.h)

// defined in numbers.c
token_t read_number_top();

// defined in scanner.c
void close_file();
int get_char();
void unget_char(int ch);
void skip_ws();
void eat_until_ws();
void eat_single_line();
void eat_multi_line();

// defined in strings.c
token_t read_dquote();
token_t read_squote();

// defined in symbols.c
token_t read_punct(int ch);
token_t read_word();

#endif
