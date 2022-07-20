# Ownership
In ALT script, instances of non-value classes are passed among [value names](Names.md) via references. They are also called [reference values](https://en.wikipedia.org/wiki/Reference_(computer_science)). [Strings](String.md), [streams](Stream.md), [arrays](Array.md) and [tuples](Tuple.md) are  reference values. Any object created from a non-value class is a reference value.

A name that holds a reference value does not have a physical storage for the entire data required by the value, rather, it holds a reference to a storage allocated in another place (a common value pool). Therefore, it is possible that multiple names hold the reference to the same reference value.

The Ownership is a set of rules that governs how these reference values are allocated and released in the common value pool . Each value name  exists in a [scope](Scopes.md), which is a region of a program text in which the declared name is valid. When the name goes away (the program control flow exits from the scope where the name is defined), the ownership rule determines whether the value referred by the name should be released in the common value pool.

## Ownership Rules for Composite Values

### Owner Rules
* **Mandatory Rule**: each composite value has an owner which is a declared name or an element inside the composite value.
* **Uniqueness Rule**: There can only be one owner at a time
* **Discard Rule**: When the owner goes away (the declared name goes out of scope, or the element inside a composite value that is released),  the value will be released. 
To illustrate the rules of owner, let's consider string tuple value intruduced in a block scope:
```altscript
{
    s := ("Hello", "World");   // a name 's' is declared here and `s' holds a reference to
                               // the tuple value ("Hello", "World") allocated in the pool.
                               // `s` owns the value
    // other stuff here
}                              // the scope is over, `s` is gone, and the tuple value is
                               // released
```
When the declared name is introduced with an initial value, the name holds a reference to the value and owns the value, if we pass the reference to another name, the owner uniqueness rule must be kept: 
```altscript
{
    s1 := ("Hello", "World"); // a name 's1' is declared here and `s1' holds a reference to
                              // the tuple value ("Hello", "World") allocated in the pool.
                              // `s1` owns the value
    s2 := s1;                 // pass the reference held in `s1` to `s2`, and `s2' holds the
                              // reference to the same string value but does not own it
    // other stuff here
}                             // the scope is over, `s1` is gone, and the string value is
                              // released. `s2` is gone too, but it has nothing to do to its
                              // referred value because it is not the owner of the value.
```
If the value is changed via any name that refers to the value, all names will reflect the change:
```altscript
{
    s1 := ("Hello", "World"); // 's1' owns the tuple value ("Hello", "World")
    s2 := s1;                 // `s2' refers to ("Hello", "World"), but does not own it
    s1[0] = "你好";           // both 's1' and `s2' refers to ("你好", "World")
    s2[1] = "世界";           // both 's1' and `s2' refers to ("你好", "世界")
} 
```
If the owner changes its reference to refer to a different value, the original value will be discarded, and other non-owner names that refer to the original value will refer to a null value:
```altscript
{
    s1 := ("Hello", "World"); // 's1' owns the tuple value ("Hello", "World")
    s2 := s1;                 // `s2' refers to ("Hello", "World"), but does not own it
    s1 = ("你好", "世界");     // the tuple value ("Hello", "World") is discarded.
                              // 's1' refers to a new value ("你好", "世界")
                              // `s2` refers to a null value
    first_word := s2[0];      // Error: Index on empty container
} 
```
However, if the non-owner changes its reference to refer to a different value, the owner and other non-owners will still refer to the original value:
```altscript
{
    s1 := ("Hello", "World"); // 's1' owns the tuple value ("Hello", "World")
    s2 := s1;                 // `s2' refers to ("Hello", "World"), but does not own it
    s2 = ("你好", "世界");     // `s2` refers to a new value ("你好", "世界")
                              // 's1' is not affected
    first_word := s1[0];      // first_word refers to "Hello"
} 
```
Above examples illustrated the owner rules: each composite value must have an owner (mandatory rule), only one owner at a time (uniqueness rule), only the owner can release the referred value (discard rule), and when the owner discards the value, renters will refer to a null value (nullness rule). In order to keep the uniqueness, when we pass the reference from an owner to another name, the other name refers to the same value but does not own it. However, to keep the uniqueness, we can also:
* Transfer the ownership to another name when we pass the reference, and as a result, the name that takes the reference becomes the new owner and the original owner becomes a renter, or
* copy the value so the reference passed to another name ia a new reference to a copy and the name that takes the new reference is the owner of copy.

Let’s explore these situations now.

### Ownership Transfer Rules

The ownership transfer rule governs how the ownership is transferred when we pass the reference to another name. Before discussing this, we need to cover the concept of [scope](Scopes.md) first. We have four kinds of scopes: object scope, class scope, input/output scope, and block scope.

The ownership transfer rule governs how the ownership is transferred when we pass the reference to another name. Before discussing this, we need to cover the concept of [scope](Scopes.md) first. We have four kinds of scopes: object scope, class scope, input/output scope, and block scope.

The input/output scope and block scope are local scopes, kind of temporary scopes created for a [routine](Routines.md) call. Names introduced in a local scope are stored in a [call stack](https://en.wikipedia.org/wiki/Call_stack). When the execution enters into a local scope, the space required by the storage of the names within the scope are pushed into the stack. When the execution exits from a local scope, the space pushed for the scope is popped out and all names introduced in the local scope are gone. The stack naturally maintains a nested structure so that local sopes can be nested inside one another.

The object scope or the class scope is an owner scope created for the lifetime of its associated object or class (owner). Names introduced in a full scope are stored in a space allocated in the common value pool for its associated owner. When the owner is created, the space required by the storage of the names within the scope are allocated in the pool. When the owner is deleted, the space allocated for the owner is released and all names introduced in the owner scope are gone.

We have following the ownership transfer rules:

**Local-to-Owner Transfer Rule**: The name in owner scope will take the ownership from the name in local scope.

Consider:
```altscript
class test
{
    s: (string; string);                  // 's' is name of string pair in object scope
    func foo ()
    {
        local_s := ("Hello", "World");   // 'local_s' is a name in block scope and has a reference to
                                         // the tuple value ("Hello", "World"), and owns value.
        s = local_s;                     // 's' gets the reference to ("Hello", "World") and becomes the
                                         // owner. 'local_s' still refers to ("Hello", "World") but loses
                                         // the ownership and becomes a renter.
    }                                    // The block scope is over and 'local_s' is gone. The value
                                         // ("Hello", "World") is not released because 'local_s' is not
                                         // the owner.
} 
```
The name `s` in the object scope takes the ownership the value previously owned by a local name `local_s` in the assignment `s = local_s`. What if the local name does not own the value? For instance:
```altscript
class test
{
    s1 := ("Hello", "World");            // a name in object scope that owns ("Hello", "World")
    s2: (string; string);                // another name in object scope
    func foo ()
    {
        local_s := s1;                   // A local name refers to but does not own the value owned by 's1'
        s2 = local_s;                    // 's2' gets the reference to ("Hello", "World") from the local name
                                         // but does not get the ownership becuase the local name does not
                                         // have the ownership. 's1' is still the owner.
    }
} 
```
If the local name does not have the ownership, the name in a full scope will not become the owner. This sometimes may cause the lost of a value owned by a local name: 
```altscript
class test
{
    s: (string; string);                  // 's' is name of string pair in object scope
    func foo ()
    {
        local_s1 := ("Hello", "World");  // 'local_s1' owns ("Hello", "World")
        local_s2 := local_s1;            // 'local_s2' refers to but does not own ("Hello", "World")
        s = local_s2;                    // 's' gets the reference to ("Hello", "World") from 'local_s2'
                                         // but does not get the ownership
    }                                    // The block scope is over. The value ("Hello", "World") owned
                                         // by 'local_s1' is gone, and 's' refers to a null value again
    ctor()
    {
        foo();                           // Call 'foo'
        first_string := s[0];            // Error: Index on empty container
    }
} 
```
To prevent this from happening, we should avoid to pass the reference from a local rentor to a name in the owner space. In this case, we should use `local_s1` rather than `local_s2`. There is, however, one special case we need to consider. If an owner takes the reference t0 a value that it currently owns, it's ownership will not be changed. Consider:
```altscript
class test
{
    s := ("Hello", "World");            // a name in object scope that owns ("Hello", "World")
    func foo ()
    {
        local_s := s;                   // A local name refers to but does not own the value owned by 's1'
        s = local_s;                    // 's' gets the same reference and it's ownership is not changed.
    }
} 
```

**Local Scope Transfer Rule**: The name in an outer local scope will take the ownership from the name in its inner local scope.

Consider:
```altscript
class test
{
    ctor ()
    {
        s1: (string; string);            // 's1' is a name in the outer block scope
        {
            s2 := ("Hello", "World");    // 's2' is an another name in the inner block scope
            s1 = s2;                     // 's1' gets the reference and takes the ownership from 's2'
        }                                // The inner block scope ends, and 's2' is gone but the value
                                         // ("Hello", "World") is now owned by 's1'
    }
} 
```

**Output Transfer Rule**: If the output is composite value, the output in an input/output scope will take the ownership from the name in the local scope and the ownership in the output is always taken by the name that takes the output.

Consider:
```altscript
class test
{
    func foo(): (string; string)
    {
        local_s := ("Hello", "World");  // 'local_s' is a name in block scope that owns ("Hello", "World")
        return local_s;                 // The onwership of 'local_s' is transferred to the output
    }                                   // theblock scope is over, and 'local_s' is gone but the value
                                        // ("Hello", "World") is now owned by the output
    s := foo();                         // 's', a name in object scope, takes the ownership of the value
                                        // ("Hello", "World") in the output of the function 'foo'
} 
```

**Capture Transfer Rule**: If the output is a [free functor](FreeFunctor.md), all captured references in the local scope will take the ownership of their source.

Consider an example:
```altscript
func bar(): fn():(string; string)    // a function returns a free functor 'fn():(string; string)'
{
    local_s := ("Hello", "World");   // 'local_s' is a name in block scope that owns ("Hello", "World")
    return { local_s };              // the functor 'fn():(string; string) {local_s}' captured 'local_s'
                                     // by reference, and on return, the captured reference takes the
                                     // ownership from 'local_s'
}                                    // 'local_s' is out of its scope and goes away, and the captured
                                     // reference in the returned functor now owns the value ("Hello", "World")
s:=bar()();                          // 's', a name in object scope, takes the output from the functor
                                     // returned by calling 'bar', and 's' also takes the ownership of
                                     // ("Hello", "World") according to the 'Output Transfer Rule'
```
where the the free functor returned by the function `bar` is `fn():(string; string) {local_s}` uses the captured reference of a local variable `local_s`. On return, the captured reference takes the ownership from local variable 'local_s'.

### Ownership Copy Rules

**Constant Copy Rule**: If a variable name takes a reference from a constant, the variable name will take a reference to a copy of the composite value and own the copy.

A very simple example is:
```altscript
s := ("Hello", "World");
```
where the literal expression `("Hello", "World")` is a constant and the variable `s` has to take a copy of the constant in order to change  the value:
```altscript
s[0] = "你好";
s[1] = "世界";
```
so the change will not affect the original  constant value.

Consider another example:
```altscript
const s1 := ("Hello", "World");
s2 := s1;
```
The constant name `s1` takes the reference of the constant `("Hello", "World")` and did not create a copy of the constant. But `s1` does not own the value, that is, if `s1` is gone, the constant ("Hello", "World") is still valid because all literal constants live in an owner scope (object or class scope) in which they appear. When the variable `s2` takes the reference from `s1`, it actually  takes a new reference to a copy of the constant value ("Hello", "World") and owns the copy.

**Convert Copy Rule**: If a variable name takes a reference to a composite value that is in a different but [convertible type](ConvertableType.md), the variable name will take a reference to a copy of the converted composite value and own the copy.

Example:
```altscript
s1 := ("Hello", "World");
s2: string... = s1;
```
The name `s2` is declared as a string stream that can hold a variable number of strings. The name `s1` is inferred as a string pair type (a tuple with two string elements) and owns the string pair value ("Hello", "World"). When we assign `s1` to `s2`, the value ("Hello", "World") must be converted to a string stream value, and this conversion will create a copy in the string stream format: `string...("Hello", "World")`.

## Ownership Rules for Objects

An Altscript application is started along with an automatically created root object. Other objects are always created as child objects through their parents - either by explicitly calling the class [constructor](Constructor.md), or by loading from files. thus, an application consists of an object tree.

Let's say we have an object file `test.alt`, and we also have a class file `ExternalClass.alt` stored in the folder `class`, which is a subfolder in the same place of the file `test.alt`:
```
📃test.alt
📂class
   📃ExternalClass.alt
```
And the object file `test.alt` has the following contents:
```altscript
object test
{
    type AnExternalClass = ExternalClass at "%/class";  // defines a alias name for the external class
    object n : AnExternalClass                          // creates a child object of ExternalClass in `test`
                                                        // using inheritance. The singleton name `n` refers
                                                        // the child object
    {
        // more stuff about this object here            // we can introduce more stuff in the singleton 
    }
    vn := AnExternalClass();                            // creates a child object of ExternalClass in `test`
                                                        // using the constructor. The variable name `vn`
                                                        // refers to the created object
}
```
A command
```
alt test
```
will start an application to create a child object `test` in the application root. In the `test` object, a singleton object of the class ExternalClass is created as a child object of `test`. And the child object is referred to by the singleton name `n`. Note that the singleton child is created by using inheritance so that we can add more member stuff within the singleton. Again in the `test` object, another child object is created in `test`, using the constructor of the class ExternalClass, and we use a variable name to refer to the child object.

A name that refers to an object can either be a variable name or a singleton name. A singleton name is constantly bound to an object in its lifetime, and they live and die together. A singleton name can only be declared in an object scope or a class scope. A variable name, if it is not a const name, can refere to different objects at different time, and it can be declared in any scope, including an input/output scope or a block scope. However, the name does not own the referred object. The owner of an object is always its parent with the following object ownership rules:

* Each object, except for the root, has a parent object that owns the object.
* Each object can only have one parent object as its owner at a time.
* When the parent object goes out of life, all child objects go out of life too.
* The root object has no owner but it is created and destroyed automatically along with the application. The root object has the same life cycle of the application process.
