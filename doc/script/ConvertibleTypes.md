# Convertible Types

[Type conversion or typecasting](https://en.wikibooks.org/wiki/Computer_Programming/Type_conversion) refers to changing the value of one data type into another. **Convertible type rules** determine if the value of one type can be converted to the value of another type.

**Widening Conversion** converts a smaller type to a larger or the same type size without data loss. Or in other words, converting a value of T1 to a value of T2 is a widening conversion if we can convert the value of T2 back to the original value of T1. For example, converting a 4-byte integer to a 8-byte integer is a widening conversion because a 8-byte integer is able to hold any value of a 4-byte integer. On the otherhand, **Narrowing  Conversion** is a typecasting that may cause data loss and the converted value cannot be converted back to the original value. For example, converting a 8-byte integer to a 4-byte integer is a narrowing conversion.

Altro performs widening conversion automatically. Narrowing conversions must be explicitly stated, otherwise, Altro wparser will report an error about potential data loss.


## Numeric Conversion

[Numeric types](TypeNumeric.md) such as integers and floating numbers are convertible to each other.  For instance, a `float` value can be converted to a `double` value or an `int` value, and an `int` value can be converted to a `float` value or an `double` value. The limited number of bits of a numeric type determines the range of possible values that the type can represent.  The important thing to remember about numeric conversion is that when converting a value of a numeric type to the value of another numeric type with less number of bits, data loss may result, and when signed is converted to unsigned, signs can be lost. For instance:

```altro
i: int = 65536;
s : short = i;   // data loss here because short integer can only represent a range [-32,768, 32,767]
j : int = -2;
u: uint = j;     // sign loss here because unsigned int cannot represent a negative value
```
The parser warns you about potential data/sign loss. Take these warnings very seriously. If you are certain that no data loss will occur because the values in the larger variable will always fit in the smaller variable, then add an explicit cast so that the parser will no longer issue a warning. If you aren't sure that the conversion is safe, add to your code some kind of runtime check to handle possible data loss so that it doesn't cause your program to produce incorrect results.

Any conversion from a floating point type to an integral type will cause data loss because the fractional portion of the floating point value is discarded and lost.
