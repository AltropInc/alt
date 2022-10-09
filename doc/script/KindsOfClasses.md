# Kinds of Classes

Everything in Altro is associated with [classes](Class.md) and and their instances. A class acts like a constructor or blueprint for creating their instances.

## Data Class vs. Object Class

Altro has two kinds of class instances: data and objects. Data are pieces of information represented in a form that can be stored, passed, and processed. Objects contain data and reside as a node in an object tree of an Altro application. Both data and objects can be structured and can have a set of behaviors that can be applied. However, objects are building blocks of an Altro application which is a root object. All other objects are created as offsprings of the application root object. An object has references to its parent and children. Data as pieces of information stored in memory do not reside in the application object tree. Unlink objects, though data can possibly be structured as in composite data, component data do not necessarily have references to its enclosing data, and the enclosing data do not necessarily have explicit reference to its component data.

Therefore， Altro has two kinds of classes: **data class** and **object class**. Obviously, instances of a data class are data, and instances of an object class are objects. The class `any` is the root class from which all other classes are derived. All object classes are derived from the class `Object`. All other classes derived from the class `any` are data classes. This is illustrated as below:
```
                        any (the root class)
                  ┌──────────────┴────────────┐
           (data classes)               Object (object classes)
```

## Value Class vs. Composite Class

Class instances can be passed either by value or by reference. Instances of a [**value class**](ValueClass.md) are also called **values** and they are from one argument to another by copying the value. Instances of a **composite class** are passed by reference, which means that they are not copied when they are passed from one argument to another, instead, only the reference to the instance is passed.

Object classes are non-value classes. Data classes can be either value classes or non-value classes.
```
                                                 any (the root class)
                                      ┌────────────────────┴──────────────────┐
                               (data classes)                           Object (composite object classes)
               ┌──────────────────────┴────────────────────────┐
      (value data classes)                          (composite data classes)
       ┌───────┼─────....                        ┌───────┬─────┼───────┬──────...
 value class such as int, char, ...           class   tuple  array  string  stream ...
```
See [Value Class](ValueClass.md) and [Argument Passing](ArgumentPassing.md) for detail.

## Incorporeal Class vs. Corporeal Class

An [**incorporeal class**](IncorporealClass.md) is a special composite data class that has an empty data structure and has no instance constructor nor instance destructor. An incorporeal class can still have [metadata](ClassMember.md), [member functions](FunctionClass.md) (a kind of [incorporeal](IncorporealClass.md) [member classes](MemberClass.md)), and [functors](Functor.md). All other classes that does not fall into the incorporeal category are **corporeal classes**.
```
                                                 any (the root class)
                                      ┌────────────────────┴──────────────────┐
                               (data classes)                           Object (corporeal classes)
               ┌──────────────────────┴────────────────────────┐
   (value data classes, corporeal)                       (composite data classes)
                                         ┌─────────────────────┼──────────────────...
                                       class          (tuple, array, string, and stream are corporeal)
                            ┌────────────┴───────────┐
                    (corporeal classes)   (incorporeal classes)
```
Since an incorporeal class has no data members, it has no physical storage (storage size is zero). References to instances of incorporeal classes always point to the same special place in the memory. Incorporeal classes are used as enclosing classes of member functions or functors that can be called with the necessity of instantiation. For detail, see [Incorporeal Class](IncorporealClass.md).

## Abstract Class vs. Concrete Class

An [**abstract class**](AbstractClass.md) is a class that cannot be instantiated, but they can be subclassed. On the other hand, a **concrete** class is an instantiable class.

A class is abstract if it contains deferred member functions or inherits such a function that is not overridden with an implementation, or it is declared abstract using the specifier abstract, or it is a parametric class with type parameters unbound. An abstract class can be instantiated either by a concrete subclass or by implementing all the deferred member functions in a singleton.

Note that a [parametric class](ParametricClass.md) with unbound type parameters is also an abstract class. This is a different concept than a parametric class provided in other programming languages such as generic classes in Java and C# and templates class in c++. Since a parametric class is a true type, it can be used as an abstract type for polymorphic variable declarations, wherein the type parameters may be left undetermined.

[**Interface classes**](InterfaceClass.md) are special abstract classes in which only deferred functions and interfaces are defined. Only interface classes can be used as multiple bases in [inheritance](Inheritance.md).
```
                                (all classes)
               ┌────────────────────┴──────────────────┐
         (abstract classes)                       (concrete classes)
               |
        (interface classes)
```
For detail, see [Abstract Class](AbstractClass.md).

## Member Class vs. Parent Class

Altro allows you to define a class within another class. 

A [**member class**](MemberClass.md) belongs to the instance scope of the enclosing class, and a [**meta member class**](MemberClass.md) belongs to the class scope of the enclosing class. We refer the enclosing class as a **parent class*.

An instance of a member class is created as a member of the instance of the enclosing class, and an instance of a meta member class is created as a meta member of the enclosing class. Therefore, to create an instance of a member class, an instance of the enclosing class must be given as the owner (unless the enclosing class is an incorporeal class), and to create an instance of a meta member class, the enclosing class is given as the owner.

A data class can be a member class of either a data class or an object class, however, an object class can only be a member class of an object class. This is because object classes are automatically chained in a tree structure and an instance of a member object class must reside as a child node of an instance of its parent class.

When we refer an enclosing class as a **parent class**, we sometimes refer a member class as a **child class** of its parent class, and this forms a class tree. When instances of a child class are created as children of the instance of a parent class, the associated object tree is formed. Note that the concept of class tree is different that class inheritance tree where the relationship between a superclass and subclasses (or base class and derived classes).

A **free class** is a class that is specified standalone and is not enclosed in any other class. Note that a free class is typically not used to create a top-level object. In an Altro application, there is only one root object, which is the altro application node. All other objects are created as offsprings of the root node. A free data class is used to create data values that do not reside in the object tree. And a free object class can be used to create member (child) objects of any other object. You do not give the owner explicitly when you create instances of a free class.  Altro provides a set of primitive free classes. User defined free classes can be either defined by external script files to be loaded/unloaded into the application dynamically, or by library extension through other programming languages.

## Superclass vs. Subclass

In [Inheritance](Inheritance.md), the existing class from which the new classes are derived is known as the **superclass**. It is also known as the **base class**. While **subclasses** (also known as **derived classes**) is the new class that inherits [class members](ClassMembers.md) from superclasses and possibly intruduces more class members. The relationship between a subclass and a superclass is an [is-a relationship](https://en.wikipedia.org/wiki/Is-a), and this relationship forms a class inheritance tree. Altro supports single inheritance through the is-a relation (`class A is B`). However, multiple [interface classes](InterfaceClass.md) can be inherited through the implementation-relationship.

A subclass can also be derived from a [parametric class](ParametricClass.md) by reconstraining or binding type paramters. In addition, functional subclasses can be derived through function declaration (see Function Class (FunctionClass.md) for detail).

## Sealed Class vs. Open Class

An [**sealed class**](SealedClass.md) is a class that cannot be inherited, but they still can be used to derive subclasses by adding more functors using [functions](FunctionClass.md) or [parametric substitution](ParametricClass.md).

**Open classes** are classes that are not sealed and can be used as base classes in class inheritance.

## Related Topics
* [Value Class](ValueClass.md)
* [Incorporeal Class](IncorporealClass.md)
* [Abstract Class](AbstractClass.md)
* [Member Class](MemberClass.md)
* [Interface Class](InterfaceClass.md)
* [Function Class](FunctionClass.md)
* [Parametric Class](ParametricClass.md)
* [Sealed Class](SealedClass.md)
* [Tuple](Tuple.md)
* [Array](Array.md)
* [Stream](Stream.md)
* [String](String.md)
* [Numeric Class](TypeNumeric.md)
* [Char Class](TypeChar.md)
* [Enum Class](TypeEnum.md)
* [Enumset Class](TypeEnumset.md)

