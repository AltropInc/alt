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
Let's consider a declaration `x: any` that declare a polymorphic variable `x` that can hold an instance of any type. The expression `x#` that calls the postfix function `#` returns the type of the current value held in `x`.
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
| #  | x# | returns the type of the current value held by the varaible `x`    |
| typeof  | typeof x | the same as x#    |

## Type Check Using Type Condition

