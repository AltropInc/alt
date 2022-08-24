# Type Promotion

**Type promotion** is a kind of type conversion applied to a binary operation on numeric values. Consider the following binary arithmetic operation:
```altro
x + y
┊ ┊ └┄┄┄┄┄┄┄ input value, the right hand operand
┊ └┄┄┄┄┄┄┄┄┄ member function, the operator
└┄┄┄┄┄┄┄┄┄┄┄ the owner value, the left hand operand
```

Before calling the member function, the operand on both sides may be converted from a lower magnitude type to a higher magnitude type to prevent information loss. This kind of type conversion from magnitude rank to higher magnitude is called type promotion. For example:
```altro
x: int = 70000;
y: int = 70000;
z := x * y;
```
Here in the expression `x * y`, both `x` and `y` are promoted from `int` to `long`, and the result will be 4,900,000,000 which is in the type of `long`. If we do not do the type promotion to get the result in the type of `int`, the result would be wrong because 4,900,000,000 is bigger than the maximum value that can be represented by the int type.

## Numeric Type Promotion Rules for Adding and Scaling Operation

When we use an [adding or scaling operator](operators.md) such as +, -, \*, / etc., the following numeric type promotion rules are used:

* if one value is a ldouble, both are promoted to ldouble, otherwise
* if one value is a double, both are promoted to double, otherwise
* if one value is a float, both are promoted to float, otherwise
* if one value is a llong, both are promoted to llong, otherwise
* if one value is a long, both are promoted to llong, otherwise
* if one value is an int, both are promoted to long, otherwise
* both tiny or short values are promoted to int.

For instance, in the expression
```altro
4 + 4.2
```
The owner (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. On the other habd,
```altro
4.2 + 4
```
The input (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. Here is another example:
```altro
x: tiny = 150;
y: tiny = 150;
z := x+y;
```
Here in the expression `x+y`, both `x` and `y` are promoted to int, and the result will be 300 in the type of `int`.

## Numeric Type Promotion Rules for Comparing Operation
When we use an [adding or scaling operator](operators.md) such as +, -, \*, / etc., the following numeric type promotion rules are used:

* if both operands are in the same type, no conversion is needed, otherwise
* if both are unsigned or both are signed, the integer in smaller magnitude is promoted to the integer of the same magnitude of the other, otherwise
* use the promotion rules for adding or scaling operators

For instance, in the expression
```altro
4 > 4.2
```
The owner (integer 4) will be promoted to `double` before the expression is executed. And the result of the execution is `false`. Consider another example:
```altro
x: int = -4;
y: uint = 150;
y_is_greater_than_y := y > x;
```
Here in the expression `x > y`, both `x` and `y` are promoted to int, and the result will be true. If we do not do the type promotion and compare the value bits of y and x directly, the result would be false.

## Type Promotion and Polymorphism Case

In a parametric polymorphism case, only type conversion is used and type promotion is not honored. For example:
```altro
meta func compare#(sealed type T: comparable)(x,y: T): int
{
    return x<=>y;
}
order := compare(5, 5.1);   // order gets value 0. 5 is not promoted to 5.0
```
In calling the `compare` function with a parametric interface, the type parameter `T` is bound to `int`, and the second input parameter '5.1' is converted to an integer '5'. And in this case, the actual parameter for 'x' will not be promoted to double.

In other polymorphism cases where the types of arguments cannot be determined to be the same, performing a comparison on these arguments is ill-formed because `ownerclass` requires that only values of the same type can be compared.
```altro
c : comparable;
c_is_greater := c > 4.0; // Error: Types not comparable: comparable, int
```
Type check is required to compare a polymorphic argument when its type cannot be determined to be the same as the one to be compared with:
```altro
c : comparable;
c_is_greater: bool;
if (c->d is double) { c_is_greater = d > 4.0; }
```

