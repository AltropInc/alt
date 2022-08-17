# Numeric

## Numeric Interfaces

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

The class **integral** is an interface class derived from `numeric` with the following additional deferred member functions for bitwise operations:
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
| \|     | ∪        | Bit Or           | x | y     | Returns bitwise-or of x and y             |
| &      | ∩        | Bit And          | x & y     | Returns bitwise-and of x and y            |
| ~      |          | Bit Complement   | x & y     | Returns Bitwise complement of x           |
