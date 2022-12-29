# Math

In mathematics, some constants and functions are important enough to deserve their own names. Examples are constants π, e, τ, and functions +, -, √, and sin. Some constants and functions are defined in the associated class, for example, the class `float` and `double` have have their own version of constant e and function +:
```altro
value class float is real
{
    meta const float e =  2.718282f;
    func + (other: ownerclass): ownerclass; 
}
value class double is real
{
    meta const double e =  2.71828182845904523536;
    func + (other: ownerclass): ownerclass; 
}
value class ldouble is real
{
    meta const ldouble e =  2.71828182845904523536028747135266ld;
    func + (other: ownerclass): ownerclass; 
}
```
See [Numeric Types](TypeNumeric.md) for the list of all constants and member functions provided in numeric types.

Some other constants and functions are defined in the `math class`, for example:
```altro
data class math
{
    meta const double e =  2.71828182845904523536;
    func sin (x: float): float;
    func sin (x: double): double;
    func sin (x: ldouble): ldouble;
}
```
The `math` class is an [incorporeal class](IncorporealClass.md). Calling member functions in an incorporeal class can be done without the need of an instance of the class. Instead, you call the functor directly using the class name:
```altro
math.sin(π);
```
Putting functions like `sin` in the `math` class makes the function call look traditional, e.g. `sin(x)`, instead of `x.sin()`. Although Altro supports the prefix member function, for instance,
```altro
value class double is real
{
    prefix func sin();
}
```
With the definition of the prefix member function, you can use prefix expression `sin x` or `sin(x)` to call the `sin` member function. However, the prefix expression has a limitation to accept convertible types. For instance, the expression `sin 3` will ill-formed because the class `int` (the type of `3`) does not have a member function `sin`. But if we put the function `sin` in the `math` class, we will find an overloaded input interface provided for the `sin` function to accept an integer, which is convertible to a floating-point number, and in this case, the interface `(x: double): double` will be selected for calling `sin`.

## Math Constants

The `math` class provides the following meta constants:

| name    | type   | description                                                                               |
|:------- |:------ |:----------------------------------------------------------------------------------------- |
| π       | double | [π](https://en.wikipedia.org/wiki/Pi) approximately equal to 3.14159... |
| pi      | double | same as π |
| τ       | double | 2 * π approximately equal to 6.28319... |
| tau     | double | same as τ |
| e       | double | [euler's number](https://en.wikipedia.org/wiki/E_(mathematical_constant)) approximately equal to 2.71828... |
| phi     | double | [golden ratio](https://en.wikipedia.org/wiki/Golden_ratio) approximately equal to 1.61803... |
| sqrt2   | double | √2 approximately equal to 1.41421... |
| sqrt_e  | double | √e approximately equal to 1.64872... |
| sqrt_pi | double | √π approximately equal to 1.77245... |
| ln2     | double | [natural logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 2 approximately equal to 0.69315... |
| ln10    | double | [natural logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 10 approximately equal to 2.30259... |

These constants are in double precision. If you want any constant in a specific precision, use the one defined the concrete floating point type, for example, use `ldouble.pi` to get the π value in long double precision

## Math Functions

The following functions are provided in the 'math` class:

**Common arithmetic functions**
* **func abs(x:T):T;** where T can be `int`, `long`, `llong`, `float`, `double`, or `ldouble`. It Returns the absolute value of x, i.e. |x|. For example, `abs(-2.8)` returns `2.8`, and `abs(-8)` returns `8`
* **func floor(x:T):T;** where T can be `float`, `double`, or `ldouble`. It rounds x downward, returning the largest integral value that is not greater than x. For example, `floor(2.8)` returns `2.0`, and `floor(-2.8)` returns `-3.0`
* **func ceil(x:T):T;** where T can be `float`, `double`, or `ldouble`. It rounds x upward, returning the smallest integral value that is not less than x. For example, `ceil(2.8)` returns `3.0`, and `ceil(-2.8)` returns `-2.0`
* **func trunc(x:T):T;** where T can be `float`, `double`, or `ldouble`. It rounds x toward zero, returning the nearest integral value that is not larger in magnitude than x. For example, `trunc(2.8)` returns `2.0`, and `trunc(-2.8)` returns `-2.0`
* **func round(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the integral value that is nearest to x, with halfway cases rounded away from zero. For example, `round(2.8)` returns `3.0`, `round(2.3)` returns `2.0`, `round(-2.8)` returns `-3.0`, `round(-2.3)` returns `-2.0`.
* **func lround(x:T):long;** where T can be `float`, `double`, or `ldouble`. It is a similar function to `round` except for the return type is a 64-bit integer.
* **func llround(x:T):llong;** where T can be `float`, `double`, or `ldouble`. It is a similar function to `round` except for the return type is a 128-bit integer.
* **func remainder(x,y:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the floating-point remainder of x/y, which is rounded to nearest. For example, `remainder(5.3, 2)** returns `-0.7`, and `remainder(18.5, 4.2)` returns `1.7`.
* **func fmod(x,y:T):T;** where T can be `float`, `double`, or `ldouble`. It is a similar function to `remainder` and returns the same but with the quotient rounded to the nearest integer, instead of being truncated.  For example, `fmod(5.3, 2)` returns `1.3`, and `fmod(18.5, 4.2)` returns `1.7`.
* **func div(x,y:T): (quot, rem:T);** where T can be `int`, `long`, or `llong`. It computes both the quotient and the remainder of the division of the numerator x by the denominator y.

**Exponential and logarithmic functions**
* **func sqrt(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the square root of x.
* **func cbrt(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the cubic root of x.
* **func exp(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the base-e exponential function of x, which is e raised to the power x: e<sup>x</sup>.
* **func pow(x, y:T):T;** where T can be `float`, `double`, or `ldouble`. It returns base `x` raised to the power exponent `y`: x<sup>y</sup>.
* **func hypot(x, y:T):T;** where T can be `float`, `double`, or `ldouble`. It returns  the hypotenuse of a right-angled triangle whose legs are `x` and `y`, i.e. the square root of (x<sup>2</sup>+y<sup>2</sup>).
* **func log(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the natural logarithm of x.
* **func log10(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the common (base-10) logarithm of x.

**Error functions**
* **func erf(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the error function of x: 2/√π)⎰<sub><sub>0</sub></sub><sup><sup>x</sup></sup>e<sup>-t<sup>2</sup></sup>dt.
* **func erfc(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the complementary error function of x: 1 - 2/√π)⎰<sub><sub>0</sub></sub><sup><sup>x</sup></sup>e<sup>-t<sup>2</sup></sup>dt.
* **func tgamma(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the gamma function of x.
* **func lgamma(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the natural logarithm of the absolute value of the gamma function of x.

**Trigonometric functions**
* **func sin(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the sine of an angle of x radians.
* **func cos(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the cosine of an angle of x radians.
* **func tan(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the tangent of an angle of x radians.
* **func asin(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the principal value of the arc sine of x, expressed in radians.
* **func acos(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the principal value of the arc cosine of x, expressed in radians.
* **func atan(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the principal value of the arc tangent of x, expressed in radians.
* **func atan(y:T)over(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the principal value of the arc tangent of y/x, expressed in radians.

**Hyperbolic functions**
* **func sinh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the hyperbolic sine of x.
* **func cosh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the hyperbolic cpsine of x.
* **func tanh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the hyperbolic tangent of x.
* **func asinh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the nonnegative area hyperbolic sine of x.
* **func acosh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the nonnegative area hyperbolic cosine of x.
* **func atanh(x:T):T;** where T can be `float`, `double`, or `ldouble`. It returns the nonnegative area hyperbolic tangent of x.

