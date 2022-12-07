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
 
Narrowing conversions must be explicitly stated, otherwise, Altro parser will report an error about potential data loss. Explicit [type cast](TypeCast.md) can be stated by [cast expression](Expressions.md), for instance,
```altro
i: int = 65537;
j: short = cast(short, i);
```
Here the 4-byte integer value `65537` is forced to be converted into a 2-byte integer and the value in j is 1, which is a result of data loss. Without `cast`, an error will be reported.

Converting a floating-point number to an integer is always a narrowing conversion because the fractional portion of the floating-point number is discarded and lost during the conversion. Explicit cast must be used for this conversion:
```altro
x = 2.14;
y: int = cast(int, x);   // y gets value 2
```

### Type Promotion

**Type promotion** is an automatic widening conversion when a binary operation is used on two operands and one of the operand will be promoted (widened) to macth the other. For instance, in the expression
```altro
4 + 4.2
```
The owner (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. On the other hand,
```altro
4.2 + 4
```
The input (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. For type promotion rules, see [Type Promotion](TypePromotion.md).

Type promotion can also happen when a prefix operation is used on an operand (the owner that performs the operator) and the operand will be promoted to the type required by the operator. For example, in the prefix expression `∛9`, the operand is an integer `9`, which is promoted to a value of double `9.0` before the operator `∛` is applied.

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

## Tuple, Array, and Stream Conversion

Values of tuple, array and stream can be converted to each other under certain conditions.

### Tuple to Tuple Conversion

A value of tuple type T1 can be converted to another tuple type T2 if the number of elements contained in T1 is equal or greater than the number of elements contained in T2, and each tuple element in T1, if used in conversion,  is convertible to the type of the corresponding element in T2. For instance:
```altro
t1: (int; string) = (1, "text");
t2: (double; char[]) = t1;
```
In the above conversion, the integer value of the first element in t1 is convereted the double value for the first element in T2; and string value of the second element in t1 is convereted the array of char for the second element in T2. Note that `char[]` is an abstarct array type, and the actual array type convereted is `char[4]`.

If element conversion involves narrowing conversion, explicit type cast must be used:
```altro
t1: (double; string) = (1, "text");
t2: (int; char[]) = cast((int; char[]), t1);
```
### Tuple to Array Conversion

A tuple value can be converted to a array if the number of elements contained in the tuple value is equal or greater than the length of the array, and each tuple element, if used in conversion,  is convertible to the element type of the array. For instance:
```altro
t: (int; int; string) = (1, 2, "text");
a: int[2] = t;
```
If element conversion involves narrowing conversion, explicit type cast must be used:
```altro
t: (int; int; string) = (1, 2, "text");
a: short[2] = cast(short[2], t);
```
