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

### integral
The class **integral** is an interface class derived from `numeric` with the following additional deferred member functions for [bitwise operations](https://en.wikipedia.org/wiki/Bitwise_operation):
```altro
interface class integral is numeric
{
    func \ (other: ownerclass): ownerclass;
    func | (other: ownerclass): ownerclass;
    func & (other: ownerclass): ownerclass;
    func ~ (): ownerclass;
}
```
| func   | Alias    | Common Name      | Example   |   Description                             |
|:------ |:-------- |:---------------- |:--------- |:----------------------------------------- |
| \      | ⊻        | Bit Exclusive Or | x \ y     | Returns bitwise-exclusive-or of x and y   |
| \|     | ∪        | Bit Or           | x \| y    | Returns bitwise-or of x and y             |
| &      | ∩        | Bit And          | x & y     | Returns bitwise-and of x and y            |
| ~      |          | Bit Flip         | x & y     | Returns bitwise complement of x           |

