#ifndef __SCANNER_H__
#define __SCANNER_H__

typedef enum {
    NONE_TOKEN = 0,
    ERROR_TOKEN,
    END_INPUT,
    END_FILE,
    END_OF_INPUT = 200,
    SYMBOL_TOKEN,
    UNUM_TOKEN,
    INUM_TOKEN,
    FNUM_TOKEN,
    ONUM_TOKEN,
    QSTRG_TOKEN,
    SLASH_TOKEN,
    MUL_TOKEN,
    MOD_TOKEN,
    COMMA_TOKEN,
    SEMIC_TOKEN,
    COLON_TOKEN,
    OSQU_TOKEN,
    CSQU_TOKEN,
    OCUR_TOKEN,
    CCUR_TOKEN,
    OPAR_TOKEN,
    CPAR_TOKEN,
    DOT_TOKEN,
    EQU_TOKEN,
    LT_TOKEN,
    GT_TOKEN,
    SUB_TOKEN,
    ADD_TOKEN,
    NOT_TOKEN,
    EQUALITY_TOKEN,
    LTE_TOKEN,
    GTE_TOKEN,
    DEC_TOKEN,
    INC_TOKEN,
    NEQ_TOKEN,
    AND_TOKEN,
    OR_TOKEN,
    FOR_TOKEN,
    IF_TOKEN,
    ELSE_TOKEN,
    WHILE_TOKEN,
    DO_TOKEN,
    SWITCH_TOKEN,
    CASE_TOKEN,
    BREAK_TOKEN,
    CONTINUE_TOKEN,
    TRY_TOKEN,
    EXCEPT_TOKEN,
    RAISE_TOKEN,
    DEFAULT_TOKEN,
    CREATE_TOKEN,
    DESTROY_TOKEN,
    SUPER_TOKEN,
    ENTRY_TOKEN,
    CLASS_TOKEN,
    CONSTRUCTOR_TOKEN,
    DESTRUCTOR_TOKEN,
    IMPORT_TOKEN,
    TRUE_TOKEN,
    FALSE_TOKEN,
    VOID_TOKEN,
    BOOL_TOKEN,
    MAP_TOKEN,
    DICT_TOKEN,
    LIST_TOKEN,
    UINT_TOKEN,
    INT_TOKEN,
    FLOAT_TOKEN,
    STRING_TOKEN,
    INLINE_TOKEN,
    PUBLIC_TOKEN,
    PRIVATE_TOKEN,
    PROTECTED_TOKEN,
} token_t;

#define TOK_TO_STR(t) \
    (((t)==ERROR_TOKEN)? "ERROR": \
    ((t)==END_OF_INPUT)? "END OF INPUT": \
    ((t)==SYMBOL_TOKEN)? "symbol": \
    ((t)==UNUM_TOKEN)? "unsigned number": \
    ((t)==INUM_TOKEN)? "signed number": \
    ((t)==FNUM_TOKEN)? "float number": \
    ((t)==ONUM_TOKEN)? "octal number": \
    ((t)==QSTRG_TOKEN)? "quoted string": \
    ((t)==SLASH_TOKEN)? "'/'": \
    ((t)==MUL_TOKEN)? "'*'": \
    ((t)==MOD_TOKEN)? "'%'": \
    ((t)==COMMA_TOKEN)? "','": \
    ((t)==SEMIC_TOKEN)? "';'": \
    ((t)==COLON_TOKEN)? "':'": \
    ((t)==OSQU_TOKEN)? "'['": \
    ((t)==CSQU_TOKEN)? "']'": \
    ((t)==OCUR_TOKEN)? "'{'": \
    ((t)==CCUR_TOKEN)? "'}'": \
    ((t)==OPAR_TOKEN)? "'('": \
    ((t)==CPAR_TOKEN)? "')'": \
    ((t)==DOT_TOKEN)? "'.'": \
    ((t)==EQU_TOKEN)? "'='": \
    ((t)==LT_TOKEN)? "'<' or 'lt'": \
    ((t)==GT_TOKEN)? "'>' or 'gt'": \
    ((t)==SUB_TOKEN)? "'-'": \
    ((t)==ADD_TOKEN)? "'+'": \
    ((t)==NOT_TOKEN)? "'!' or 'not'": \
    ((t)==EQUALITY_TOKEN)? "'==' or 'equ'": \
    ((t)==LTE_TOKEN)? "'<=' or 'lte'": \
    ((t)==GTE_TOKEN)? "'>=' or 'gte'": \
    ((t)==DEC_TOKEN)? "'decrement'": \
    ((t)==INC_TOKEN)? "'increment'": \
    ((t)==NEQ_TOKEN)? "'!=' or 'neq'": \
    ((t)==AND_TOKEN)? "'&' or 'and'": \
    ((t)==OR_TOKEN)? "'|' or 'or'": \
    ((t)==FOR_TOKEN)? "'for'": \
    ((t)==IF_TOKEN)? "'if'": \
    ((t)==ELSE_TOKEN)? "'else'": \
    ((t)==WHILE_TOKEN)? "'while'": \
    ((t)==DO_TOKEN)? "'do'": \
    ((t)==SWITCH_TOKEN)? "'switch'": \
    ((t)==CASE_TOKEN)? "'case'": \
    ((t)==BREAK_TOKEN)? "'break'": \
    ((t)==CONTINUE_TOKEN)? "'continue'": \
    ((t)==TRY_TOKEN)? "'try": \
    ((t)==EXCEPT_TOKEN)? "'except": \
    ((t)==RAISE_TOKEN)? "'raise": \
    ((t)==DEFAULT_TOKEN)? "'default'": \
    ((t)==CREATE_TOKEN)? "'create'": \
    ((t)==DESTROY_TOKEN)? "'destroy'": \
    ((t)==SUPER_TOKEN)? "'super'": \
    ((t)==ENTRY_TOKEN)? "'entry'": \
    ((t)==CLASS_TOKEN)? "'class'": \
    ((t)==CONSTRUCTOR_TOKEN)? "'constructor'": \
    ((t)==DESTRUCTOR_TOKEN)? "'destructor'": \
    ((t)==IMPORT_TOKEN)? "'import'": \
    ((t)==TRUE_TOKEN)? "'true'": \
    ((t)==FALSE_TOKEN)? "'false'": \
    ((t)==VOID_TOKEN)? "'void'": \
    ((t)==BOOL_TOKEN)? "'bool'": \
    ((t)==MAP_TOKEN)? "'map'": \
    ((t)==DICT_TOKEN)? "'dict'": \
    ((t)==LIST_TOKEN)? "'list'": \
    ((t)==UINT_TOKEN)? "'uint'": \
    ((t)==INT_TOKEN)? "'int'": \
    ((t)==FLOAT_TOKEN)? "'float'": \
    ((t)==STRING_TOKEN)? "'string'": \
    ((t)==PUBLIC_TOKEN)? "'public'": \
    ((t)==PRIVATE_TOKEN)? "'private'": \
    ((t)==PROTECTED_TOKEN)? "'protected'": \
    ((t)==INLINE_TOKEN)? "'inline'": "UNKNOWN")

#include <stdint.h>

#define MAX_FILE_NESTING    (15)

// interface prototypes
const char* token_to_str(token_t);
void init_scanner();
token_t get_tok();
token_t expect_tok_array(token_t*);
token_t expect_tok(token_t);
void open_scanner_file(const char*);
const char* get_file_name();
int get_line_no();
int get_column_no();
const char* get_tok_str();

#endif