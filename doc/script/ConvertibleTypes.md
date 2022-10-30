# Convertible Types

[Type conversion or typecasting](https://en.wikibooks.org/wiki/Computer_Programming/Type_conversion) refers to changing the value of one data type into another. **Convertible type rules** determine if the value of one type can be converted to the value of another type.

## Numeric Conversion

[Numeric types](TypeNumeric.md) such as integers and floating numbers are convertible to each other. Numeric type conversion can be classified into widening type conversion and narrowing type conversion.

### Widening Conversion

**Widening Conversion** converts a smaller numeric type to a larger or the same type size without data loss. Or in other words, converting a value of T1 to a value of T2 is a widening conversion if we can convert the value of T2 back to the original value of T1. For example, converting a 4-byte integer to a 8-byte integer is a widening conversion because a 8-byte integer is able to hold any value of a 4-byte integer.

Altro performs widening conversion automatically. The following automatic widening conversions are supported:
```
tiny  ⟶ short  ⟶ int  ⟶ long  ⟶ llong  ──┐
 ⥮         ⥮        ⥮       ⥮        ⥮      ├──► float ⟶ double ⟶ ldouble
utiny ⟶ ushort ⟶ uint ⟶ ulong ⟶ ullong ──┘
```
In the above chart, for example, the value of a tiny or utiny can be automatically converted to any other primitive numeric type. Note that the conversion between signed and unsigned may cause sign change, but the bitwise data is retained because the value of the original type can be recovered when we convert the value back. For example,
```altro
  x: short = -2;
  y: ushort = x;  // after converting from ushort to short, y gets value 65534
  z: short = y;   // after converting back from ushort to short, z gets value -2
```

### Narrowing Conversion

On the other hand, **Narrowing Conversion** is a typecasting that may cause data loss and the converted value may not be able to be converted back to the original value. For example, converting a 8-byte integer to a 4-byte integer is a narrowing conversion.
 
Narrowing conversions must be explicitly stated, otherwise, Altro parser will report an error about potential data loss. Explicit type cast can be stated by the keyword `cast`, for instance,
```altro
i: int = 65537;
j: short = cast i;
```
Here the 4-byte integer value `65537` is forced to be converted into a 2-byte integer and the value in j is 1, which is a result of data loss. Without the keyword `cast`, an error will be reported.

Converting a floating-point number to an integer is always a narrowing conversion because the fractional portion of the floating-point number is discarded and lost during the conversion. Explicit cast must be used for this conversion:
```altro
x = 2.14;
y: int = cast x;   // y gets value 2
```

### Type Promotion

**Type promotion** is an automatic widening conversion when a binary operation is used on two operands. For instance, in the expression
```altro
4 + 4.2
```
The owner (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. On the other hand,
```altro
4.2 + 4
```
The input (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. For type promotion rules, see [Type Promotion](TypePromotion.md).

## Charater and String Conversion

The character type `char` (32-bit unicode) and `utf8` (UTF-8 encoded char) are interchangeable:
```altro
c1: utf8 = 'A';
c2: char = c1;
```
A character cannot be converted to an integer. The method `code` must be called in ordder to get its integral code:
```altro
c: char = 'A';
A_code : uint = 'A'.code();  // A_code gets value 65
```
Because `char` and `utf8` are interchangeable, a string and a stream of `char` are also interchangeable:
```altro
ss: char... = "Hello 世界";
s: string = ss;
```
Here `ss` is a stream that contains eight 32-bit unicode characters which is initialialized by the value converted from an UTF-8 string "Hello 世界". And `s` is a string that gets the initial value converted from a stream of characters contained in `ss`.

## Tuple Conversion

