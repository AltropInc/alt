# Comparable

The class **comparable** is an interface class that imposes a total ordering on the objects of each class that implements it. The deferred member function `<=>` is the method that checks the natural ordering between two instances of the class and all concrete classes derived from `compatable` have to provide the implementation of `<=>`.
```altro
interface class comparable
{
    func <=> (other: ownerclass): int;
}
```
The implementation of the member function `<=>` that compares the [owner](SelfAndOwner.md) instance (an object or value of comparable) with the specified `other` input must return
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
Here `int` is a builtin type that implements the comparable interface so that you can use `age<=>other.age` for ordering people based on their ages. Note that the overridden function `<=>` is final. That means method of ordering people is final, no differnet ordering method can be provided further in any derived class from `Person`. If you do not want to fix the ordering method in the class `Person`, you can implement a comparing method without overriding the one defined in the `comparable` class:
```altro
class Person is comparable
{
    name: string;
    age: int;
    func <=> (other: Person): int { age<=>other.age }
}
```
This one does not override the deferred function in the `comparable` interface because the signature `(other: Person)` does not match the signature `(other: ownerclass)` if the class `Person` is not sealed or the member function itself is not final (For more discussion, see [ownerclass](SelfAndOwner.md). Then, in a derived class of `Person`, you can provide the final one:
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

The member function defined in the `comparable` interface is a [three-way comparison](https://en.wikipedia.org/wiki/Three-way_comparison). In many cases, we use the following two-way comparisons to get a boolean value as the result of the comparison:
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
For instance,
```altro
class Person is comparable
{
    name: string;
    age: int;
    final func <=> (other: Person): int { age<=>other.age }
    func is_older (other: Person): bool { owner > other }   // this is translated to owner<=>other > 0
    func > (other: Person): bool { age > other.age }        // this is interpreted by the system because int is built-in
}
s1 := Person("John", 18);
s2 := Person("Mary", 17);
b := s1 > s2;      // this is translated to s1<=>s2 > 0
```
Certainly， you provide two-way comparison to prevent using a three-way comparison:
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
b := s1 > s2;      // this uses two-way operator > directly
```

## Builtin Comparables
