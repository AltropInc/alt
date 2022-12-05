# Abstract Class

An abstract class is a class that cannot be instantiated, but they can be subclassed. A class is abstract if it is declared abstract using the specifier `abstract` or it is a [parametric class](ParametricClass.md) with type parameters unbound. An abstract class can contain deferred member function interfaces or deferred meta constants:
```altro
abstract class Base
{
    deferred func foo();  // a deferred member function interface
    deferred meta const length: int; // a deferred meta constant
}
```
Deferred items can only be declared in an abstract class.

If a derived class inherits an abstract interface with any deferred items, the derived class must provide implementation to any deferred member function interface, and  provide initialization value to any deferred meta constants unless the derived class is abstract:
```altro
class Derived is Base
{
    func foo() { /* do something */ }  // the implementation to the deferred member function interface
    length = 10;                       // the initialization value to the deferred meta constant
}
abstract class AbstractDerived is Base
{
    // Okay, this derived class is abstract 
}
class IncompleteDerived is Base
{
    // Error: the deferred const name 'length' is not initialized in IncompleteDerived
    // Error: the deferred interface 'foo' is not implemented in IncompleteDerived
}
```

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
