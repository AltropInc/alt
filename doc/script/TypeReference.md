# Reference Type

A **reference** in a reference type refers to an object or value in some memory space. The reference type is a built-in class defined as following:
```altro
class ref#(type referred_type: any);
```
In a name declaration,
```altro
x: int = 3;
```
`x` is declared as a name (a storage in some memory space) to hold a value in the type of `int`. Sometimes when we say that `x` is in the type of `int`, we really mean that the content in `x` is in the type of `int`. The name `x` itself is actually in the type of ref#(int). When an L-value or name expression (the memory location to store the value) is required, for instance, at the left side of an assignment, the expression `x` is evaluated to a reference in the type of `ref#(int)`, which is the location where the integer value is stored.  When an R-value or value expression (the value stored in the name) is required, for instance, at the right side of an assignment, the expression `x` is evaluated to an integer value stored in `x`, which is in the type of `int`.

Therefore, in most cases, reference types are not explicitly used. In fact, a reference type cannot be used as a declared type for an argument. For instances,
```altro
x := 3;
y : ref#(int) = x;  // Error: reference type cannot be explicitly used in name declaration
```
Here `y` provides independent storage to hold the storage location of `x`. If this is allowed, the reference held by `y` might become a [dangling reference](https://en.wikipedia.org/wiki/Dangling_pointer) when `y` outlives `x`. Consider the following
```altro
x := (1,2,3,4);
y : ref#(int) = x[3];  // Error: reference type cannot be explicitly used in name declaration
x = (5,6);  // the value (1,2,3,4) is destroyed before 'x' takes the new value (5,6).
```
If we allowed `y` to take the reference to the third element of `x`, `y` would have became a dangling reference after `x` takes a new value.

A reference type can only be used for the output type in a function interface. When a function returns a reference, it returns an implicit reference in its return value. This way, a function can be used on the left side of an assignment or wherever an L-value or name expression is required. For instance:
```altro
class SizeList
{
    size_list: int...;
    ctor (inti_val: int...) { size_list = inti_val }
    func [ (index: int) ] : ret#(int) { index < size_list.length() ? size_list[index] : null }
}
```
The function `[]` returns a reference type and then can be used at the left side of an assignment:
```altro
sl := SizeList(1,2,3,4,5);
s1[2] = s[3];   // assign the value stored in sl[3] to the location indicated by sl[2]
```
Note that a function can only return a reference in object/class scope. Returning a reference in block or I/O scope is considered an error.

