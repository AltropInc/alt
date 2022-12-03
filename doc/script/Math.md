# Math

In mathematics, some constants and functions are important enough to deserve their own names. Examples are constants π, e, τ, and functions +, -, √, and sin. Some constants and functions are defined in the assoicated class, for example, the class `float` and `double` have have their own version of constant e and function +:
```altro
value class float is real
{
    meta float e =  2.718282f;
    func + (other: ownerclass): ownerclass; 
}
value class double is real
{
    meta float e =  2.71828182845904523536;
    func + (other: ownerclass): ownerclass; 
}
value class ldouble is real
{
    meta float e =  2.71828182845904523536028747135266ld;
    func + (other: ownerclass): ownerclass; 
}
```
And some other constants and functions are defined in the `math `class`:
```altro
data class math
{
    meta double e =  2.71828182845904523536;
    func sin (x: float): float;
    func sin (x: double): double;
    func sin (x: ldouble): ldouble;
}
```
The `math` class is an [incorporeal class](IncorporealClass.md). Calling member functions in an incorporeal class can be done without the need of an instance of the class. Instead, you call the functor directly using the class name:
```altro
math.sin(π);
```
Putting functions like `sin` in the `math` class makes the function call look traditional, e.g. `sin(x)`, instead of `x.sin()`. Although Altro supports the prefix member funtion, for instance,
```altro
value class double is real
{
    prefix func sin();
}
```
Then you can use prefix expression `sin x` or `sin(x)` to call the `sin` function. However, the prefix expression has a limitation to accept convertable types. For instance, the expression `sin 3` will ill-formed because the class `int` does not have a member funcion `sin`. But if we put the function `sin` in the `math` class, we will find an overloaded input interface provided for the `sin` function to accept an integer, which is convertable to a floating-point number, and in this case, the interface `(x: double): double` will be selected.

## Constants in Floating-Number Types (hfloat, float, double, and ldouble)

The following (meta) constants are defined in floating-number types to represent the information about floating-point types (hfloat, float, double, or ldouble):
| name      | type      |description                                                                               |
|:--------- |:--------- |:---------------------------------------------------------------------------------------- |
| max_val   | selfclass | the largest positive finite value of the given type |
| min_val   | selfclass | the lowest negative finite value of the given type  |
| smallest  | selfclass | the smallest positive nonzero value of the given type |
| epsilon   | selfclass | the difference between 1.0 and the next representable value of the given floating-point type |
| max_exp   | int       | the largest positive number n such that 2ⁿ⁻¹ is a representable value of the given floating-point type |
| min_exp   | int       | the lowest negative number n such that 2ⁿ⁻¹ is a representable value of the given floating-point type |
| max_exp10 | int       | the largest positive number n such that 10ⁿ is a representable value of the given floating-point type |
| min_exp10 | int       | the lowest negative number n such that 10ⁿ is a representable value of the given floating-point type |
| dig       | int       | the number of digits in base-2 that can be represented by the given floating-point type without change |
| dig10     | int       | number of decimal digits that can represent a value of the given floating-point type |
| infinity  | selfclass | positive [infinity](https://en.wikipedia.org/wiki/Infinity), ∞ can also be used. Use -infinity for negative infinity |
| nan       | selfclass | a value representing “not a number” of the given floating-point type |

The following (meta) constants are defined in floating-number types to represent special values in different precision according to the given floating-point type (hfloat, float, double, or ldouble):
| name      | type      | description                                                                               |
|:--------- |:--------- |:----------------------------------------------------------------------------------------- |
| pi        | selfclass | [π](https://en.wikipedia.org/wiki/Pi) approximately equal to 3.14159... |
| tau       | selfclass | 2 * π approximately equal to 6.28319... |
| e         | selfclass | [euler's number](https://en.wikipedia.org/wiki/E_(mathematical_constant)) approximately equal to 2.71828... |
| phi       | selfclass | [golden ratio](https://en.wikipedia.org/wiki/Golden_ratio) approximately equal to 1.61803... |
| sqrt2     | selfclass | √2 approximately equal to 1.41421... |
| sqrt_e    | selfclass | √e approximately equal to 1.64872... |
| sqrt_pi   | selfclass | √π approximately equal to 1.77245... |
| ln2       | selfclass | [natural logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 2 approximately equal to 0.69315... |
| ln10      | selfclass | [natural Logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 10 approximately equal to 2.30259... |

## Functions in Floating-Number Types (hfloat, float, double, and ldouble)

```altro
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
prefix func +(): ownerclass;              // negatable
prefix func -(): ownerclass;              // negatable
prefix func |()|: ownerclass;             // negatable
prefix func ⌈()⌉: ownerclass;
prefix func ⌊()⌋: ownerclass;
func ²() : ownerclass;
func ³() : ownerclass;
func √() : ownerclass;
func ∛() : ownerclass;
```
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
| +      |          | Positive sign    | +x        | Returns positive (unchanged) value of x            |
| -      |          | Negative sign    | -x        | Returns the negating value of x                    |
| \|()\| |          | Absolue value    | \|x\|     | Returns the absolute value of x                    |
| ⌈()⌉   |           | Absolue value    | ⌈x⌉        | Returns the smallest integer value not less than x |
| ⌊()⌋   |           | Absolue value    | ⌊x⌋        | Returns the largest integer value not greater than x |
| ²      |          | Squared          | x²        | Returns x\*x                                       |
| ³      |          | Cubed            | x³        | Returns x\*x\*x                                    |
| √      |          | Square Root      | √x        | Returns square root of x                           |
| ∛      |          | Cube Root        | ∛x        | Returns cube root of x                             |
