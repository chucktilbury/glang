/*
    Symbols are stored in a hash table. All symbols are decorated to represent
    the type attributes of the symbol. When a symbol is looked up, it gets
    undecorated so it can be found in the table. Names are decorated with a
    preceeding '$' and type parameters are decorated with a preceeding '@'.

    for example a class method defined in a class named some_cls as
    public void some_meth(int, string) is decorated as
    $some_cls$some_meth@int@string

    If that method defines an int variable named 'var', then that is stored as
    $some_cls$some_meth@int@string$var
    The type information is kept in the actual symbol table entry.

    If a symbol is defined in an anonymous block, such as a while loop, it is
    assigned a serial number according to the number of '{' that have been seen
    such as
    $some_cls$some_meth@int@string$0003$var

    When the above defined method is referenced in the source code, it will be
    seen as some_cls.some_meth(var1, var2). The resolver searches out the types
    of the parameters and any assignment to recreate the fully decorated name.

    When the symbol is rendered into C code, the '$' and '@' characters are
    converted to '_'.

 */

#include "common.h"

#include "symbols.h"
#include "char_buffer.h"
#include "hashtable.h"

static hashtable_t* htable;
static char_buffer_t deco_buffer;

// called by atexit
static void destroy_symbol_table() {

    destroy_char_buffer(deco_buffer);
    destroy_hash_table(htable);
}

/*
    Create the hash table and the decorator string buffer.
*/
void init_symbol_table() {

    htable = create_hash_table();
    deco_buffer = create_char_buffer();
}

/*
    Clear the decorator string
*/
void init_deco_str() {

    init_char_buffer(deco_buffer);
}

/*
    Add a name to the decorator string.
*/
void deco_add_name(const char* name) {

    add_char_buffer(deco_buffer, '$');
    add_char_buffer_str(deco_buffer, name);
}

/*
    Add a type to the decorator string.
*/
void deco_add_type(const char* type) {

    add_char_buffer(deco_buffer, '@');
    add_char_buffer_str(deco_buffer, type);
}

/*
    Truncate the decorator string after the class name.
*/
void deco_decapitate() {

}

/*
    Extract the class name from the decorated name. Return value of this
    function is allocated memory.
*/
const char* deco_extract_class(const char* name) {

}

/*
    Extract the name of the object from the decorated name. Return value of
    this function is allocated memory.
*/
const char* deco_extract_name(const char* name) {

}

/*
    Extract the type at the given index from the decorated name. If there
    is nothing at that index, then return NULL. Return value of this
    function is allocated memory.
*/
const char* deco_extract_type(const char* name, int index) {

}

/*
    Add the fully decorated name to the symbol table.
*/
void add_symbol(const char* name, symbol_mask_t mask) {

}

/*
    Find a fully decorated name in the symbol_table.
*/
symbol_mask_t get_symbol_mask(const char* name) {

}

double get_symbol_double(const char* name) {

}

uint64_t get_symbol_uint(const char* name) {

}

int64_t get_symbol_int(const char* name) {

}

const char* get_symbol_str(const char* name) {

}

void set_symbol_double(const char* name, double num) {

}

void set_symbol_uint(const char* name, uint64_t num) {

}

void set_symbol_int(const char* name, int64_t num) {

}

void set_symbol_str(const char* name, const char* str) {

}

void set_symbol_mask(const char* name, symbol_mask_t mask) {

}