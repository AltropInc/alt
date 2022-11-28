# Literals

[Literals](https://en.wikipedia.org/wiki/Literal_(computer_programming)) represent the possible fixed values in primitive types of Altro. In contrast to literals, [names](Names.md) are variable or constant symbols that can take on one of fixed values of a type, and the constant is constrained not to change after it takes the initial value. Altro literals are integers, floating point, enumerated values, enumerated set values, characters, strings, and compound values in tuples. Each of these are discussed in sections below.

## Integer Literal

A literal of integer type is know as the integer literal. It can be decimal, octal, binary, or hexadecimal constant. A suffix can be used with integer literals to indicate the type of the integer literal. By default, every integer literal without suffix is of `int` type. A plus (+) or minus (-) symbol can precede an integer literal. However, it is not part of the literal, rather, it is interpreted as an unary operator.

The following suffixes can be used with integert literal to indicate a specific floating-point type:

| suffix | example  | type    | comments                        |
|:------ |:-------- |:------- |-------------------------------- |
| t      | 25t      | tiny    | 8-bit signed integer            |
| s      | 355s     | short   | 16-bit signed integer           |
| l      | 355l     | long    | 64-bit signed integer           |
| ll     | 355ll    | llong   | 128-bit signed integer          |
| ut     | 25ut     | utiny   | 8-bit unsigned integer          |
| us     | 355us    | ushort  | 16-bit unsigned integer         |
| u      | 355u     | uint    | 32-bit unsigned integer         |
| ul     | 355ul    | ulong   | 64-bit unsigned signed integer  |
| ull    | 355ull   | ullong  | 128-bit unsigned signed integer |

In the form of decimal literals (Base 10), the allowed digits are 0-9, and the starting digit should be non-zero:
```altro
0       // integer 0 in the type of int
45s     // integer 45 in the type of short
3245    // integer 3245 in the type of int
102u    // unsigned integer 102 in the type of uint
3245l   // integer 3245 in the type of long
3245ull // integer 3245 in the type of ullong
```
In the form of octal literals (Base 8), the allowed digits are 0-7, and the starting digit should always be zero:
```altro
010     // integer 8 in the type of int
012s    // integer 10 in the type of short
0100u   // unsigned integer 64 in the type of uint
07778   // Error: Invalid octal digit 
```
In the form of binary literals (Base 2), the allowed digits are only 1’s and 0’s, and the starting characters are `0b`:
```altro
0b101     // integer 5 in the type of int
```
In the form of hexadecimal literals (Base 16), the allowed digits are 0-9, characters are a-f or A-F, and the starting characters are `0x` or `0X` (characters are case insensitive):
```altro
0x8FC3     // integer 36803 in the type of int
0X8fC3     // integer 36803 in the type of int
```

## Floating-Point Literal

A floating-point literal has an integer part, a decimal point, a fractional part, and an exponent part.  The following suffixes can be used with floating-point literal to indicate a specific floating-point type:
| suffix | example  | type    | comments                                                  |
|:------ |:-------- |:------- |---------------------------------------------------------- |
| h      | 0.25h    | hfloat  | 16-bit half precision floating-pointer number             |
| f      | 0.25f    | float   | 32-bit single precision floating-pointer number           |
| ld     | 0.25ld   | ldouble | 128-bit extended double precision floating-pointer number |

By default, every floating-point literal without suffix is of `double` type (64-bit double precision floating-pointer number).

A plus (+) or minus (-) symbol can precede a floating-point literal. However, it is not part of the literal, rather, interpreted as an unary operator.

A floating-point literal starts with digit 0 to 9 or with a decimal point. The integer part can be any form of an integer literal without any suffix. If the integer part is missing, the floating-point literal starts with a decimal point and the base is 10. The fractional part is a sequence of digits and may also include a-f or A-F if the integer part has the base of 16. The integer part determines the base of the fractional part in floating-point literal. The exponent part starts with any of the four letters: e, p, E or P, and followed by a decimal integer possibly prefixed with a sign character. When hexadecimal base 16 is used, the exponent part must start with p or P because e and E are valid hexadecimal digits.

If exponential form is used, meaning that the value of the floating-point literal is the significand multiplied by the number of 10 raised to the power of exponent, or by the number of 2 raised to the power of exponent if the significand has the base 2, 8, or 16. For examples, the mathematical meaning of 1.23e14 is 123×10¹⁴, and 0x1.4p3 is 0x1.4×2³ (1.25×2³, or 10.0).

Here are some examples of floating-point literals:
```altro
3.14145    // Valid. The integer part is 3; the fractional part is 14145
.14145     // Valid. The integer part is empty; the fractional part is 14145
3.         // Valid. The integer part is 3; the fractional part is empty
3129E-5    // Valid. The integer part is 3129; the fractional part is empty; The exponent part is -5; the value is 3129x10⁻⁵
0x1f.4ep5  // Valid. The integer part is 0x1f; the fractional part is 4e; The exponent part is -8; the value is (31 + 78/256) x 2⁵
.e4        // Invalid, missing integer or fraction
```
Altro provides hardware support arithmethic operations on [half precision floating-pointer numbers](https://en.wikipedia.org/wiki/Half-precision_floating-point_format) only when the hardware has instructions for half precision floating-pointer numbers. Otherwise, Altro only suppot load and store operations and half precision floating-pointer numbers will be converted to single precision floating-pointer numbers before any arithmethic operation is done.


