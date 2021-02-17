
# Symbol Table

The symbol table is a central part of the parser. It is used to store symbol definitions and then look up the
symbol references to verify that the type information is applicable to the usage. Symbol references that have
not been found in the table, or that are have an incompatible type for the way they are being applied, represent a syntax error. 

Symbols are organized by blocks. That is, the scope of a symbol is block-oriented, similar to the way that symbols in C are block oriented. Classes have exactly one block.  Syntactically, method definitions are outside of the class, but semantically, they are inside it. Therefore, a method definition has direct access to names that are defined in the class. An import has a block with classes embedded in it.  When a module is imported, only class declarations are read into the symbol table, since that gives access to every name that the current module will have access to. The current module has a root symbol table in which classes and imports are kept. Classes and imports are scoped globally to the module. Imported names are accessed through the name that was imported using dot notation.  

The symbol table API saves a symbol definition name after it has been completely read. All of the information, such as scope and type are known at that point. Symbols are saved in a tree-like structure where nodes of the tree have their own symbol table. Whether or not a node has child branches is known by the type of the node. Classes, Imports and methods have child nodes and so the symbol table associated with the node is valid. A symbol's position in the tree shows whether it's in scope or not. If it cannot be found then it is not in scope. 

As symbols are read from the input, their place in the hierarchy is apparent. The API uses a stack to keep track of the current symbol context. When a ```{``` is encountered, a context is pushed on the stack. When a ```}``` is encountered, a context is popped. 

As symbol references are encountered, the symbol resolver looks up each one from the current context and follows a specific and rigid set of rules to locate symbol information. These steps are detailed below.

Note that storing and resolving symbols are completely separate operations and the only interaction that they have is the symbol table itself.  There is no danger of any race condition because the parser is single threaded, but it does mean that storing and resolving symbols cannot share any state. Adding a symbol must take place in a single location and once it's added, it cannot be changed. 

Symbols are stored in a hash table using the name of the symbol. The data structure of a symbol looks like this:

```c
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

```

* The name type is the lexical representation of the symbol. For example a class name or a method name are
lexical representations. There are implications that govern the contents of the symbol data structure. For
example, only a variable or a method can have an assign type of ```int```.

* The assign type is the type that this symbol might be assigned to. For example a symbol that has the
assign type of ```dict``` cannot be assigned to a variable of type ```int```. This field is used when evaluating
expressions as well.

* Scope is the visibility of the definition. For example, a ```private``` class object can only be "seen"
within the class definition.

* The table exists when the symbol represented by the data structure has "child" symbols. For example, a class
has variables and methods. Those elements will be found in this symbol table. Variables that are ***defined*** in
a method will be found in the symbol table that is a part of the symbol that represents that method.

* The constant value is where the current value of a symbol is located. If the symbol is a built in type, such as an ```int```, then the value is placed there. If it's a user-defined type (a class) then a pointer to the symbol table entry for that definition is placed in the ```const_val```. Other types such as ```dict```, ```list```, and ```map``` have internal pointers to their implementation that is built as a pseudo-class. From the point of view of the source code, they behave exactly like classes, but they are implemented as internal types.

## The API

The symbol table API is used by the parser and the table is written to when a symbol is defined. When a symbol is
referenced, then the symbol table is read. This section documents the conceptual API that implementes these two
operations.

### Symbol Storage

When the parser starts up, there is a root symbol table that already exists. Entries in this table can only include
import names and class names. All entries in the root symbol table are considered to be global in scope. The symbol
tables under  those variables have the contents of the names. The root symbol table is a anonymous name space. 

* Imports are  only partially read so that the names and the type information is stored. The implementation details of an imported name is taken to be referenced by the linker.  Only the class declarations are read.  When the import is read, then the class information will be referenced using the import name.  The import name is used as a local name space for the module being compiled. 
* Classes, as one would expect, contain the methods and attributes associated with the class. 

As the parser chews through the input,  it encountered names in a particular order. Names must be defined before they are referenced. For example, a class declaration must have been seen before a method implementation for that class is encountered.  Multiple declarations for the same class name at the same level are considered to be syntax errors. 

The symbol storage code uses the notion of a stack to establish a "current" name space for new symbol declarations.  It is expected that the current symbol will be built up in a local data structure (probably defined on the stack) and then saved to the symbol table when it is complete. 

While symbols are being created, when a symbol that represents a class name, an import name, or a method name is committed, to the symbol table, then that name has to be pushed on the stack. This should be done when the first ```{``` is seen. When the last ```}``` is seen, then the symbol should be popped from the stack.  When an anonymous block is encountered, then a name is generated for it in the form of a global serial number. Semantically, only variables can be defined in an anonymous block and they are not visible outside of it. However, variables defined in an enclosing block can be seen in a block that is enclosed by it.

### Symbol References

When a symbol is being resolved, there are specific rules and it can get rather complicated. When any name is referenced, it is taken in segments. A name can have one or more segments. A name that has only one segment is called a simple symbol. Names that have more than one segment is called a complex symbol. Name segments are separated by a dot (```.```).  Each name segment is attempted to resolve as a variable that is as local to the current context as possible. 

#### simple symbols

A simple symbol is one that does not have a ```.``` in it. The first place to look is the current context. If the symbol cannot be found in the current context, then the next context up is searched, unless it's a method.  If a method is encountered, then the class to which the method belongs is searched.  

A simple name can also refer to a class variable or a local. When a local shadows a class variable, then a warning is emitted. This can be resolved by specifying the class variable as a complex symbol that specifies the class name. 

Consider this:

```c++
class some_class {
    private int some_var;
    public int some_method();
}

some_class.some_method() {
    int some_var = 0; // this line produces a warning.
    some_class.some_var = 0; // this line accesses the class variable.
    return some_var; // this line returns the value of the local variable
}
```



#### complex symbols

A complex symbol is a string of simple symbols that are separated by a dot (```.```) character.  A complex symbol can represent an attribute of a class, a class within an import name space, or a method called on a ```dict```, a ```list```, or a ```map```. 

#### resolving a symbol

The purpose of resolving a symbol is to find out the attributes that the name was defined with. A complex symbol is regraded as a sort of path to use to find the original definition.  Symbols are resolved as locally as possible. For example, there may be a class declared with a particular name, and there may be a method in another class that uses the same name as a local variable in one of its methods. This is permissible and does not create a warning if the local does not "shadow" another name.  This is implemented by searching for a variable as locally as possible. Searches follow this order:

1. For the first element in a complex name.
   1. Search the current symbol context for the symbol. If it's not found then go to step 2, otherwise, return it.
   2. If the current context is a class, then retreat to the root and look for a class name or an import name space. Otherwise, retreat toward the root one context level and search the symbol table. if it's not found, then go to step 3, otherwise return it. The current context is pushed on the context stack for the next part of the symbol.
   3. The symbol was not found.
2. For the not first element in a complex name.
   1. If the name has ended, then pop the context to get the original one.  Otherwise, search the current context for the new symbol segment. If it's found, then return it, otherwise, go to step 2.
   2. 

A complex symbol is one that does contain a ```.``` character.  A complex symbol can specify a local ```dict```, ```list```, or ```map``` variable,  or it could specify a ```class``` name or a ```import``` name space. They are resolved in that order. First the local name space is checked, and then the current class, and then the imported name spaces. 

