# Glang

## Introduction

Glang is a compiled object language. The Glang source is translated to ANSI C and then compiled to object. The source code can be either configured as a single module that imports (includes) everything that it needs, or it can be independent modules that are linked together. In the end, the Glang language is a simplified C-like language that adds in classes and exceptions. There are some major differences in the way that strings are constructed and the way blocks of code are defined. Overriding class methods using different parameter types is supported. Single inheritance is supported. Overriding some operators is supported.  Intrinsic types (such as ```int```) can be inherited to augment their functionality. Glang does not have pointers. 

### Memory and libraries

Glang is a managed language in that it does have garbage collection. However, any library that can be linked to a C program can be linked to a Glang program, since the base language of Glang is C. However, that raises some questions about memory allocation, since many, if not most C libraries allocate memory that the Glang garbage collector will not have access to. Therefore, the keywords of ```create``` and ```destroy``` are used to wrap calls to the C memory allocation routines. These should only be used when building an interface class for an external library.  There may be others that will be added as required.

### Scope

In Glang, all classes are global. There can be no functions or data defined outside of a class. That having been said, there is no need to put everything in a single file. Inside a class, methods and data can be defined with a scope of ```public```, ```private```, or ```protected```.  These have the same meaning as the do in other languages such as C++. 

The classic header/implementation strategy of defining a functionality is recommended. When a module is imported, it is the normal case for only the class definition to be read and to link to the implementation.

### Type system

The Glang type system is similar to the C type system, but simplified. All of these first-class types are implemented as if they were classes with methods and state. They can be inherited to create an augmented version of the type.  The promotion rules are generally  such that a item can be cast to another type if there is no information lost. An expression result assumes the type that has the highest resolution of any entity found. For example, if an arithmetic expression has an ```int```, a ```bool```, and a ```float``` in it, then the result will be a float. If an expression has any comparison operators in it then the result is always boolean. Type casts has the form of ```type(value)```. A syntax error is issued if a type cannot be cast as requested and a warning is issued if a valid cast reduces the resolution of it item that is being cast. For example, it's possible to cast a ```bool``` to an ```int``` with no resolution lost, but if an ```int``` is cast to a ```bool``` then a warning is issued. Casting an ```int``` to a ```dict``` is a syntax error.

* An ```int``` is a 64 bit signed entity.
* A ```uint``` is an unsigned 64 bit entity.
* A ```float``` is a double.
* A ```bool``` can be cast to a ```int```, ```uint``` or a ```float```.  Casting any of those types to a ```bool``` generates a warning.
* A ```string``` is a first class type that cannot be indexed except through class methods.  Any type can be cast to a string. Casting a class to a string is the way to check for equivalence. 
* A ```dict``` is a hash that must contain all the same type and is indexed using a ```string``` as the index.
* A ```map``` is a hash that can contain any type and is indexed using a ```string```.
* A ```list``` is an array that is indexed using a ```int```. A negative value indexes from the end of the array.  A ```list``` must contain all the same types.
* A ```class``` is a complex user defined type that has methods and state.

### Control constructs

The same control constructs exist in Glang as in other languages, but the syntax is a little more regular to simplify the parser. All blocks must be enclosed with ```{}``` curly brackets. Supported control statements include ```if/else```, ```while```, ```do/while```, ```for```, ```switch/case```. Also, the ```try/except/raise``` keywords are used to implement exceptions. And ```break/continue``` are used in their expected way inside loops. A control statement takes the usual form. For example: ```while(is_true) {do stuff}```. A blank test, for example ```if {}``` or ```while {}``` is considered true.  There is no notion of a label in Glang, so things like ```switch/case``` constructs look like this: ```switch(variable) { case(literal) {do stuff}}```.  

### Strings

Glang supports formatted strings as well as unformatted strings. A string that is initialized with single quotes has not formatting done to it whatsoever. When a string is initialized with double quotes, then formatting is done on it. Unprintable characters can be specified as octals or hex and escaped characters are permitted. In addition, all strings have have substitutions when they are defined.  String rendering is implemented in the language.

* Octals conform to ```\\[0-7]{1,3}```
* Hex escapes conform to ```\\[xX][0-9a-fA-F]{1,3}```
* All of the C escapes are supported: ```\n, \r, \t, \b, \f, \v, \\, \", \', \?```. Any other character preceded by a back-slash is added to the string as itself. 

Strings can span lines simply by placing them on lines with nothing before or after them. For example:

```c
string some_name = "the first line"
    "the second line"
    "the third line"
```

Will create the string as ```"the first line\nthe second line\nthe third line"```. If the string was defined using single quotes for example:

```c
string some_name = 'first line'
    'second line'
    'third line'
```

The string will be defined as ```"first linesecond linethird line"``` with nothing added to taken out of it.

Formatting a string is done similar to other languages using ```{num}``` notation. For example, the string defined as:

```c
string some_name = "this the {0} thing called a {1}\n"(12, "string {0}"("thing"))
```

The string will be rendered as ```"this is the 12 thing called a string thing\n"```.

Trying to format a string that is defined using single quotes is a syntax error.

## Examples

This example code is contrived to illustrate the overall look and feel of Glang code. It's not intended to make sense. 

```c++
/* 
  The stuff module implements a class called "things", which has a protected method 
  called "do_things" and a string variable called the_things.
*/
import stuff
import system /* system has classes that implement things like printing and command args. */

class my_class(stuff.things) {
  int number // no ';' required, no assignment allowed.
  // The class definition must not contain the implementation of a method.
  public float do_something(int, bool) // default scope is private
  public string to_str()
  // overloaded declarations for the constructor
  constructor(int)
  constructor()
  // implicit destructor requires no declaration
}

// The types of the parameters was specified in the declaration. There is no
// need to specify them again. 
my_class.constructor(val) {
  // class members are always in scope. There is no need for "this" or "self",
  the_things = string(val)
  number = val
}

my_class.constructor() {
  super.things()
  number = 0
}

my_class.do_something(num, flag) {
  if(flag) {
    number += num
  }
  else(num > 10) { // There is no elseif clause
    number -= num
  }
  // This could also be written as else() {}
  else {  // A blank test clause is taken to be true.
    number *= num
  }
  return(float(number))
}

// This is a redundant method. Number and other types are converted to a string on
// demand.
my_class.to_str() {
  return(string(number))
}

// The constructor of the Entry class is the entry point of the program. 
class Entry(system.args) {
// No constructor declaration is required if the only one has no args.
}

Entry.constructor() {
  // Args class has an list of strings that are created when the program starts up
  // from the command line.
  my_class the_things = my_class(int(cmd_line[1]))
  print(the_things.do_something(10, true))
  switch(the_things.number) {
    case 10 { print("the number is 10\n")}
    case 21 { print("the number is 21\n")}
    default { print("the number is not 10 or 21, it's {0}"(string(the_things.number)))}
  }
}

```



## Implementation

The Glang parser is implemented using Bison and Flex. The output is emitted as the input is parsed, rather than building a tree and then walking it.  Some constructs, such as the class system constructor and destructor have to be emitted after the entire class has been parsed. Another instance of having to defer emitting is in the case of a method that uses exceptions. Which exceptions are being used and where must be known before the method is emitted because code has to be called when the method is entered.

### Constructor and destructor

Quite a lot of work has to be done when a class is created or destroyed. Methods must be mapped to pointers in the struct and any other initialization that needs to be performed must happen. So there are actually two constructors and destructions. There is a system-defined constructor that does the initialization that has to happen for the class to exist, such as allocation memory for it. Then, there is the optional user defined constructor. This is a function like any other, except that it is called when an object is created from the class. The system destructor is called when the class is either manually ```destroy```ed or when the garbage collector reclaims the memory. Care is taken to not have dependent memory objects that will cause problems if they are free()ed in the wrong order. The user defined destructor is only called if the object is manually ```destroy```ed. Otherwise, the garbage collector will collect any memory that is dangling. 

### Inheritance

Only single inheritance is supported. When one class inherits from another, then a pointer to the base class is placed in the class definition. It is then referred to as a part of the child class with respect to scoping rules.  When the class is constructed, then the base class has to be created, however, the user-defined constructor is only called if the child class specifies it with the ```super.``` keyword. Otherwise, it is assumed that the child class is going to initialize the base class. Where there are naming conflicts, the base class can still be accessed using the ```super.``` keyword. Otherwise the method or data in the base class is hidden or overridden by the child class. If a base class inherits from another class and the intent is that another class will inherit from that base class, then an interface must be provided. There is no ```super.super.``` construct to access the base class of a base class. 

### Imports

When a module is imported, then only the class definitions are read. All of the other data in the file is discarded with the assumption that the implementation of the import will be linked to this implementation.  When the class definitions are read, then they are stored in the symbol table in order to verify them for syntax purposes.  Imports are not carried over. That is if a module imports another module, the names that are nested deeper than one level will not be visible to the top level importer. If that is desired, then an interface class must be created to expose the deeper level imports. The idea is that a module may **use** imports but not **export** them without explicitly specifying it. Circular imports are prevented by simple depth counting. 

The import name specifies a file name, which is ultimately found by the system and opened for parsing. The module that imports the name is importing the classes in that file. It can then refer to the symbols that were imported using the name that they were imported under. For example: 

```C
import blart
class something(blart.someelse) {}
```

### Exceptions

Exceptions in Glang are implemented using the setjmp C library. A global stack of possible exceptions is kept and updated when a function is entered or left, before any user code is called for the method. A deeply nested function that causes an exception has to be able to jump to the right spot in the code, depending on what exception was invoked.