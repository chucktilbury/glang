
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
    // no assignment type is allowed
    SYM_NO_TYPE = 325,
    SYM_INT_TYPE,
    SYM_UINT_TYPE,
    SYM_FLOAT_TYPE,
    SYM_BOOL_TYPE,
    SYM_STRING_TYPE,
    SYM_DICT_TYPE,
    SYM_MAP_TYPE,
    SYM_LIST_TYPE,
    // These two are handled the same except that the SYM_INHERIT_TYPE
    // type has a pointer to the symbol that the class inherited from
    // in the value union. The SYM_CLASS_TYPE means that there is no
    // base class.
    SYM_CLASS_TYPE,
    SYM_INHERIT_TYPE,
} assignment_type_t;

typedef enum {
    SYM_CLASS_TYPE = 350,
    SYM_METHOD_TYPE,
    SYM_VAR_TYPE,
    SYM_CONST_TYPE,
    SYM_IMPORT_TYPE,
    // name is a system-wide serial number and is only accessed at the top of
    // the symbol table stack.
    SYM_ANON_TYPE,
} name_type_t;

typedef enum {
    SYM_PUBLIC_TYPE = 375,
    SYM_PRIVATE_TYPE,
    SYM_PROTECTED_TYPE,
} symbol_scope_t;

typedef hashtable_t symbol_table_t;

typedef struct _symbol_t {
    name_type_t name_type;
    assignment_type_t assign_type;
    symbol_scope_t scope;
    symbol_table_t table;
    union {
        uint64_t uint_val;
        int64_t int_val;
        double float_val;
        char* str_val;
        symbol_t* symbol;
    } const_val;
} symbol_t;

// defined in symbols.c
void init_symbol_table();
symbol_error_t add_symbol();
symbol_error_t update_symbol(const char*, symbol_t*);
symbol_error_t get_symbol(const char* name, symbol_t* sym);

#endif
