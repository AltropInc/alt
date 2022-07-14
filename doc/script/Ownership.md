# Ownership
In ALT script, instances of non-value classes are passed among [value names](Names.md) via references. They are also called [reference values](https://en.wikipedia.org/wiki/Reference_(computer_science)). [Strings](String.md), [streams](Stream.md), [arrays](Array.md) and [tuples](Tuple.md) are  reference values. Any object created from a non-value class is a reference value.

A name that holds a reference value does not have a physical storage for the entire data required by the value, rather, it holds a reference to a storage allocated in another place (a common value pool). Therefore, it is possible that multiple names hold the reference to the same reference value.

The Ownership is a set of rules that governs how these reference values are allocated and released in the common value pool . Each value name  exists in a [scope](Scopes.md), which is a region of a program text in which the declared name is valid. When the name goes away (the program control flow exits from the scope where the name is defined), the ownership rule determines whether the value referred by the name should be released in the common value pool.

## Ownership Rules for Composite Values

### The Owner Rules
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
However, if the non-owner changes its reference to refer to a different value, the owner and other non-owners will still refer to original value:
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
* transfer the ownership to another name when we pass the reference, and as a result, the name that takes the reference becomes the new owner and the original owner becomes a renter.
* copy the value so the reference passed to another name ia a new reference to a copy and the name that takes the new reference is the owner of copy.
Let’s explore these situations now.

### Ownership Transfer Rules

The ownership transfer rule governs how the ownership is transferred when we pass the reference to another name. Before discussing this, we need to cover the concept of [scope](Scopes.md) first. We have four kinds of scopes: object scope, class scope, input/output scope, and block scope.

The ownership transfer rule governs how the ownership is transferred when we pass the reference to another name. Before discussing this, we need to cover the concept of [scope](Scopes.md) first. We have four kinds of scopes: object scope, class scope, input/output scope, and block scope.

The input/output scope and block scope are local scopes, kind of temporary scopes created for a [routine](Routines.md) call. Names introduced in a local scope are stored in a [call stack](https://en.wikipedia.org/wiki/Call_stack). When the execution enters into a local scope, the space required by the storage of the names within the scope are pushed into the stack. When the execution exits from a local scope, the space pushed for the scope is popped out and all names introduced in the local scope are gone. The stack naturally maintains a nested structure so that local sopes can be nested inside one another.

The object scope or the class scope is an owner scope created for the lifetime of its associated object or class (owner). Names introduced in a full scope are stored in a space allocated in the common value pool for its associated owner. When the owner is created, the space required by the storage of the names within the scope are allocated in the pool. When the owner is deleted, the space allocated for the owner is released and all names introduced in the owner scope are gone.

We have following the ownership transfer rules:

**Transfer Rule 1**: The name in owner scope will take the ownership from the name in local scope.

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
To prevent this from happening, we should avoid to pass the reference from a local rentor to a name in the owner space. In this case, we should use `local_s1` rather than `local_s2`.

**Transfer Rule 2**: The name in an outer block scope will take the ownership from the name in its inner local scope.

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
