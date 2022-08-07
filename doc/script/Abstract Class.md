# Abstract Class

An abstract class is a class that cannot be instantiated, but they can be subclassed. A class is abstract if it

* contains deferred member functions or inherits such function that is not overridden with an implementation,
* is declared abstract using the specifier `abstract`, or
* is a [parametric class](Parametric Class.md) with type parameters unbound.

If a class contains a member function with any deferred interface, this class must be declared as an abstract or interface class:
```Altro
class Base
{
    deferred func foo(); // Error: Class contains deferred members must be declared as an abstract or interface class
}
```
If a class inherits any deferred interface but not implemented, this class is abstract too, but the error will be reported the first time when the class is used to create an instance:
```Altro
abstract class Base
{
    deferred func foo();
}
class Derived is Base
{
}
d := Derived(); // Error:  Object instantiation failed -- the deferred interface 'foo' is not implemented
```
The purpose of using lazy error detection here is to avoid checking inherited but unimplemented functions for every class, which may slow down the parsing speed in code loading.

An [interface class](InterfaceClass.md) is an abstract class because it can only have deferred member functions.  

A [parametric class](ParametricClass.md) with unbound type parameters is also an abstract class. This is a different concept than a parametric class provided in other programming languages such as generic classes in Java and C# and templates class in c++. Since a parametric class is a true type, it can be used as an abstract type for polymorphic variable declarations, wherein the type parameters may be left undetermined
```Altro
class Point #(type UnitT: numeric)
{
    x, y: UnitT;
    ctor(x, y: UnitT)
    {
       
    }
}
p: Point;
p = Point#(int)(3,4);
```
Here the class `Point` is an abstract class, and the variable name `p` is polymorphic and can be used to accept any instance of a concrete subclass derived from `Point`.
