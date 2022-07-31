# Class Members

Class members are the members declared in an enclosing class that represent the data and behavior of instances created from the enclosing class or the enclosing class itself.

Those representing the data and behavior of instances of the enclosing class are **members of object** (abbreviation: **members**), which belongs to the [object scope](Scopes.md) and they are accessed through an instance of the enclosing class. The instance of the enclosing class is the **owner** of a member.

Those representing the data and behavior of the enclosing class itself are **members of class** (abbreviation: **meta members**), which belongs to the [class scope](Scopes.md) and they are accessed through the enclosing class. The enclosing class is the **owner** of a meta member.

Members of object (members) class can be of the following kinds:
* [**Member classes**](MemberClass.md): an inner class in the enclosing class to define how member instances are structured and created within the instance of the enclosing class. A member class can be a [functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the class instances.
* [**Member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the instance of the enclosing class. But the value or object referred by the reference held by a member name can be owned by another object or class.
*  [**Member Singletons**](Singleton.md): a singleton class defined in the enclosing class and its singleton object will be created as an member object in the instance of the class.

Members of class (meta members) can be of the following kinds:
* Class parameters
* [**Meta member classes**](MemberClass.md): an inner class in the enclosing class to define how meta member instances are structured and created within the enclosing class. A meta member class can be a [meta functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the enclosing class.
* [**Meta member Singletons**](Singleton.md): a singleton object defined in the enclosing class
* [**Meta member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the enclosing class. But the value or object referred by the reference held by a member name can be owned by another object or class.

Except for the class paramters which are declared in the [**class header**](Class.md), all other members are defined in [**class body**](Class.md)

[Constructors](Constructor.md) and [Destructors](Destructor.md) are also defined within a class, but they are not class members. They are directly the properties of the class used to initialize and release instances of the class. A constructor is routine type invoked to initialize a new instance of the class. A destructor is routine type invoked to release the instance created from the class.

[Functors](Functor.md) defined within a class is not a class member either. They are directly the properties of the class instance and used to create a routine that accomplishes a specific task by processing the input data and generating a result, if any, within in the instance of the class.

A [type definition](TypeDefinition.md) defined within a class is not a class member. It is used to create an additional alias name for another type or a [simple class](SimpleClass.md). Though the alias name is local to the class where it is defined, the type or the simple class referred by the alias is not neccesarily a member type or a member class. It can refer to an external top-level class.

## Access of Class Members

All the members need to be named, and the name is called the **member name**. Member names must be unique within the class. Multiple member functions may appear to have the same name, but they are actually separated forms of the same member [function class](FunctionClass.md).

A member is accessed through its owner and the member's name, using the **member selector** operator `.`:
```altscript
MemberOwner.MemberName
```





Except for routine types (functors, constructors and destructor) which are accessed through the operator '()',


Example of class members:
```altscript
class MemberExample #(type T: any)  // A class parameter 'T' in the header
{                        // The class body starts from here
    class MC             // A member class named MC
    { }
    meta class MMC       // A meta member class named MMC
    { }
    object MO            // A member singleton object named MO
    { }
    meta object MMO      // A meta member singleton object named MMO
    { }
    func MF(x:T)         // A function of the member class named MF
    { }
    func MF(x,y:T)       // Another function of the member class named MF
    { }
    meta MMF(x:T)        // A function of the meta member class named MMF
    { }
    x: int;              // A memeber name 'x' that holds an integer
    const c:=1;          // A const memeber name 'c' that holds an integer value 1
    s: string="abc";     // A memeber name 's' that holds a reference to a string value
    meta mx: int;        // A meta memeber name 'mx' that holds an integer
    meta ms :="abc";     // A meta memeber name 's' that holds a reference to a string value
    ctor()               // A constructor
    { }
    ctor(x: int)         // Another constructor with different input interface
    { }
    meta ctor()          // The meta constructor
    { }
    fn ()                // A functor
    { }
    fn (x: int)          // Another functor with different input interface
    { }
}                        // The class body ends here
```
