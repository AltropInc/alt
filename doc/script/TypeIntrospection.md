# Type Introspection

Type introspection is the ability of a program to examine the type or properties of a variable at runtime.

## Type Property Checks

Type property checks are functions defined in the root class `any`:
```altro
class any
{
    meta func to_string(): string;
    meta func is_abstract(): bool;
    meta func is_incorporeal(): bool;
    meta func is_sealed(): bool;
    meta func is_value(): bool;
    meta func is_object(): bool;
    meta func is_trivial(): bool;
    meta func is_convertible_to(type t: any): bool;
    meta func is_castable_to(type t: any): bool;
    meta func is_subtype_of(type t: any): bool;
    meta func implements(type t: any): bool;
    meta func super_class(): type any;
    meta func subtype_check(type t: any): int;
    func # (x: any) : type any;
    prefix func typeof (x: any) : type any;
}
```
Let's consider a declaration `x: any` that declares a polymorphic variable `x` that can hold an instance of any type. The expression `x#` that calls the postfix function `#` returns the type of the current value held in `x`.
| func         | Example         |   Description                                       |
|:------------ |:--------------- |:--------------------------------------------------- |
| to_string    | t.to_string()   | returns t's type name in [string](String.md)                   |
| is_abstract  | t.is_abstract() | returns true if the type t is an [abstract type](AbstractClass.md)  |
| is_incorporeal  | t.is_incorporeal() | returns true if the type t is an [incorporeal type](IncorporealClass.md)  |
| is_sealed  | t.is_sealed() | returns true if the type t is a [sealed type](SealedClass.md)  |
| is_value  | t.is_value() | returns true if the type t is a [value type](ValueClass.md)  |
| is_object  | t.is_object() | returns true if the type t is an [object class](Object.md)  |
| is_object  | t.is_trivial() | returns true if the type t is an [trivial type](TrivialType.md)  |
| is_convertible_to  | t.is_convertible_to(t1) | returns true if the type t is [auto-convertible](ConvertibleTypes.md) to t1  |
| is_castable_to  | t.is_castable_to(t1) | returns true if the type t can be [casted](Typecasting.md) to t1  |
| is_subtype_of  | t.is_castable_to(t1) | returns true if the type t is a [subtype](Subtype.md) of t1  |
| implements  | t.implements(t1) | returns true if the type t implements the [interface class](InterfaceClass.md) t1  |
| super_class  | t.super_class() | returns the [base class](Class.md) of t  |
| subtype_check  | t.subtype_check(t1) | returns 0 if t==t1, -1 if t is not subtype of t1, otherwise a positive integer    |
| #  | x# | returns the type of the current value held by the variable `x`    |
| typeof  | typeof x | the same as x#    |

Note that the type returned by functions of `typeof` and `super_class` cannot be used for type declaration. For instance:
```altro
n: numeric := 3;
m: typeof n;
```
The above declaration of `m` is ill-formed, before `typeof n` returns the actual type of the current value contained in the variable of `n`, not a fixed one as the declared type `numeric` for the variable of `n`. The `typeof` expression is only used for type checking

## Type Check Using Type Condition

The type condition appearing in an [if-statement](StatementIf.md) is a type assumption expression  in the format of "expression -> tag is type". If the actual type of the expression is the subtype of the given type, the expression evaluates to true and the value of the expression is assigned to the tag. For example
```altro
n: numeric := 3;
if (n -> i is int)
{
    // do something with i
}
elif (n -> d is double)
{
    // do something with d
}
```

## Type Check Using Type Case

The [switch statement](StatementSwitch.md), if the control expression is a type expression, can be used for type checking:
```altro
func numeric_generator(): numeric { /* generate some number in any numeric type */ }
switch (typeof numeric_generator() -> result)
{
    case int: // do something when the result is an integer
    case double:  // do something when the result is a double
}
```





