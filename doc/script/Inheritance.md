# Inheritance

Inheritance is a mechanism where you can create a new class from an existing class for a hierarchy of classes that share a set of members. The class which is newly created is known as the **subclass** or **derived class** of the existing class on which the new class is based. The existing class is the **superclass** or **base class** of the newly created class. The relationship between a derived class and a base class is an [**is-a** relationship](https://en.wikipedia.org/wiki/Is-a). A subclass is a **subtype** of its base, i.e. any instance of a subclass is also an instance of its base.

## Single Inheritance

In Altro, each class can only be derived from one existing base class. This is called **single inheritance**.You use the keyword `is` or colon to identify the base class that your subclass is derived from. If you don’t declare a superclass, your class is implicitly based on the class `Object`. `Object` is the root of a subclass tree for all objects created in Altro. However, `Object` is not the ultimate root. The ultimate root in the entire class hierarchy is the type `any` that includes [built-in classes](PrimitiveTypes.md) that are not explicitly defined in Altro.

The syntax of using inheritance in Altro is:
```Altro
class BaseClass
{
    //class members
}
class DerivedClass is BaseClass
{
    // override some members in BaseClass
    // more class members
}
```
A concrete example:
```Altro
class Person
{
    name: string;
    age: int;
    address: string;
}
class Student is Person
{
    grade: int;
    school: string;
}
```
In the above code, when an object of `Student` is created, all class members in the superclass `Person` (the declared values held by `name`, `age`, and `address`) are inherited by the `Student` object, so that we can access these values in the superclass by using the object of the subclass:
```Altro
object JohnSmith is Student
{
    name = "John Smith";
    age = 17;
    address = "740 Place, Town 12345";
    grade = 9;
    school = "Town High School";
}
```

## Shadowing

Though names declared in the base class are accessible in derived class, the derived class can introduce a new member with a name that is already used in the base class, then the  new member shadows the one with the same name in the base class. For example:
```Altro
class Person
{
    func occupation(): string { "Occupation: " }
}
class Student is Person
{
    func occupation(): string { "Student" }
}
```
If we use a student reference to call `occupation`, the returned value will be "Student":
```Altro
s := Student();
s_occupation := s.occupation();    // s_occupation gets "Student"
```
This is because the function `occupation` defined in `Student` **shadows** the function `occupation` defined in `Person`. If we need to call `occupation` defined in `Person` by using a student reference, we need to use the scope selector to explicitly refers the function defined in `Person`:
```Altro
s := Student();
s_occupation := s.Person::occupation() + s.occupation(); // s_occupation gets "Occupation: Student"
```
For the same reason, we need to use the scope selector within the body of the derived class to access any shadowed name declared in the base class:
```Altro
class Person
{
    func occupation(): string { "Occupation: " }
}
class Student is Person
{
    func occupation(): string { Person::occupation() + "Student" }
}
```

## Overriding

Back to the Person-Student example:
```Altro
class Person
{
    func occupation(): string { "unset" }
}
class Student is Person
{
    func occupation(): string { "Student" }
}
```
When we use a name reference of `Person` to call `occupation`, the function `occupation` defined in `Person` will be called even if the name reference refers to a student:
```Altro
p: Person = Student();
p_occupation := p.occupation();    // p_occupation gets "unset"
```
This is because the function `occupation` defined in `Person` and the one defined in `Student` belong to two distinct function classes. When we use a name reference of `Person` to call `occupation`, we explicitly call the one defined in `Person`.

What if we want to call the function defined in a derived class through a base reference? This is the mechanism of **overriding**. When the function of the same name is defined in a derived class, it overrides the implementation of the same function defined in the base class. To make this happen, we just need to define the function in the base virtual, meaning, overridable:
```Altro
class Person
{
    virtual func occupation(): string { "unset" }
}
class Student is Person
{
    override func occupation(): string { "Student" }
}
```
Then,
```Altro
p: Person = Student();
p_occupation := p.occupation();
```
`p_occupation` will get the value of "Student". This overriding mechanism allows [subtype polymorphism or inclusion polymorphism](https://en.wikipedia.org/wiki/Subtyping) in which a name of a superclass can denote instances possibly in many different subclasses and can be used to access members actually defined in the subclass that is the type of the current instance denoted by the name.

## Finalizing

In the overriding example, the keyword `override` ensures that the function `occupation` is virtual in the superclass `Person`. The program is ill-formed if this is not true from the superclass class and a compile-time error is generated. However, the keyword `override` is optional, the function `occupation` will always override the virtual one in the superclass if they have the matched interfaces:
```Altro
class Person
{
    virtual func occupation(): string { "unset" }
}
class Student is Person
{
    func occupation(): string { "Student" }  // still overrides without presenting 'override'
}
```
So then we come to the question of how to stop overriding a virtual function in derived classes. This can be achieved by using the keyword `final`. The `final` specifier ensures that the member function will not be overridden by derived classes.
```Altro
class Base
{
    virtual func foo() {}
    func bar() {}
}
class Derived is Base
{
    func foo() {}           // override the one in 'Base'
    override func bar() {}  // Error: 'override' can only be used for virtual defined in the base
}
class Derived2 is Derived
{
    final func foo() {}     // override the one in 'Derived', but cannot be overridden further
}
class Derived3 is Derived2
{
    func foo() {}          // Error: Override final member function is not allowed
}
```
Like the specifier `override`, the pecifier `final` also ensures that the function  is virtual in the superclass:
```Altro
class Base
{
    func foo() {}
}
class Derived is Base
{
    final foo bar() {}     // Error: 'final' can only be used for virtual defined in the base
}
```

## Deferring

A class can have member functions with only a specified interface but without an implementation block:
```Altro
class Person
{
    deferred func occupation(): string;
}
```
A deferred member function is virtual, that is, it is overridable. In fact, its implementation has to be presented in a derived class:
```Altro
class Student is Person
{
    func occupation(): string { "Student" }
}
```
A class that contains deferred member functions is an [abstract class](AbstractClass.md):
```Altro
p := Person();   // Error: Abstract class is not instantiable
```
A class that contains only deferred member functions is an [interface class](InterfaceClass.md). And in an interface class, all functions declared are deferred functions even without using the `deferred` specifier:
```Altro
interface class Person
{
    func occupation(): string;  // 'occupation' is deferred ensures that the function  is virtual in the superclass:
}
```
Please note the difference between a deferred function and a function in [forward declaration](ForwardDeclaration.md). A forward declared function is also a function can be declared without having it followed by its implementation, but its implementation cannot be deferred to derived classes. Its implementation must be provided in the same class with a full declaration:
```Altro
class Person
{
    func occupation(): string;  // 'occupation' is forward declared
    // more members here declarations
    func occupation(): string {"unset"}  // 'occupation' is fully declared
}
```
The program is ill-formed if the full declaration is not provided for a forward declared function:
```Altro
class Person
{
    func occupation(): string;  // 'occupation' is forward declared
}  // Error: The class interface is not implemented: occupation
```

## Multiple Inheritance Through Interface Classes

Altro supports single inheritance through the is-a relation (`class A is B`). However, multiple [interface classes](InterfaceClass.md) can be inherited through the implementation-relationship:
```Altro
class Derived is Base implements Interface1, Interface2
{
}
```

## Differences among Shadowing, Overriding and Overloading

Please note the difference between shadowing and overriding. In shadowing, the members with the same name in subclass and superclass are different entities so the subclass can use the name freely for any other purpose. But in overriding, the members with the same name in subclass and superclass are the same entity so the subclass must use the name following certain rules in order to achieve overriding.

Please also note the difference between overloading and overriding. Overloading is a kind of [ad hoc polymorphism](https://en.wikipedia.org/wiki/Ad_hoc_polymorphism) in which a [function class](FunctionClass) can have multiple interfaces (functions), and which interface to use in a function call is determined by the actual parameters provided in the function call. This ad hoc polymorphism is resolved statically during compile time. Overriding is a kind of [subtype polymorphism](https://en.wikipedia.org/wiki/Subtyping) in which one interface of a [function class](FunctionClass) can have multiple implementations in different subclasses, and which implementation to use in a function call is determined by the actual type of the object that owns the function call. This subtype polymorphism is resolved dynamically at run time.

## Inheritance in Parametric Class

## Related Topics
* [Class](Class.md)
* [Class Inheritance Tree](InheritanceTree.md)
* [Class Members](ClassMember.md)
* [Member Class](MemberClass.md)
* [Interface Class](InterfaceClass.md)
* [Sealed Class](SealedClass.md)
* [Kinds of Classes](KindsOfClasses.md)


