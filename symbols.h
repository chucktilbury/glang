
#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <stdint.h>
#include "hashtable.h"

// define the bits for the sym_type_mask
#define SYM_ISROOT      ((uint64_t)0x01 << 0)
#define SYM_ISCLASS     ((uint64_t)0x01 << 1)
#define SYM_ISCLASSMETH ((uint64_t)0x01 << 2)
#define SYM_ISCLASSVAR  ((uint64_t)0x01 << 3)
#define SYM_ISMETHOD    ((uint64_t)0x01 << 4)
#define SYM_ISMETHPARM  ((uint64_t)0x01 << 5)
#define SYM_ISMETHVAR   ((uint64_t)0x01 << 6)
#define SYM_ISVAR       ((uint64_t)0x01 << 7)
#define SYM_ISINT       ((uint64_t)0x01 << 8)
#define SYM_ISUINT      ((uint64_t)0x01 << 9)
#define SYM_ISLONG      ((uint64_t)0x01 << 10)
#define SYM_ISULONG     ((uint64_t)0x01 << 11)
#define SYM_ISFLOAT     ((uint64_t)0x01 << 12)
#define SYM_ISMAP       ((uint64_t)0x01 << 13)
#define SYM_ISDICT      ((uint64_t)0x01 << 14)
#define SYM_ISLIST      ((uint64_t)0x01 << 15)
#define SYM_ISBOOL      ((uint64_t)0x01 << 16)
#define SYM_ISIMPORT    ((uint64_t)0x01 << 17)
#define SYM_HASCONST    ((uint64_t)0x01 << 18)
#define SYM_HASCHILD    ((uint64_t)0x01 << 19)

// error conditions
#define SYM_NOERROR     ((uint64_t)0)
#define SYM_NOTFOUND    ((uint64_t)0x01 << 63)
#define SYM_DUPLICATE   ((uint64_t)0x01 << 62)
#define SYM_GENERR      ((uint64_t)0x01 << 61)

#define SYM_ISERROR(s)  SYM_TEST((s)->mask, (SYM_NOTFOUND|SYM_DUPLICATE|SYM_GENERR))

// utility macros
#define SYM_GETMASK(s)  ((s)->mask)
#define SYM_TEST(s, b)  ((uint64_t)((s)->mask)&(b))
#define SYM_SET(s, b)   ((uint64_t)((s)->mask)|=(b))
#define SYM_CLEAR(s, b) ((uint64_t)((s)->mask)&=(~(b)))

typedef uint64_t symbol_mask_t;

typedef struct _symbol_t {
    symbol_mask_t mask;
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
const char* deco_extract_type(const char* name, int index);
void add_symbol(const char* name, symbol_mask_t mask);
symbol_mask_t get_symbol_mask(const char* name);
double get_symbol_double(const char* name);
uint64_t get_symbol_uint(const char* name);
int64_t get_symbol_int(const char* name);
const char* get_symbol_str(const char* name);
void set_symbol_double(const char* name, double num);
void set_symbol_uint(const char* name, uint64_t num);
void set_symbol_int(const char* name, int64_t num);
void set_symbol_str(const char* name, const char* str);
void set_symbol_mask(const char* name, symbol_mask_t mask);

#endif
