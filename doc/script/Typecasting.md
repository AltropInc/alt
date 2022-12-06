# Type-casting

Altro is a strong-typed language. Some [type conversions](ConvertibleTypes.md) may imply a different interpretation of the value and require an explicit conversion, known as type-casting. Type cast can be stated by a **cast expression**:

*cast-expression*:<br>
&nbsp;&nbsp;&nbsp;&nbsp;**cast**(*type-expression*, [*expression*](Expression.md))

The cast expression can be used for two kinds of purposes: [narrowing conversion](ConvertibleTypes.md), and [downcast](https://en.wikipedia.org/wiki/Downcasting).  

Narrowing conversion is a typecasting that may cause data loss and the converted value may not be able to be converted back to the original value. For example, converting a 8-byte integer to a 4-byte integer is a narrowing conversion, and this conversion is cannot be done automatically and explicit type expression must be used for narrowing conversion:
```altro
i: int = 65537;
j: short = cast(short, i);
```
Here the 4-byte integer value 65537 is forced to be converted into a 2-byte integer and the value in j will be 1, which is a result of a data loss. Without cast, an error will be reported.

Downcasting or type refinement is the act of casting a variable of a supertype to one of its subtypes. If the actual type of the value held by the variable is not the subtype to be casted, the downcasting will fail and an exception will be raised.
```altro
n: numeric = 65537;
i: int = cast(int, n);
d: double = cast(double, n);
```
Here `n` is a polymorphic variable of the type `numeric`, which can hold a value of any numeric type. The cast expression `cast(int, n)` is successful but the cast expression `cast(double, n)` is not.

