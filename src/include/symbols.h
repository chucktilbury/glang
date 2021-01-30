
#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <stdint.h>
#include "hashtable.h"
#include "scanner.h"

typedef enum {
    SYM_NO_ERROR = 300,
    SYM_EXISTS,
    SYM_NOT_FOUND,
    SYM_ERROR,
} symbol_error_t;

typedef enum {
    ASST_INT = 325,
    ASST_UNT,
    ASST_FLOAT,
    ASST_BOOL,
    ASST_STRING,
    ASST_DICT,
    ASST_MAP,
    ASST_LIST,
    ASST_CLASS,
} assignment_type_t;

typedef enum {
    NAMET_CLASS = 350,
    NAMET_METHOD,
    NAMET_VAR,
    NAMET_CONST,
    NAMET_IMPORT,
} name_type_t;

typedef enum {
    SCOT_PUBLIC = 375,
    SCOT_PRIVATE,
    SCOT_PROTECTED,
} symbol_scope_t;

typedef struct _symbol_t {
    name_type_t name_type;
    assignment_type_t assign_type;
    symbol_scope_t scope;
    union {
        uint64_t uint_val;
        int64_t int_val;
        double float_val;
        char* str_val;
    } const_val;
} symbol_t;

void init_symbol_table();
void init_deco_str();

void deco_add_name(const char* name);
void deco_add_type(const char* type);
void deco_decapitate();
const char* deco_extract_class(const char* name);
const char* deco_extract_name(const char* name);
token_t deco_extract_type(const char* name, int index);
const char* deco_extract_type_str(const char* name, int index);

symbol_error_t add_symbol();
symbol_error_t update_symbol(const char*, symbol_t*);
symbol_error_t get_symbol(const char* name, symbol_t* sym);

const char* undecorate_name(const char* name);

#endif
