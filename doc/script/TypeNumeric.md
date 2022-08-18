# Numeric

Numeric classes representing numeric values that have the property of arithmetic calculation such as addition, substraction, multiplication, and division. Numeric interfaces can be further divided into three category: integral, rational, and complex. The integral class represents whole numbers - numbers are not in fractions. The rational class represents real numbers that can be expressed as the quotient or fraction and their bitwise represenation in platform is approximate (note that irrational numbers can only be represented approximately by rational numbers. Complex numbers have a real and imaginary part, which are each a rational number.

## Numeric Interfaces

All concrete numeric classes such as int, short, long, double, float, etc. re derived from the interface class **numeric**.
```
                                                  nemric
                              ┌─────────────────────┴───────────┬─────────────────────┐
                          integral                           rational              complex
              ┌───────────────┴─────────────┐            ┌───────┼───────┐
          unsigned                        signed      float   double  ldouble 
  ┌─────┬─────┼─────┬─────┐      ┌─────┬────┼────┬────┐
utiny ushort uint ulong ullong  tiny short int long llong 
```

### numeric
The class **numeric** is an interface class that imposes arithmetic functions on a derived class that implements them:
```altro
interface class numeric is comparable
{
    func + (other: ownerclass): ownerclass;
    func - (other: ownerclass): ownerclass;
    func * (other: ownerclass): ownerclass;
    func / (other: ownerclass): ownerclass;
    func ÷ (other: ownerclass): ownerclass;
    func % (other: ownerclass): ownerclass;
    func ^ (other: ownerclass): ownerclass;
    func += (other: ownerclass): ownerclass;
    func -= (other: ownerclass): ownerclass;
    func *= (other: ownerclass): ownerclass;
    func /= (other: ownerclass): ownerclass;
    func ÷= (other: ownerclass): ownerclass;
    func %= (other: ownerclass): ownerclass;
    func ^= (other: ownerclass): ownerclass;
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
| %      |          | Modulus          | x % y     | Returns the division remainder                      |
| ^      |          | Power            | x ^ y     | Returns the value of x raised to the power of y     |
| +=     |          | Add Assign       | x += y    | Adds y to x and x gets the sum                      |
| -=     |          | Sub Assignn      | x -= y    | Substrcts y from x and x gets the difference        |
| \*=    |          | Mul Assign       | x \*= y   | Multiplies x by y and x gets the product            |
| /=     |          | Div Assign       | x /= y    | Divides x by y and x gets the quotient              |
| ÷=     | /#=      | Int Div Assign   | x ÷= y    | Divides x by y and x gets the integral part of the quotient    |
| %=     |          | Mod Assign       | x %= y    | Divides x by y and x gets the remainder             |
| ^=     |          | Power Assign     | x ^= y    | Raised the value of x to the power of y             |

### integral
The class **integral** is an interface class derived from `numeric` with the following additional deferred member functions for [bitwise operations](https://en.wikipedia.org/wiki/Bitwise_operation):
```altro
interface class integral is numeric
{
    func ++ (): ownerclass;
    func -- (): ownerclass;
    prefix func ++ (): ownerclass;
    prefix func -- (): ownerclass;
    func \ (other: ownerclass): ownerclass;
    func | (other: ownerclass): ownerclass;
    func & (other: ownerclass): ownerclass;
    prefix func ~ (): ownerclass;
    func << (n: int): ownerclass;
    func >> (n: int): ownerclass;
    func \= (other: ownerclass): ownerclass;
    func |= (other: ownerclass): ownerclass;
    func &= (other: ownerclass): ownerclass;
    func <<= (n: int): ownerclass;
    func >>= (n: int): ownerclass;
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
The class **signed** is an interface class derived from `integral` for all integers include negative ones. They have the property of a `+`(positive) or `-` (negative)  sign associated with them. The class `signed` has the following additional deferred member functions:
```altro
interface class signed is integral
{
    prefix func + (): ownerclass;
    prefix func - (): ownerclass;
}
```
| func        | Alias   | Common Name      | Example   |   Description                                      |
|:----------- |:------- |:---------------- |:--------- |:-------------------------------------------------- |
| + (prefix) |          | Positive sign    | +x        | Returns positive (unchanged) value of x            |
| + (prefix) |          | Negative sign    | -x        | Returns the negating value of x                    |

## Compound Assignments

A compound assignment in the form `x op= y` is equivalent to x = x op y, except that x is evaluated only once. For exampe, the following code
```altro
x: int = 4;
x += 5.2;
```
is equivalent to
```altro
x: int = 4;
x = x + 5.2;
```
The result of `x + 5.2` is 9.2 which is converted to the type of `x` in assigment. Therefore, `x` gets the value 9.

If you want to define your own numeric class derived from any numeric interface classes, you are not required to provide member functions for compound assignments. They will be automatically translated into the form `x = x op y`. However，the translation may not be eqivalent if there is a side effect in evaluating `x`. To prevent this from happening, you can provide member functions for compound assignments in your numeric class. 

For built-in types, compound assignments are interpreted directly without the translation.



