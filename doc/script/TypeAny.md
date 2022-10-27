# Class `any`

The class `any` is the root class in the [class inheritance tree](InheritanceTree.md).
```altro
class any
{
    meta func name(): string;
    meta func is_abstract(): bool;
    meta func is_incorporeal(): bool;
    meta func is_sealed(): bool;
    meta func is_value(): bool;
    meta func is_object(): bool;
    meta func is_trivial(): bool;
    meta func subtype_check(type t: any): int;
}
```
* `meta func name(): string` -- returns the name of this type.
* `meta func is_abstract(): bool` -- returns true if this type is [abstract](AbstractClass.md).
* `meta func is_incorporeal(): bool` -- returns true if this type is [incorporeal](IncorporealClass.md).
* `meta func is_sealed(): bool` -- returns true if this type is [sealed](SealedClass.md).
* `meta func is_value(): bool` -- returns true if this type is a [value class](ValueClass.md).
* `meta func is_object(): bool` -- returns true if this type is an [object class](Object.md).
* `meta func is_trivial(): bool` -- returns true if this type is trivial, that is, its instances can be freed without using [destructor](Desctructor.md).
* `meta func subtype_check(type t: any): int` -- returns a positive integer if `t` is a (pure) subtype of this type; zero if `t` is the same of this type; or a negative integer otherwise.
