# Sealed Class

A sealed class is a class that can not be inherited, but sealed classes are not necessarily [leaf classes](https://en.wikipedia.org/wiki/Leaf_class_(computer_programming)). They still can be used to derive subclasses by

* adding more [functors](Functor.md) using separated class forms, or
* parametric substitution

Because a sealed class can not be inherited, it has the following restrictions:

* All member functions in a sealed class are final, that is, a derived class from the sealed cannot override any member function defined in its base.
* A derived class from a sealed class cannot define its own constructors. it uses the constructors defined in its base.
* A derived class from a sealed class cannot define its own destructor.
* A derived class from a sealed class cannot have more class members.

A sealed class is declared by using the keyword `sealed`. A [value class](ValueClass.md) is sealed no matter if the keyword `sealed` is provided or not.

```altro
sealed class class_name
{
    // class members
}
```
Many primitive classes are sealed, including all concrete [numeric types](TypeNumeric.md) such as `int` and `double`,  [character type](Character.md], all concrete [enumeration types](TypeEnumeration.md] derived by `enum`,  all concrete [enumeration set types](TypeEnumeration.md] derived by  `enumset`, [iterator type](TypeIterator.md], [reference type](TypeReference.md], all concrete composite data derived from [tuple](TypeTuple.md), [stream](TypeStream.md), and [array](TypeArray.md). You cannot use any of these types as a base type in a class declaration, thus the following declaration is ill-formed:
```altro
class my_int is int  // Error:  Sealed class cannot be inherited: int
{
}
```

## Deriving Subclasses from a Sealed Classes With More Functors

You can have a subclass of a sealed class with additional [functors](Functor.md) using [separated function forms](SeparatedFunctionForm.md). Each function form add one more functor in the derived class:
```altro
sealed_class_base derived_class (input1) : output1
{
   // function body block here
}
sealed_class_base derived_class (input2) : output2
{
   // function body block here
}
```
The declaration of the `derived_class` by two function forms is conceptually equivalent to the following:
```altro
// Note, the following code is not allowed because a sealed class cannot be inherited.
// This is for conceptual illustration only
class derived_class is sealed_class_base
{
    fn (input1) : output1 { /*function body block here*/ }
    fn (input2) : output2 { /*function body block here*/ } 
}
```

## Deriving Subclasses from a Sealed Classes With Parametric Substitution

A sealed class can be an abstract class only in terms of parametricism because a sealed class cannot define any [deferred member function interface](Inheritance.md). Subclasses of a parametric sealed calles can only be derived through parametric substitution. Consider a sealed parametric class `Point`:
```altro
sealed class Point #(type T: real)
{
    x, y: T;
    ctor (x_, y_: T) { x=x_; y=y_ }
    func + (other: ownerclass): ownerclass { Point(x+other.x, y+other.y) }
}
```
You can derive subclasses from `Point` using parametric substitution:
```altro
type PointFloat = Point#(float);
type PointDouble = Point#(double);
```
But deriving subclasses from `Point` using inheritance is not allowed:
```altro
class Point3D #(type T: real) is  Point#(T) // Error:  Sealed class cannot be inherited: Point
{
    z: T;
}
```

## See also:
* [Parametric Class](ParametricClass.md)
* [Value Class](ValueClass.md)
* [Function Class and Functions](FunctionClass.md)
* [Inheritance](Inheritance.md)
