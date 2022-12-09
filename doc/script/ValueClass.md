# Value Class and Non-Value Class

A **value class** declaration introduces a type whose instances are values that are passed among arguments by copying ([pass-by-value](ArgumentPassing.md)). A **non-value class** declaration introduces a type whose instances are values or objects passed among arguments by reference ([pass-by-reference](ArgumentPassing.md)).

These primitive types are value classes: all concrete [numeric types](TypeNumeric.md), [character type](Character.md], [enumeration type](TypeEnumeration.md], [enumeration set type](TypeEnumerationSet.md], [iterator type](TypeIterator.md], [reference type](TypeReference.md] and value tuple(TypeTuple.md).

User defined value classes are accomplished through the definition of a class or a tuple prefixed with the keyword "value". Here is a value tuple:
```altro
value tuple point (x, y: double);
```
or, simply,
```altro
value point (x, y: double);
```
And here is an example of parametric value class:
```altro
value class complex #(type T: real) : numeric
{
    a, b: T;
    ctor (a_, b_: T) { a=a_; b=b_ }
    final func <=> (other: ownerclass): int { a<=>other.a }
    final func + (other: ownerclass): ownerclass { complex(a+other.a, b+other.b) }
    final func - (other: ownerclass): ownerclass { complex(a-other.a, b-other.b) }
    final func * (other: ownerclass): ownerclass { complex(a*other.a-b*other.b, a*other.a+b*other.b) }
    final func / (other: ownerclass): ownerclass { c:=other.a*other.a+other.b*other.b; complex((a*other.a+b*other.b)/c, (b*other.a-a*other.b)/c) }
}
```
A value class can only be derived from an [interface class](InterfaceClass.md) or a [stateless class](StatelessClass.md).

A value class is a [sealed class](SealedClass.md) and as a sealed class, it can only be used to derive subclasses with the following restrictions:

* All member functions in a value class are final, that is, a derived class cannot [override](Inheritance.md) any member function defined in its base.
* A derived class from a value class cannot have its own constructors. it uses the constructors defined in its base.
* A derived class from a value class cannot have its own destructor.
* A derived class from a value class cannot have more data members.

## See also:
* [Argument Passing](ArgumentPassing.md)
* [Interface Class](InterfaceClass.md)
* [Sealed Class](SealedClass.md)
* [Stateless Class](StatelessClass.md)
* [Class Members](ClassMember.md)
* [Overriding](Inheritance.md)
