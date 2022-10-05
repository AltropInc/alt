# Incorporeal Class

An incorporeal class is a data class that has no

* data members in [instance scope](Scopes.md),
* constructor in [instance scope](Scopes.md), and
* destructor in [instance scope](Scopes.md)

However, the class can have

* meta members
* functors
* member functions

There is no keyword reserved for incorporeal classes as long as they are not inherited from the `object` class and they meet the decription above. The `object` class cannot be incorporeal because instances (objects) of an object class reside in an object tree with member references for its parent and children nodes.

Since an incorporeal class class has no data members, it has no physical storage (storage size is zero). References to instances of incorporeal classes always point to the same special place in the memory.

Calling member functions or functors in an incorporeal class can be done without the need of an instance of the class. Instead, you call the functor directly using the class name. For instance,
```altro
sealed class Incorporeal: any
{
    meta factor := 4;   // an incorporeal call can have meta members
    func getFactor (): int { factor }   // an incorporeal call can have member functions
};
f := Incorporeal.getFactor();   // call member function `getFactor' using the class directly
```
An incorporeal class is often used to define functors so that calling these functors defined in the class (function call) can be done without the need of an instance of the class. Instead, you call the functor directly using the class name. See [Function Call Using Class Name](Functor.md).

The class `func` is a built-in functional class which is incorporeal and implements a calling protocol using an efficient and sequential procedure based on the call stack technique for a traditional function call. Using the `func` class, we can derive subclasses in terms of functions:
```altro
func sum(values: int...): int
{
    sum := 0;
    foreach (v in values) sum += v;
    sum
}
```
where `sum is a subclass derived from `func` with a functor in the interface `(int...):int`. Calling the functor defined in `sum` can use the class `sum` directly without instantiation becuase `sum` is an incorporeal class:
```altro
sum(1,2,3,4)
```
For more information about functions, see [Function Class and Functions](FunctionClass.md)

## See also:

* [Scope](Scopes.md)
* [Function Call Using Class Name](Functor.md)
* [Function Class and Functions](FunctionClass.md)

