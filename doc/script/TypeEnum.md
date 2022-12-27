# Enumeration and Enumeration Set Type

## Enumeration Type

An *enumeration type* is a data type consisting of a set of named values called *enumeration values*. All enumeration types are derived from the `enum` class, which impelemts the *enumerable* interface:
```altro
interface class enumerable is comparable, incrementable
{
}
value class enum is enumerable
{
    const func <=> (other: ownerclass): int;  // implements comparable interface
    func ++ (): ownerclass;                   // implements incrementable interface
    func -- (): ownerclass;                   // implements incrementable interface
    prefix func ++ (): ownerclass;            // implements incrementable interface
    prefix func -- (): ownerclass;            // implements incrementable interface
    meta func count(): int;                   // returns number of enumeration values
    meta func first(): ownerclass;            // returns the first enumeration value
    meta func last(): ownerclass;             // returns the last enumeration value
    meta func null_value(): ownerclass;       // returns the an enumeration value that represents a null (invlid) value
    const func order(): int;                  // returns the order in the enumeration value list
    const func is_valid(): bool;              // returns true if this is not a null value (a valid value)
    const func to_string(): string;           // returns the name of the enumeration value
}
```

## Define a Enumeration Type

To define an enumeration type, use the `enum` keyword and specify the names of enum members:
```altro
enum Day(SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY);
```
