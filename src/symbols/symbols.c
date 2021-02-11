/**
 * @file
 * symbols.c
 *
 * The symbol table is a hirarchical structure that maintains the names that
 * the translator is aware of. This uses the concept that a symbol is "owned"
 * by another symbol. For example, a class owns its methods. The hirarchy is
 * built to reflect this. Every symbol has as one of its members another symbol
 * table. The child table holds references that the parent symbol owns.
 *
 * When a class inherits another class, then the symbols in that class are
 * refrenced using a pointer to the parent class. If a symbol cannot be found in
 * the class declaration, then the parent class is searched. Symbols that are
 * duplicated by the parent can be referenced by specifying the parent name as
 * a part of the reference using dot '.' notation.
 *
 * Methods are decorated with the parameter types of the method to facilitate
 * method overloading. When a method is referenced, the types of the parameters
 * have to be looked up to validate the name of the method. If the parameter
 * types don't match, then the method reference fails. Method parameters are
 * separated by a '$' in the name.
 *
 * Symbols that are defined within a method are visible only within the block
 * in which they are defined. Symbols that are defined in an anonymous block,
 * such as while loop, are given a serial number. When a block is opened, then
 * the serial number is incremented and the number is stored in the symbol
 * table.
 *
 * NOTE: is this what is actually needed? Maybe a temporary symbol table is
 * the best option.
 *
 * Classes and imports are global to the file in which they are defined. Classes
 * can have a scope indicator of public or private that controls whether the
 * class is made visible when imported. Making a class protected does not make
 * sense because there is no way to inherit a module. The default scope of a
 * class is public.
 *
 * The root hash table is a global pointer in the symbol table module. All
 * symbols are referenced from there.
 *
 */

#include "common.h"
#include "scanner.h"

#include "symbols.h"

typedef struct __symbol_table_stack_t {
    symbol_table_t* tab;
    struct __symbol_table_stack_t* next;
} symbol_table_stack_t;

symbol_table_t* sym_tab; // root symbol table.

/**
 * When a parse rule is found that will have children, such as a class, then
 * this used to track it. The top of the stack is the current parent of any new
 * symbols that are parsed into the system.
 */
symbol_table_stack_t* top_stk;

/**
 * Push a symbol on the symbol stack.
 */
static void push_symbol_table(symbol_table_t* tab) {

    symbol_stack_t* sstk = MALLOC(sizeof(symbol_table_stack_t));
    sstk->tab = sstk;
    sstk->next = top_stk;
    top_stk = sstk;
}

/**
 * Pop a symbol from the symbol stack. Destroies the top of the stack. Don't
 * free the symbol because it is still in use.
 */
static symbol_table_t* pop_symbol_table() {

    if(top_stk != NULL) {
        symbol_table_t* tab = top_stk->tab;
        symbol_table_stack_t* sym_stk = top_stk;
        top_stk = tab->next;
        FREE(sym_stk);
        return sym;
    }
    return NULL;
}

/**
 * Peek at the top of the stack without popping it.
 */
static symbol_table_t* peek_symbol_table() {

    if(top_stk != NULL)
        return top_stk->tab;
    return NULL;
}

/**
 * Recursively destroy all allocated data in the symbol tree.
 */
static void recurse_destroy(hashtable_t* ht) {

    symbol_t* sym;

    for(sym = iterate_hash_table(ht, 1); sym != NULL; sym = iterate_hash_table(ht, 1)) {
        if(sym->table != NULL) {
            recurse_destroy(sym->table);
            if(sym->assign_type == SYM_STRING_TYPE && sym->const_val.str_val != NULL)
                FREE(sym->const_val.str_val);
        }
        destroy_hash_table(ht);
    }
}

/**
 * Called by atexit.
 */
static void destroy_symbol_table() {

    destroy_char_buffer(deco_buffer);
    //destroy_hash_table(htable);
    recurse_destroy(htable);
}

/**
 * Create the hash table and the decorator string buffer.
 */
void init_symbol_table() {

    symbol_table_t* sym_tab = (symbol_table_t*)create_hash_table();
    push_symbol_table(sym_tab);
    deco_buffer = create_char_buffer();

    atexit(destroy_symbol_table);
}

/**
 * Allocate memory for a symbol table and return it.
 */
symbol_t* create_symbol() {
    // A memory allocation error is a fatal error....
    return (symbol_t*)CALLOC(1, sizeof(symbol_t));
}

/**
 * Allocate a symbol table and return it. Also push the new symbol table on the
 * symbol table stack and assign if the symbol parameter is not NULL.
 */
symbol_table_t* create_symbol_table(symbol_t* sym) {

    symbol_table_t* stab = (symbol_table_t*)create_hash_table();
    if(sym != NULL) {
        sym->table = stab;
        push_symbol_table(stab);
    }
    return stab;
}

/**
 * Add the name to the symbol table that is on the top of the symbol table stack.
 * If there is no symbol table on the top of the stack, then create one.
 */
symbol_error_t add_symbol(const char* name, symbol_t* sym) {

    if(top_stk == NULL) {
        symbol_table_t* stab = create_symbol_table(NULL);
        push_symbol_table(stab);
    }

    hash_retv_t val = insert_hash(top_stk->tab, name, (void*)sym, sizeof(symbol_t));
    if(val == HASH_EXIST) {
        const char* str = undecorate_name(name);
        syntax("name already exists: %s", undecorate_name(str));
        FREE((void*)str);
        return SYM_EXISTS;
    }
    return SYM_NO_ERROR;
}

/**
 * Get the symbol data structure by name. This func copies the data into the
 * sym parameters.
 */
symbol_error_t get_symbol(symbol_table_t* tab, const char* name, symbol_t* sym) {

    hash_retv_t val = find_hash(tab, name, (void*)sym, sizeof(symbol_t));

    symbol_error_t retv;
    if(val == HASH_NO_ERROR)
        retv = SYM_NO_ERROR;
    else if(val == HASH_NOT_FOUND) {
        retv = SYM_NOT_FOUND;
        const char* str = undecorate_name(name);
        syntax("name not found: %s", str);
        FREE((void*)str);
    }

    return retv;
}

/**
 * Replace the symbol data with the structure supplied.
 */
symbol_error_t update_symbol(symbol_table_t* tab, const char* name, symbol_t* sym) {

    hash_retv_t val = replace_hash_data(tab, name, (void*)sym, sizeof(symbol_t));

    symbol_error_t retv;
    if(val == HASH_NO_ERROR)
        retv = SYM_NO_ERROR;
    else if(val == HASH_NOT_FOUND) {
        retv = SYM_NOT_FOUND;
        const char* str = undecorate_name(name);
        syntax("name not found: %s", str);
        FREE((void*)str);
    }

    return retv;
}

// TODO: resolving a symbol is a multi-step process. Compound symbols are
// resolved one segment at a time. For each segment, then the symbol that is
// found has to be cached and a new search proceeds from there. Situations
// where the class is redundantly specified should yield a warning but not
// an error. Some kind of cache reset method is needed. Can probably use the
// symbol table stack as a cache.

/**
 * This function resolves symbol refrences. Names are only referenced inside a
 * method. Names can be defined in a class or in a method. Names that are
 * defined in a class are resolved from the root symbol table. Names that are
 * defined in a method are resolved from the method's symbol table. If a symbol
 * cannot be found in the method's symbol table, then the class that the method
 * is a part of is searched. If is still not found, then the class that the
 * class inherited from is searched.
 *
 * Compound symbols are symbols that have dots '.' in them. A compound symbol is
 * resolved one segment at a time. The parser handles what to do about it if the
 * symbol cannot be found.
 *
 * If the symbol is found, then the assignment type is returned. If the symbol
 * cannot be found, then SYM_NOT_FOUND is returned.
 *
 */
int resolve_symbol(const char* name) {

    return SYM_NOT_FOUND;
}
