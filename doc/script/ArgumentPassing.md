# Argument Passing

Passing instances among arguments can either be **pass-by-value** or **pass-by-reference**. 

All instances of [value types](ValueClass.md) such as int, char, enum etc are passed by value. That is, a copy (or converted copy) of the instance (the value) is passed from one argument to another. As result, the two arguments hold distinct copies of the value.
```
Illustration of pass-by-value
――――――――――――――――――――――――――――――
a ──owns─🡪 value(1,2,3,4)                     a ──owns─🡪 value(1,2,3,4)
b := a                                         b ──owns─🡪 value(1,2,3,4)
```
Because `a` and `b` own or hold their own value, Changing value through one argument will not affect the value held by the other argument. Therefore, when a caller passes a value instance into an input interface during a function or constructor call, the change made within the call will not be reflected outside of the caller.

All instances of non-value classes are passed by reference, which means that values are not copied when they are passed from one argument to another, instead, only the reference to the value is passed, and the [ownership](Ownership.md) is not passed either. 
```
Illustration of pass-by-reference
――――――――――――――――――――――――――――――――――
a ──owns─🡪 tuple(1,2,3,4)                     a ──owns────┐
                                                           ├───🡪 tuple(1,2,3,4)
b := a                                         b ──────────┘
```
Because `a` and `b` hold the same reference to the same value, a modification to an instance of a non-value class through one argument will be sensed by the other argument that has the same reference. Therefore, when a caller passes a non-value instance into an input interface during a function or constructor call, the change made within the call will be visible to the caller. Because `b` does not own the tuple value, the tuple value is released only when `a` goes out of scope.

Pass-by-value accepts values of [subtypes](Subtypes.md] and [convertible types](ConvertibleTypes.md]. For instance, you can pass an integer value to a double
precision parameter because an integer value can be converted to double precision. You can also pass an integer to a parameter in declared type `numeric`, because integer is a subtype of `numeric`.

Pass-by-reference only accepts references of [subtypes](Subtypes.md]. If any conversion is required, a copy of the converted value will be created and this is equivalent to pass-by-value. For instance, a tuple of three integers is a subtype of a tuple of two integers, thus an assignment of a value of three-integer tuple to a two-integer tuple argument is a pass by reference:
```
a ──owns─🡪 tuple(1,2,3)                        a ───owns───┐
                                                            ├───🡪 tuple(1,2,3)
b: (int; int) = a                               b ──────────┘
```
A tuple of three integers is a subtype of an integer stream, but can be converted to an integer stream,  thus an assignment of a value of three-integer tuple to an integer stream argument is a pass by value converted:
```
a ──owns─🡪 tuple(1,2,3,4)                      a ───owns───🡪 tuple(1,2,3,4)
b : int... = a                                  b ───owns───🡪 int...(1,2,3,4)
```

Pass-by-reference has another exception. If argument `a` is constant but argument `b` is not, passing value from `a` to `b` will be a pass by value, not by reference:
```
const a ──owns─🡪 tuple(1,2,3,4)                a ───owns───🡪 tuple(1,2,3,4)
b := a                                          b ───owns───🡪 tuple(1,2,3,4)
```

## See also:
* [Ownership](Ownership.md)
* [Assignment](Assignment.md)
* [Name Declaration](NameDeclaration.md)
* [Value Class](ValueClass.md)
* [Reference Type](TypeReference.md)
* [Scope](Scopes.md)
* [Convertible Type](ConvertibleTypes.md)
* [Subtypes](Subtypes.md)

