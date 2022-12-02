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

## Constants in Numeric Types

Constants in numeric types are meta names.

The following constants are defined in floating-number types (hfloat, float, double, and ldouble) with different precision:
| name     | description                                                                               |
|:-------- |:----------------------------------------------------------------------------------------- |
| pi       | [π](https://en.wikipedia.org/wiki/Pi) approximately equal to 3.14159... |
| tau      | 2 * π approximately equal to 6.28319... |
| e        | [euler's number](https://en.wikipedia.org/wiki/E_(mathematical_constant)) approximately equal to 2.71828... |
| phi      | [golden ratio](https://en.wikipedia.org/wiki/Golden_ratio) approximately equal to 1.61803... |
| sqrt2    | √2 approximately equal to 1.41421... |
| sqrt_e   | √e approximately equal to 1.64872... |
| sqrt_pi  | √π approximately equal to 1.77245... |
| ln2      | [natural logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 2 approximately equal to 0.69315... |
| ln10     | [natural Logarithm](https://en.wikipedia.org/wiki/Natural_logarithm) of 10 approximately equal to 2.30259... |
| infinity | positive [infinity](https://en.wikipedia.org/wiki/Infinity), ∞ can also be used. Use -infinity for negative infinity |
| max_val  | the largest finite value of the given type |
| min_val  | the smallest finite value of the given type |
| epsilon  | the difference between 1.0 and the next representable value of the given floating-point type |
| nan      | a value representing “not a number” of the given floating-point type |


