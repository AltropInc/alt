# Comparable

The class **comparable** is an interface class that imposes a total ordering on the objects or values of each derived class that implements it. The deferred member function `<=>` is the method that checks the natural ordering between two instances of the class. All concrete classes derived from the `comparable` interface have to provide the implementation of `<=>`.
```altro
interface class comparable
{
    func <=> (other: ownerclass): int;  // Note: all member functions in an interface are deferred
}
```
The implementation of the member function `<=>` that compares the [owner](SelfAndOwner.md) instance (an object or value of a comparable class) with the specified `other` input must return
* a positive integer, if the the owner instance is greater than `other` from the input;
* a negative integer, if the owner instance is less than `other` from the input;
* zero, if the owner instance is equal to `other` from the input.

## Comparable Examples

Let's see an example of a concrete comparable interface that sorts people on the basis of age:
```altro
class Person is comparable
{
    name: string;
    age: int;
    final func <=> (other: Person): int { age<=>other.age }
}
```
Here `int` is a builtin type that implements the comparable interface so that you can use `age<=>other.age` for ordering people based on their ages. Note that the overridden function `<=>` is final. That means the method of ordering people is final, no different ordering method can be provided further in any derived class from `Person`. If you do not want to fix the ordering method in the class `Person`, you can implement a comparing method without overriding the one defined in the `comparable` class interface:
```altro
class Person is comparable
{
    name: string;
    age: int;
    func <=> (other: Person): int { age<=>other.age }
}
```
This one does not override the deferred function in the `comparable` interface because the signature `(other: Person)` does not match the signature `(other: ownerclass)` if the class `Person` is not sealed or the member function itself is not final (For more discussion, see [ownerclass](SelfAndOwner.md). You can provide the final version in a derived class of `Person`:
```altro
class Student is Person
{
    grade: int;
    final func <=> (other: Student): int
    {
        if (grade==other.grade) return owner.Person::<=>(other);  // call the function defined in Person
                                                                  // if they are in the same grade
        return grade<=>other.grade;                               // otherwise the grade dominates the ordering
    }
}
```

## Two-way Comparison

The member function `<=>` defined in the `comparable` interface is a [three-way comparison](https://en.wikipedia.org/wiki/Three-way_comparison). In many cases, we use the following two-way comparisons to get a boolean value as the result of the comparison:
* x < y	  ―― returns true if x is less than y, false otherwise.
* x > y	  ―― returns true if x is greater than y, false otherwise.
* x <= y  ―― returns true if x is less than or equal to y, false otherwise.
* x >= y  ―― returns true if x is greater than or equal to y, false otherwise.
* x == y  ―― returns if x is equal to y, false otherwise.
* x != y  ―― returns if x is not equal to y, false otherwise.

In any class derived from the `comparable` interface, you do not need to provide a member function for any two-way comparison. Whenever a two-way comparison operator is used, it is automatically translated to the following three-way comparison:
* x < y	  ⟶  x<=>y < 0
* x > y	  ⟶  x<=>y > 0
* x <= y  ⟶  x<=>y <= 0
* x >= y  ⟶  x<=>y >= 0
* x == y  ⟶  x<=>y == 0
* x != y  ⟶  x<=>y != 0

Here is the example  of using two-way comparisons:
```altro
class Person is comparable
{
    name: string;
    age: int;
    final func <=> (other: Person): int { age<=>other.age }
    func is_older (other: Person): bool { owner > other }   // this is translated to owner<=>other > 0
}
s1 := Person("John", 18);
s2 := Person("Mary", 17);
b := s1 > s2;      // this is translated to s1<=>s2 > 0
```
Certainly，you can provide a member function for two-way comparison to prevent from converting it to a three-way comparison:
```altro
class Person is comparable
{
    name: string;
    age: int;
    final func <=> (other: Person): int { age<=>other.age }
    func > (other: Person): bool { age > other.age }   // this is interpreted by the system because int is built-in
}
s1 := Person("John", 18);
s2 := Person("Mary", 17);
b := s1 > s2;      // this uses the two-way operator '>' directly
```

## Built-in Comparables

Built-in types such as [integers and floating-point numbers](TypeNumeric.md) are comparable types. They are classes derived (indirectly) from the interface `comparable`. However, the implementation of the required member function `<=>` is not explicitly written in these built-in classes. These built-in classes also interpret two-way comparisons directly without a need to translate them into a three-way comparison. Therefore, the expression
```altro
4 > 3
```
is executed directly without calling any member function in the `int` class.

## Numeric Type Promotion for Comparison Operation

Note that member functions used for comparisons using `ownerclass` for the type of instance or value to be compared with. This requires that the one to be compared with must be in the same type as the owner who performs the comparison. If they are in different types, one of them must be converted to the type of the other. In case of comparing numbers, a type promotion rule is used to determine what widening conversions are used to prevent the loss of information about the magnitude. For instance, in the expression
```altro
4 > 4.2
```
The owner (integer 4) will be promoted to `double` before the expression is executed. And the result of the execution is `false`. Consider another eample:
```altro
x: int = -4;
y: uint = 150;
y_is_greater_than_y := y > x;
```
Here in the expression `x+y`, both `x` and `y` are promoted to int, and the result will be true. If we do not do the type promotion and compare the value bits of y and x directly, the result would be false.

Here are the numeric type promotion rules for comparison operation:

* if both operands are in the same type, no conversion is needed, otherwise
* if both are unsigned or both are signed, the integer in smaller magnitude is promoted to the integer of the same magnitude of the other, otherwise
* if one value is a ldouble, both are promoted to ldouble, otherwise
* if one value is a double, both are promoted to double, otherwise
* if one value is a float, both are promoted to float, otherwise
* if one value is a llong, both are promoted to llong, otherwise
* if one value is a long, both are promoted to llong, otherwise
* if one value is an int, both are promoted to long, otherwise
* both tiny or short values are promoted to int.

For more information about this topic, see [Type Promotion](TypePromotion.md).



