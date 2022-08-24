# Numeric

Numeric classes representing numeric values that have the property of arithmetic calculation such as addition, subtraction, multiplication, and division. Built-in numeric interfaces can be further divided into two categories: integral and real. The integral class specifies [whole numbers](https://en.wikipedia.org/wiki/Integer) that can be represented without a fractional component. The real class represents [real number](https://en.wikipedia.org/wiki/Real_number) values that have a continuous quantity, but their bit-representation in platform is approximate due to the limited number of bytes used to store their values, and they are often referred as [floating point numbers](https://en.wikipedia.org/wiki/Floating-point_arithmetic).

## Numeric Interfaces

All concrete numeric classes such as int, short, long, double, float, etc. re derived from the interface class **numeric**.
```
                                                  nemric
                              ┌─────────────────────┴────────────┬──────── ─ ─ ─ ─ ┬ ─ ─ ─ ─ ─ ─ ┐
                          integral                              real           rational       complex
              ┌───────────────┴─────────────┐            ┌───────┼───────┐
          unsigned                        signed      float   double  ldouble 
  ┌─────┬─────┼─────┬─────┐      ┌─────┬────┼────┬────┐
utiny ushort uint ulong ullong  tiny short int long llong 
```

### numeric
The class **numeric** is an interface class that imposes arithmetic functions on a derived class that implements required capabilities for arithmetic numbers that are values of comparable, addable, and scalable. The class `numeric` inherits deferred comparable interfaces and adds the following deferred member functions for the above listed required capabilities:

```altro
interface class numeric is comparable         // inherits comparable
{
    func + (other: ownerclass): ownerclass;   // addable
    func - (other: ownerclass): ownerclass;   // addable
    func * (other: ownerclass): ownerclass;   // scalable
    func / (other: ownerclass): ownerclass;   // scalable
    func ÷ (other: ownerclass): ownerclass;   // scalable
    func % (other: ownerclass): ownerclass;   // scalable
    func ^ (other: ownerclass): ownerclass;   // scalable
    func += (other: ownerclass): ownerclass;  // addable
    func -= (other: ownerclass): ownerclass;  // addable
    func *= (other: ownerclass): ownerclass;  // scalable
    func /= (other: ownerclass): ownerclass;  // scalable
    func ÷= (other: ownerclass): ownerclass;  // scalable
    func %= (other: ownerclass): ownerclass;  // scalable
    func ^= (other: ownerclass): ownerclass;  // scalable
}
```
A class derived form the `numeric` interface must implement the deferred member functions as listed:

| func   | Alias    | Common Name      | Example   |   Description                                       |
|:------ |:-------- |:---------------- |:--------- |:--------------------------------------------------- |
| +      |          | Addition         | x + y     | Returns the sum of x and y                          |
| -      |          | Subtraction      | x - y     | Returns the difference by subtracting y from x      |
| *      |          | Multiplication   | x * y     | Returns the multiplies of x by the factor y         |
| /      |          | Division         | x / y     | Returns the quotient of dividing x by the divisor y |
| ÷      | /#       | Integer Division | x ÷ y     | Returns the integral part of the quotient from x/y  |
| %      |          | Modulus          | x % y     | Returns the integral division remainder             |
| ^      |          | Power            | x ^ y     | Returns the value of x raised to the power of y     |
| +=     |          | Add Assign       | x += y    | Adds y to x and x gets the sum                      |
| -=     |          | Sub Assignn      | x -= y    | Substrcts y from x and x gets the difference        |
| \*=    |          | Mul Assign       | x \*= y   | Multiplies x by y and x gets the product            |
| /=     |          | Div Assign       | x /= y    | Divides x by y and x gets the quotient              |
| ÷=     | /#=      | Int Div Assign   | x ÷= y    | Divides x by y and x gets the integral part of the quotient    |
| %=     |          | Mod Assign       | x %= y    | Divides x by y and x gets the remainder             |
| ^=     |          | Power Assign     | x ^= y    | Raised the value of x to the power of y             |

### integral
The class **integral** is an interface class derived from `numeric` for all integer types. Integers are values comparable, addable, scalable, incrementable, clusivity-operable, and bit-shiftable. The class `integral` is derived from `numeric` with the following additional deferred member functions for incrementable, clusivity-operable([bitwise operations](https://en.wikipedia.org/wiki/Bitwise_operation)), and bit-shiftable values:
```altro
interface class integral is numeric             // inherits comparable, addable, and scalable
{
    func ++ (): ownerclass;                     // incrementable
    func -- (): ownerclass;                     // incrementable
    prefix func ++ (): ownerclass;              // incrementable
    prefix func -- (): ownerclass;              // incrementable
    func \ (other: ownerclass): ownerclass;     // clusivity-operable
    func | (other: ownerclass): ownerclass;     // clusivity-operable
    func & (other: ownerclass): ownerclass;     // clusivity-operable
    prefix func ~ (): ownerclass;               // clusivity-operable
    func << (n: int): ownerclass;               // bit-shiftable
    func >> (n: int): ownerclass;               // bit-shiftable
    func \= (other: ownerclass): ownerclass;    // clusivity-operable
    func |= (other: ownerclass): ownerclass;    // clusivity-operable
    func &= (other: ownerclass): ownerclass;    // clusivity-operable
    func <<= (n: int): ownerclass;              // bit-shiftable
    func >>= (n: int): ownerclass;              // bit-shiftable
}
```
| func        | Alias    | Common Name      | Example   |   Description                                              |
|:----------- |:-------- |:---------------- |:--------- |:---------------------------------------------------------- |
| ++ (prefix) |          | Increment        | x++       | Increases x by 1 and returns the value before increasement |
| ++ (prefix) |          | Increment        | x++       | Decreases x by 1 and returns the value before decreasement |
| ++ (prefix) |          | Increment        | ++x       | Increases x by 1 and returns the increased value           |
| -- (prefix) |          | Decrement        | --x       | Decreases x by 1 and returns the decreased value           |
| \           | ⊻        | Bit Exclusive Or | x \ y     | Returns bitwise-exclusive-or of x and y                    |
| \|          | ∪        | Bit Or           | x \| y    | Returns bitwise-or of x and y                              |
| &           | ∩        | Bit And          | x & y     | Returns bitwise-and of x and y                             |
| ~ (prefix)  |          | Bit Flip         | ~x        | Returns bitwise complement of x                            |
| >>          |          | Left Shift       | x >> n    | Returns x value left-shifted by n bits                     |
| <<          |          | Right Shift      | x << n    | Returns x value right-shifted by n bits                    |
| \=          | ⊻=       | XOR Assign       | x \= y    | Bitwise-exclusive-ors x by y and returns the xor result    |
| \|=         | ∪=       | Bit Or Assign    | x \|= y   | Bitwise-ors x by y and returns the result                  |
| &=          | ∩=       | Bit And Assig    | x &= y    | Bitwise-ands x by y and returns the result                 |
| >>=         |          | LShift Assign    | x >>= n   | Left-shifts x by n bits and returns the result             |
| <<=         |          | RShift Assign    | x <<= n   | Right-shifts x by n bits and returns the result            |

### unsigned
The class **unsigned** is an interface class derived from `integral` for non-negative (zero or positive) integers. They do not have a `+`(positive) or `-` (negative) sign associated with them. The class `unsigned` has no additional deferred member functions.

### signed
The class **signed** is an interface class derived from `integral` for all integers include negative ones. They are negatable values and have the property of a `+`(positive) or `-` (negative)  sign associated with them. The class `signed` has the following additional deferred member functions for negatable values:
```altro
interface class signed is integral
{
    prefix func +(): ownerclass;     // negatable
    prefix func -(): ownerclass;     // negatable
    prefix func |()|: ownerclass;    // negatable
}
```
| func        | Alias   | Common Name      | Example   |   Description                                     |
|:----------- |:------- |:---------------- |:--------- |:------------------------------------------------- |
| +           |         | Positive sign    | +x       | Returns positive (unchanged) value of x            |
| -           |         | Negative sign    | -x       | Returns the negating value of x                    |
| \|()\|      |         | Absolue value    | \|x\|    | Returns the absolute value of x                    |

### real
The class **real** is an interface class derived from `numeric` with the following additional deferred member functions for negatable values:(https://en.wikipedia.org/wiki/Bitwise_operation):
```altro
interface class real is numeric
{
    prefix func +(): ownerclass;     // negatable
    prefix func -(): ownerclass;     // negatable
    prefix func |()|: ownerclass;    // negatable
}
```
| func        | Alias   | Common Name      | Example   |   Description                                     |
|:----------- |:------- |:---------------- |:--------- |:------------------------------------------------- |
| +           |         | Positive sign    | +x       | Returns positive (unchanged) value of x            |
| -           |         | Negative sign    | -x       | Returns the negating value of x                    |
| \|()\|      |         | Absolue value    | \|x\|    | Returns the absolute value of x                    |


## Built-in Number Types

All concrete built-in number types are derived from the `numeric` interface. They are [value types](ValueType.md) and their instances are [passed by value](ArgumentPassing.md). They are also simple types so you can use literals to represent instances of the type. 

### Built-in Integer Types

All built-in integer types are derived from the `integral` interface. Altro supports the following built-in integral types:

| class name  | derived from | size      | Literal Example   |   Description                 |
|:----------- |:------------ |:--------- |:----------------- |:----------------------------- |
| tiny        | signed       | 1 byte    | 43t               | Signed 8-bit integer          |
| short       | signed       | 2 bytes   | 43s               | Signed 16-bit integer         |
| int         | signed       | 4 bytes   | 43                | Signed 32-bit integer         |
| long        | signed       | 8 bytes   | 43l               | Signed 62-bit integer         |
| llong       | signed       | 16 bytes  | 43L               | Signed 128-bit integer        |
| utiny       | unsigned     | 1 byte    | 43ut              | Unsigned 8-bit integer        |
| ushort      | unsigned     | 2 bytes   | 43us              | Unsigned 16-bit integer       |
| uint        | unsigned     | 4 bytes   | 43u               | Unsigned 32-bit integer       |
| ulong       | unsigned     | 8 bytes   | 43ul              | Unsigned 62-bit integer       |
| ullong      | unsigned     | 16 bytes  | 43uL              | Unsigned 128-bit integer      |

Each of the integral types has constant meta names `min` and `max` that provide the minimum and maximum value of that type. 

| class name  |min                          | max                             |
|:----------- |:--------------------------- |:------------------------------- |
| tiny        | -128                        | 127                             |
| short       | 32,768                      | 32,767                          |
| int         | -2,147,483,648              | 2,147,483,647                   |
| long        | -9,223,372,036,854,775,808  | 9,223,372,036,854,775,807       |
| llong       | -(2^127) ≅ -(1.7 x 10^38)   | 2^127 - 1 ≅ 1.7 x 10^38         |
| utiny       | 0                           | 255                             |
| ushort      | 0                           | 65,535                          |
| uint        | 0                           | 4,294,967,295                   |
| ulong       | 0                           | 18,446,744,073,709,551,615      |
| ullong      | 0                           |  2^128 - 1 ≅ 3.4 x 10^38        |

For instance, the expression `int.min` represents the minimum value of integers in the `int` type, which is `-2147483648`.

### Built-in real Types

All built-in real number types are derived from the `real` interface.  Altro supports the following built-in real number types:

| class name  | derived from | size      | Literal Example   |   Description                 |
|:----------- |:------------ |:--------- |:----------------- |:----------------------------- |
| float       | real         | 4 byte    | 43.23f            | 4-byte floating-point number  |
| double      | real         | 8 bytes   | 43.23             | 8-byte floating-point number  |
| ldouble     | real         | 16 bytes  | 43.23l            | 16-byte floating-point number |

A real number literal can also be represented as a mantissa with an exponent. For example:
```altro
       mantissa   exponent
         ┌──┴──┐  ┌┴┐
         1.23456e-67
```

## Other Numeric Classes

Other derived concrete numeric classes are not the built-in ones in the altro core. They are provided in libraries as user defined types, such as `rational` that represents fractional numbers by exact ratios of two integers, `complex` numbers that have a real and imaginary part in a real number, `decimal` data type defined by its precision (total number of digits) and scale (number of digits to the right of the decimal point), and etc. To retain the property of arithmetic calculation, they must impement all deferred member functions listed in the `numeric` interface class.

## Numeric Type Promotion for Addable and Scalable Operation

When we use an [add or scale operation](operators.md) (binary +, -, \*, / etc.) that have another input as the second operand required to be the ownerclass for the same type of the owner (the first operand), say
```altro
x op y
```
where `x` is the owner value (the first operand), `op` is the operation and `y` is the input (the second operand). A type promotion rule is used to determine whether `x` or `y` need to be converted using widening conversions to prevent the loss of information about the magnitude of the result value. For instance, in the expression
```altro
4 + 4.2
```
The owner (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. On the otherhabd,
```altro
4.2 + 4
```
The input (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. Here is another eample:
```altro
x: tiny = 150;
y: tiny = 150;
z := x+y;
```
Here in the expression `x+y`, both `x` and `y` are promoted to int, and the result will be 300 in the type of `int`.

Here are the numeric type promotion rules for addable and scalable opration:

* if one value is a ldouble, both are promoted to ldouble, otherwise
* if one value is a double, both are promoted to double, otherwise
* if one value is a float, both are promoted to float, otherwise
* if one value is a llong, both are promoted to llong, otherwise
* if one value is a long, both are promoted to llong, otherwise
* if one value is an int, both are promoted to long, otherwise
* both tiny or short values are promoted to int.

For more information about this topic, see [Type Promotion](TypePromotion.md).

