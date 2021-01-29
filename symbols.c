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

#include "scanner.h"
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
    atexit(destroy_symbol_table);
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

    const char* str = get_char_buffer(deco_buffer);
    size_t len = strlen(str);
    size_t idx;

    for(idx = 0; str[idx] != 0 && idx < len; idx++)
        if(str[idx] == '$')
            break;

    truncate_char_buffer(deco_buffer, idx);
}

/*
    Extract the class name from the decorated name. Return value of this
    function is allocated memory.
*/
const char* deco_extract_class(const char* name) {

    char* str = STRDUP(name);
    char* loc = strchr(str, '@');

    if(loc != NULL)
        *loc = 0;
    else {
        FREE(str);
        return NULL;
    }

    return str;
}

/*
    Extract the name of the object from the decorated name. Return value of
    this function is allocated memory.
*/
const char* deco_extract_name(const char* name) {

    char* str = STRDUP(name);
    char* loc = strchr(&str[1], '$');

    if(loc != NULL) {
        int len = strlen(loc);
        memmove(str, loc, len);
        str[len] = 0;
        return str;
    }
    else {
        FREE(str);
        return NULL;
    }
}

/*
    Extract the type name at the given index from the fully decorated symbol
    table entry. If there is nothing at that index, then return NULL. The
    returned value is an immutable allocated string.
*/
const char* deco_extract_type_str(const char* name, int index) {

    char* str = STRDUP(name);
    char* tmps;
    char* retv;

    tmps = strtok(str, "@"); // This is the class and the func name
    for(int i = 0; i <= index; i++) {
        tmps = strtok(NULL, "@"); // this is the type name at the current index
        if(tmps == NULL)
            break;  // index was not found
    }

    if(tmps != NULL) {
        retv = STRDUP(tmps);
    }
    else
        retv = NULL;

    FREE(str);
    return retv;
}

/*
    Quick and dirty function that converts the type name to a token value.
    If the name given is not a keyword (i.e. a number), then the SYMBOL_TOKEN
    is returned.
*/
token_t deco_type_to_tok(const char* name) {

    return str_to_token(name);
}

/*
    Add the fully decorated name to the symbol table from the deco_buffer.
*/
symbol_error_t add_symbol(symbol_t* sym) {

    const char* name = get_char_buffer(deco_buffer);
    hash_retv_t val = insert_hash(htable, name, (void*)sym, sizeof(symbol_t));
    if(val == HASH_EXIST) {
        // TODO: convert the decorated buffer into a readable prototype and print it.
        const char* str = undecorate_name(name);
        syntax("name already exists: %s", str);
        FREE((void*)str);
        return SYM_EXISTS;
    }
    return SYM_NO_ERROR;
}

/*
    Replace the symbol data with the structure supplied.
*/
symbol_error_t update_symbol(const char* name, symbol_t* sym) {

    hash_retv_t val = replace_hash_data(htable, name, (void*)sym, sizeof(symbol_t));

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

/*
    Get the symbol data structure by name. This func copies the data into the sym
    parameters.
*/
symbol_error_t get_symbol(const char* name, symbol_t* sym) {

    hash_retv_t val = find_hash(htable, name, (void*)sym, sizeof(symbol_t));

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

/*
    Convert a decorated name is un-decorate it to turn it into a readable prototype.
    Returns an allocated string. Does not check for a malformed string.
*/
const char* undecorate_name(const char* name) {

    char_buffer_t buf = create_char_buffer();
    const char* tpt = name;
    int finished = 0;
    int state = 0;


    while(!finished) {
        tpt++;
        switch(state) {
            case 0:
                if(*tpt == '$')
                    add_char_buffer(buf, '.');
                else if(*tpt == '@') {
                    add_char_buffer(buf, '(');
                    state = 1;
                }
                else
                    add_char_buffer(buf, *tpt);
                break;
            case 1:
                if(*tpt == '@')
                    add_char_buffer(buf, ',');
                else if(*tpt == 0) {
                    add_char_buffer(buf, ')');
                    finished++;
                }
                else {
                    add_char_buffer(buf, *tpt);
                }
                break;
        }
    }
    tpt = get_char_buffer(buf);
    destroy_char_buffer(buf);
    return STRDUP(tpt);
}

