# Value Class

A value class declaration introduces a class whose instances are value objects. It is a mechanism to avoid allocating objects/values in heap. These primitive types are value classes: all concrete [numeric types](TypeNumeric.md), [character type](Character.md], [enumeration type](TypeEnumeration.md], [enumeration set type](TypeEnumerationSet.md], [iterator type](TypeIterator.md], [reference type](TypeReference.md] and value tuple(TypeTuple.md).

User defined value classes are accomplished through the definition of class prefixed with the keyword "value":
```altro
value class point
{
    x, y, z: double;
}
```
A value class is a sealed class and cannot be used to derive subclasses with more [member name declaration](NameDeclaration.md).
