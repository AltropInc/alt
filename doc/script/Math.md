# Math

In mathematics, some constants and functions are important enough to deserve their own names. Examples are constants π, e, τ, and functions +, -, √, and sin. Some constants and functions are defined in the assoicated class, for example, the class `float` and `double` have have their own version of constant e and function +:
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
Putting functions like `sin` in the `math` class makes the function call look traditional, e.g. `sin(x)`, instead of `x.sin()`. Although Altro supports the prefix member funtion, for instance,
```altro
value class double is real
{
    prefix func sin();
}
```
With the definition of the prefix member function, you can use prefix expression `sin x` or `sin(x)` to call the `sin` member function. However, the prefix expression has a limitation to accept convertable types. For instance, the expression `sin 3` will ill-formed because the class `int` (the type of `3`) does not have a member funcion `sin`. But if we put the function `sin` in the `math` class, we will find an overloaded input interface provided for the `sin` function to accept an integer, which is convertable to a floating-point number, and in this case, the interface `(x: double): double` will be selected for calling `sin`.

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
| ln10    | double | [natural Logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 10 approximately equal to 2.30259... |

These contants are in double precision. If you want any contant in a specific precision, use the one defined the the concrete floating point type, for exmaple, use `ldouble.pi` to get the π value in long double precision.

## Math Functions

The following functions provided in the 'math` class has four overloading interfaces:<br>
&nbsp;&nbsp;&nbsp;&nbsp;func *name* (hfloat): hfloat;<br>
&nbsp;&nbsp;&nbsp;&nbsp;func *name* (float): float;<br>
&nbsp;&nbsp;&nbsp;&nbsp;func *name* (double): double;<br>
&nbsp;&nbsp;&nbsp;&nbsp;func *name* (ldouble): ldouble;<br>

| name    | example   | description                                                                               |
|:------- |:---------- |:----------------------------------------------------------------------------------------- |
| floor   | floor(x)   | Rounds x downward, returning the largest integral value that is not greater than x |
| ceil    | ceil(x)    | Rounds x upward, returning the smallest integral value that is not less than x |
| trunc   | trunc(x)   | Rounds x toward zero, returning the nearest integral value that is not larger in magnitude than x. |
| round   | round(x)   | Returns the integral value that is nearest to x, with halfway cases rounded away from zero. |
    ADD_MATH_FUNC(lround)
    ADD_MATH_FUNC(llround)
    ADD_MATH_FUNC(nearbyint)
    ADD_MATH_FUNC(rint)
    ADD_MATH_FUNC(lrint)
    ADD_MATH_FUNC(llrint)
    
| sqrt    | sqrt(x)   | Returns the square root of x. |
| cbrt    | cbrt(x)   | Returns the cubic root of x. |
| exp     | exp(x)    | Returns the base-e exponential function of x, which is e raised to the power x: e<sup>x</sup>. |
| log     | log(x)    | Returns the natural logarithm of x. |
| log10   | log10(x)  | Returns the common (base-10) logarithm of x. |
| erf     | erf(x)    | Returns the error function of x (2/√π)⎰<sub>0~x</sub>e<sup>-t<sup>2</sup></sup>dt). |
| erfc    | erfc(x)   | Returns the complementary error function of x (1-2/√π)⎰<sub>0~x</sub>e<sup>-t<sup>2</sup></sup>dt). |
| tgamma  | tgamma(x) | Returns the gamma function of x. |
| lgamma  | lgamma(x) | Returns the natural logarithm of the absolute value of the gamma function of x. |
    ADD_MATH_FUNC_2(pow)
    ADD_MATH_HYPOT()

| sin    | sin(x)   | Returns the sine of an angle of x radians. |
| cos    | cos(x)   | Returns the cosine  of an angle of x radians. |
| tan    | tan(x)   | Returns the tangent  of an angle of x radians. |
| asin   | asin(x)  | Returns the principal value of the arc sine of x, expressed in radians. |
| acos   | acos(x)  | Returns the principal value of the arc cosine of x, expressed in radians. |


 ADD_MATH_FUNC(abs)


    ADD_MATH_ATAN()
    ADD_MATH_FUNC(cosh)
    ADD_MATH_FUNC(sinh)
    ADD_MATH_FUNC(tanh)
    ADD_MATH_FUNC(acosh)
    ADD_MATH_FUNC(asinh)
    ADD_MATH_FUNC(atanh)
