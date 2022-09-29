# Value Class

A value class declaration introduces a type whose instances are values. Values are passed among arguments by copying ([pass-by-value](ArgumentPassing.md)). 

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
value class Complex #(type T: real) : numeric
{
    a, b: T;
    ctor (a_, b_: T) { a=a_; b=b_ }
    final func <=> (other: ownerclass): int { a<=>other.a }
    final func + (other: ownerclass): ownerclass { Complex(a+other.a, b+other.b) }
    final func - (other: ownerclass): ownerclass { Complex(a-other.a, b-other.b) }
    final func * (other: ownerclass): ownerclass { Complex(a*other.a-b*other.b, a*other.a+b*other.b) }
    final func / (other: ownerclass): ownerclass { c:=other.a*other.a+other.b*other.b; Complex((a*other.a+b*other.b)/c, (b*other.a-a*other.b)/c) }
}
```
A value class is a sealed class and cannot be used to derive subclasses with more [member name declaration](NameDeclaration.md).
