# Enumeration and Enumeration Set Type

## Enumeration Type

An *enumeration type* is a data type consisting of a set of named values called *enumeration values*. All enumeration types are derived from the `enum` class, which implements the *enumerable* interface:
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
    meta func null_value(): ownerclass;       // returns the an enumeration value that represents a null (invalid) value
    const func order(): int;                  // returns the order in the enumeration value list
    const func is_valid(): bool;              // returns true if this is not a null value (a valid value)
    const func to_string(): string;           // returns the name of the enumeration value
    meta func from_string(str: string): ownerclass;   // returns the enumeration value from the enumeration value name
    meta func from_order(order: int): ownerclass;     // returns the enumeration value from the order in the value list
}
```

## Define a Enumeration Type

To define an enumeration type, use the `enum` keyword and specify the names of enum members:
```altro
enum Day(SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY);
```
To access the the enumeration value listed in the enumeration type, using the expression `enumeration_type::enumeration_value_name`:
```altro
day_one := Day::SUNDAY;
day_two := Day::MONDAY;
```
If the enumeration value name is not shadowed by the meta member function name defined in the class `enum`, you may also use expression  `enumeration_type.enumeration_value_name` to access the enumeration value. Otherwise, the expression `enumeration_type.name` refers to the meta member function defined in the class `enum`. For example:
```altro
enum MedalOrdinal(first, second, third);
first_medal: MedalOrdinal = MedalOrdinal.first;  // Error: The type of the expression provided cannot be converted to the expected Day type
```
In this case, you need to use either
```altro
first_medal: MedalOrdinal = MedalOrdinal.first();
```
or
```altro
first_medal: MedalOrdinal = MedalOrdinal::first;
```
Each enumeration type has an null value, which is implicitly defined. Use meta member function `null_value` to get the null value:
```altro
invalid_day := Day.null_value();
```
The `null value` is considered an invalid value. Use `is_valid` to check is the enumration value is not an null value. The following expression:
```altro
invalid_day.is_valid();
```
returns false.

## Enumeration Set with an Enum Type

You use a set of enumeration values to represent a choice from a set of mutually exclusive values or a combination of choices. To represent a combination of choices, use the expression `enumeration_type(enumeration_value_name1, enumeration_value_name2, ...)`. For example:
```altro
week_days := Day(MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY);
```
Note that the type of  `week_days` is `enumset of Day`, not `Day` for a single enumeration_value:
```altro
week_days: enumset of Day = Day(MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY);
```
The expression `enumeration_type(...)` gives a universal set of all enumeration values:
```altro
all_days := Day(...);
```
The type `enumset` is always coupled with an enumeration type. You can use type definition to give a name to the `enumset` type:
```altro
type Days = enumset of Day;
```
Where an enumset value is needed, the single enumeration value is automatically converted the a single element enumset value, as shown in the following example:
```altro
single_day_set: Days = Day::Monday;
```
You can also use the expression `enumset_type(...)` to give a universal set of all enumeration values:
```altro
all_day_set: Days = Days(...);
```
And use `enumset_type()` to give an empty set:
```altro
empty_day_set: Days = Days();
```

## Enumeration Set with Implicit Enum Type

You can also define an `enumset` type without explicitly giving an enumeration type:
```altro
enumset DaySet (Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday);
```
The enumset `DaySet` uses an anonymous enumeration type implicitly defined inside the enumset (Note that it is not compatible with the enum type `Day`, and hence DaySet is considered a different type from `enumset of Day`). Use this form of definition when you want to always treat enumerations as set values, and in this case, a single enumeration value can only be expressed as a single element set value.
```altro
monday_only: DaySet = DaySet(Monday);
weekend_days: DaySet = DaySet(Sunday, Saturday);
```

## Enumeration Set Class

All concrete enumeration set type are derived from the enumeration set class `enumset`, which is a value class that implements the *comparable* interface with a support of all built-in operators in [clusivity group](Operators.md):
```altro
value class enumset is comparable
{
    const func <=> (other: ownerclass): int;  // implements comparable interface
    const func \ (other: ownerclass): ownerclass;  // clusivity operator 'exclusive or'
    const func | (other: ownerclass): ownerclass;  // clusivity operator 'inclusive  or'
    const func & (other: ownerclass): ownerclass;  // clusivity operator 'conjunction  or'
    const func \= (other: ownerclass): ownerclass;  // clusivity operator 'exclusive or assign'
    const func |= (other: ownerclass): ownerclass;  // clusivity operator 'inclusive or assign'
    const func &= (other: ownerclass): ownerclass;  // clusivity operator 'conjunction or assign'
    prefix func ~ (): ownerclass;  // clusivity operator 'flip'
    const func is_empty (): bool;  // returns true if the set contains no enumeration value
    const func length (): int;  // returns the number of elements in this set.
    const func to_string (): string;  // returns string to represent the set.
    const func has (other: ownerclass): bool;  // returns true if this set contains any of the value in other
    const func has_all (other: ownerclass): bool;  // returns true if this set contains all values in other
    const func has_only (other: ownerclass): bool;  // returns true if this set only contains all values in other
    const func ⊃ (other: ownerclass): bool;  // the same as has_all
    const func is_subset (other: ownerclass): bool;  // returns true if this set is a subset of other
    const func ⊂ (other: ownerclass): bool;  // the same as is_subset
    const func is_disjoint (other: ownerclass): bool;  // returns true if this set has no elements in common with other. 
    const func - (other: ownerclass): ownerclass;  // returns a set containing elements present in this set but not in other. 
    const func + (other: ownerclass): ownerclass;  // returns a set containing any elements present in either set.
    func += (other: ownerclass): ownerclass;  // adds all elements in another set to this one and returns the added result.
    func -= (other: ownerclass): ownerclass;  // removes all elements in another set from this one and returns the removed result.
    func toggle (other: ownerclass): ownerclass;  // toggles elements in this set when the element appeas in other.
    func set (other: ownerclass): bool;  // adds all elements in another set to this one and returns true if this set is changed.
    func unset (other: ownerclass): bool;  // removes all elements in another set from this one and returns true if this set is changed.
    const meta func all (): ownerclass;  // returns the set that contains all valid enumeration values.
    const meta func empty (): ownerclass;  // returns an empty set
}
```

The built-in clusivity operators, when applied to enumset, have the similar meaning when they are applied to [integral types](TypeNumeric.md), which is explained as below:
| operator    | Alias | Example   | Description                                                                                 |
|:----------- |:----- |:--------- |:------------------------------------------------------------------------------------------- |
| \           | ⊻     | x \ y     | Returns a set containing every element present in either x or y, but is not present in both |
| \|          | ∪     | x \| y    | Returns a set containing any elements present in either x or y                              |
| &           | ∩     | x & y     | Returns a set containing every element present in both x and y                              |
| ~ (prefix)  |       | ~x        | Returns a complement set containing every element that does not present in x                |
| \=          | ⊻=    | x \= y    | Modifies x by y using operator \ and returns the modified result                            |
| \|=         | ∪=    | x \|= y   | Modifies x by y using operator | and returns the modified result                            |
| &=          | ∩=    | x &= y    | Modifies x by y using operator & and returns the modified result                            |

Because an enumeration value can be implicitly converted to an enumset value when a set value is expected, you can give the enumeration value for the input of a function where a set is required. For example:
```altro
enum Day(SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY);
all_days := Day(...);
weekdays := all_days - Day(SUNDAY, SATURDAY);
all_days_expect_for_sunday := all_days - Day::SUNDAY;
```
Here the function `-` requires an input of `enumset of Day`. The expression `all_days - Day::SUNDAY` is actually converted to `all_days - Day(SUNDAY)`.

## Iteration through Enumeration Set

A value of an enumeration set is [iterable](TypeIterable.md). It can be used in a foreach loop. For example,
```altro
enum Day(SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY);
weekdays := Day(MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY);
foreach (day in weekdays)
{
    // do something for each weekday
}
```
To iterate all enumeration values, you can do: 
```altro
foreach (day in Day(...))
{
    // do something for each day
}
```
You can also use other loop statements to manually iterate through an enumeration set value, but you have to check if the enumeration value is actually contained in the set:
```altro
somedays := Day(MONDAY, WEDNESDAY, FRIDAY);
for (day:=Day.first(); day.is_valid(); ++day)
{
    if (somedays ⊃ day)
    {
       // do something for the day 
    }
}
```
