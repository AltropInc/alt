# Incorporeal Class

An incorporeal class is a non-value data class that is not inherited from the class `object` and has no

* data members in [instance scope](Scopes.md),
* constructor in [instance scope](Scopes.md), and
* destructor in [instance scope](Scopes.md)

However, the class can have

* meta members
* member functions
* functors

There is no keyword reserved for incorporeal classes as long as they are not value classes, they are not inherited from the `object` class, and they meet the description above. A [value class](ValueClass.md) cannot be incorporeal because it cannot have empty data. The `object` class cannot be incorporeal because instances (objects) of an object class reside in an object tree with member references for its parent and children nodes.

Since an incorporeal class has no data members, it has no physical storage (storage size is zero). References to instances of incorporeal classes always point to the same special place in the memory.

Calling member functions in an incorporeal class can be done without the need of an instance of the class. Instead, you call the functor directly using the class name. For instance,
```altro
class math
{
    meta const := 3.14159265358979323846;                   // a meta constant member
    func sin (x: double): double { /* return sin of x */ }  // a member function
};
sx := math.sin(1.5);   // call member function `sin' using the class directly
```
An incorporeal class is also used to define functors so that calling these functors defined in the class (function call) can be done without the need of an instance of the class. Instead, you call the functor directly using the class name. See [Function Call Using Class Name](Functor.md).

The class `func` is a built-in functional class which is incorporeal and implements a calling protocol using an efficient and sequential procedure based on the call stack technique for a traditional function call. Using the `func` class, we can derive subclasses in terms of functions:
```altro
func sum(values: int...): int
{
    sum := 0;
    foreach (v in values) sum += v;
    sum
}
```
where `sum is a subclass derived from `func` with a functor in the interface `(int...):int`. Calling the functor defined in `sum` can use the class `sum` directly without instantiation because `sum` is an incorporeal class:
```altro
sum(1,2,3,4)
```
For more information about functions, see [Function Class and Functions](FunctionClass.md).

Note that a member function is an incorporeal class too. Theoretically, an incorporeal class could have any incorporeal member classes, not just limited to member functions. However, there is little use of incorporeal member classes within an enclosing incorporeal class and allowing member classes in a incorporeal class could easily break the property of the enclosing incorporeal class. That is why only member functions, not any incorporeal member classes, are allowed in an incorporeal  class.

## See also:

* [Scope](Scopes.md)
* [Value Class](ValueClass.md)
* [Types](Types.md)
* [Function Call Using Class Name](Functor.md)
* [Function Class and Functions](FunctionClass.md)

