# Class Members

Class members are the members declared in an enclosing class that represent the data and behavior of instances created from the enclosing class or the enclosing class itself. Those representing the  data and behavior of the enclosing class itself are meta members.

## Class Members For Objects

Class members representing the data and behavior of instances of a class can be of the following kinds:
* [**Member classes**](MemberClass.md): an inner class in the enclosing class to define how member instances are structured and created within the instance of the enclosing class. A member class can be a [functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the class instances.
* [**Member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the instance of the enclosin class. But the value or object referred by the reference held by a member name can be owned by other object or class.
*  [**Member Singletons**](Singleton.md): a singleton class defined in the enclosing class and its singleton object will be created as an member object in the instance of the class.
* [**Functors**](Functor.md): a [function routine type](Routine.md) which is called to accomplish a specific task by processing the input data and generating a result, if any, in the output type as specified.

Example of class members:

## Class Members For Classes

Class members representing the data and behavior of the class itself can be of the following kinds:
* Class parameters
* [**Meta member classes**](MemberClass.md): an inner class in the enclosing class to define how meta member instances are structured and created within the enclosing class. A meta member class can be a [meta functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the enclosing class.
* [**Meta member Singletons**](Singleton.md): a singleton object defined in the enclosing class
* [**Meta member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the the enclosin class. But the value or object referred by the reference held by a member name can be owned by other object or class.
* Constructors
* Destructors
