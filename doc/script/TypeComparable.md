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

Note that member functions used for comparisons using `ownerclass` for the type of instance or value to be compared with. This requires that the one to be compared with must be in the same type as the owner who performs the comparison. If they are in different types, one of them must be converted to the type of the other. Using comparison operators on built-in types honors the [type promotion rule](TypePromotion.md), a rule of using widening conversions to prevent the loss of information about the magnitude of a value. For instance, in the expressionn
```altro
4 > 4.2
```
The owner (integer 4) will be promoted to `double` before the expression is executed. And the result of the execution is `false`.

However, in a parametric polymorphism case, only type conversion is used and type promotion is not honored. For example:
```altro
meta func compare#(sealed type T: comparable)(x,y: T): int
{
    return x<=>y;
}
order := compare(5, 5.1);   // order gets value 0. 5 is not promoted to 5.0
```
In calling the `compare` function with a parametric interface, the type parameter `T` is bound to `int`, and the second input parameter '5.1' is converted to an integer '5'. And in this case, the actual parameter for 'x' will not be promoted to double.

In other polymorphism case where the types of arguments cannot be determined to be the same, performing a comparison on these arguments is ill-formed because `ownerclass` requires that only values of the same type can be compared.
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


