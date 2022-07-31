# Class Members

Class members are the members declared in an enclosing class that represent the data and behavior of instances created from the enclosing class or the enclosing class itself.

Those representing the data and behavior of instances of the enclosing class are **members of object** (abbreviation: **members**), which belong to the [object scope](Scopes.md) and they are accessed through an instance of the enclosing class. The instance of the enclosing class is the **owner** of these members.

Those representing the data and behavior of the enclosing class itself are **members of class** (abbreviation: **meta members**), which belong to the [class scope](Scopes.md) and they are accessed through the enclosing class. The enclosing class is the **owner** of these meta members.

Members of object (members) class can be one of the following kinds:
* [**Member classes**](MemberClass.md): an inner class in the enclosing class to define how member instances are structured and created within the instance of the enclosing class. A member class can be a [functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the class instances.
* [**Member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the instance of the enclosing class. But the value or object referred by the reference held by a member name can be owned by another object or class.
*  [**Member Singletons**](Singleton.md): a singleton class defined in the enclosing class and its singleton object will be created as an member object in the instance of the class.

Members of class (meta members) can be one of the following kinds:
* [**Class parameters**](ParametricClass.md): a parameter in a parametric class to introduce a family of classes by substitution of the parameter with different types or values.
* [**Meta member classes**](MemberClass.md): an inner class in the enclosing class to define how meta member instances are structured and created within the enclosing class. A meta member class can be a [meta functional class(member function)](FunctionClass.md) that provides functors for member activities that represent the behavior of the enclosing class.
* [**Meta member Singletons**](Singleton.md): a singleton object defined in the enclosing class
* [**Meta member name declarations**](NameDeclaration.md): a name declared in an enclosing class to hold a value or a reference to a composite value or object. The value and reference held by the member name is owned by the enclosing class. But the value or object referred by the reference held by a member name can be owned by another object or class.

Except for the class paramters which are declared in the [**class header**](Class.md), all other members are defined in [**class body**](Class.md).

All the members need to be named, and the name is called the **member name**. Member names must be unique within the class. Multiple member functions may appear to have the same name, but they are actually separated forms of the same member [function class](FunctionClass.md).

[Constructors](Constructor.md) and [Destructors](Destructor.md) are also defined within a class, but they are not class members. They do not have a name, and they are directly the properties of the class used to initialize and release instances of the class. A constructor is routine type invoked to initialize a new instance of the class. A destructor is routine type invoked to release the instance created from the class.

[Functors](Functor.md) defined within a class is not a class member either. They do not have a name, and they are directly the properties of the class instance and used to create a routine that accomplishes a specific task by processing the input data and generating a result, if any, within in the instance of the class.

A [type definition](TypeDefinition.md) defined within a class is not a class member. It is used to create an additional alias name for another type or a [simple class](SimpleClass.md). Though the alias name is local to the class where it is defined, the type or the simple class referred by the alias is not neccesarily a member type or a member class. It can refer to an external top-level class.

Example of class members:
```altro
class EC #(type T: any)  // A class parameter 'T' in the class 'EC' header
{                        // The 'EC' class body starts from here
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
    meta func MMF(x:T)   // A function of the meta member class named MMF
    { }
    x: int;              // A member  name 'x' that holds an integer
    const c:=1;          // A const member  name 'c' that holds an integer value 1
    s: string="abc";     // A member  name 's' that holds a reference to a string value
    meta mx: int;        // A meta member  name 'mx' that holds an integer
    meta ms :="abc";     // A meta member  name 's' that holds a reference to a string value
    ctor()               // A constructor of 'EC'. This is not a member.
    { }
    ctor(x: int)         // Another constructor with a different input interface.
    { }
    meta ctor()          // The meta constructor of 'EC'. This is not a member.
    { }
    fn ()                // A functor of 'EC'. This is not a member.
    { }
    fn (x: int)          // Another functor with a different input interface
    { } 
}                        // The 'EC' class body ends here
```

## Access of Class Members

A member is accessed through its owner and the member's name, using the **member selector** operator `.`:

```altscript
MemberOwner.MemberName
```
Therefore, a member is referred by `InstanceOfClass.MemberName`, and a meta member is referred by `Class.MemberName`. Example::
```altro
EC.T      // access the class parameter 'T' using 'EC'
EC().x    // access the variable name 'x' using an instance of the class 'EC'
EC.mx     // access the meta variable name 'mx' using the class 'EC'
```

In calling a constructor of a member class, we need to provide the input values required by the constructor of the member class. A member class may have multiple constructors with different interfaces. The one with a matched interface will be selected for this constructor call. Example:
```altro
class EC
{
    class MC             // A member class named MC
    {
        ctor(x: int) { }
        ctor(s: string) { }
    }
    meta class MMC       // A meta member class named MMC
    {
        ctor(x: int) { }
        ctor(s: string) { }
    }
}
ec  := EC();           // create an instance of EC and referred by 'ec'
mc1 := ec.MC(3);       // created an instance of MC in the instance referred by 'ec'
mc2 := ec.MC("xyz");   // created another instance of MC using a different constructor
mmc1 := EC.MMC(3);     // created an instance of MMC in the class 'EC'
mmc2 := EC.MMC("xyz"); // created another instance of MMC using a different constructor
```
Because a member class may have multiple functions with different interfaces, for the same reason, in calling a function of a member class, we need to provide the input values required by the function of the member class.  The one with a matched interface will be selected for this function call. Example:
```altro
class EC
{
    func MF(x:T):int        // A function of the member class named MF
    { }
    func MF(x,y:T):int      // Another function of the member class named MF
    { }
    meta func MMF(x:T):int  // A function of the meta member class named MMF
    { }
}
ec := EC();           // create an instance of EC and referred by 'ec'
i1 := ec.MF(3);       // Call MF's function through the instance of 'EC' just created
i2 := ec.MF(2,3);     // Call MF's another function with an input of two integers
mi := EC.MMF(3);      // Call MMF's function through the class 'EC'.
                      // This is a call to a function of the meat member class 'MMF'
```

Functors, constructors and destructors are not members. Constructors and meta functors are called by using the enclosing class name directly. See [Constructor](Constructor.md) for more description about constructor calls. Functors are called either by the instance of its enclosing class or by the enclosing class directly if the enclosing class is a stateless class. See [Functor](Functor.md) for more description about functor calls. [Destructor](Destructor.md) are not called explicitly.

Because a [type definition](TypeDefinition.md) defined within a class is not a member, we cannot use the member selector `.` to access the inner type definition. Instead, we use the scope selector `::`
```altro
class EC
{
    type NC = int[4];    // define an alias type name for int[4]
}
nc : EC::NC = (1,2,3,4);
```
