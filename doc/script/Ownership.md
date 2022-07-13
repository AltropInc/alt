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
