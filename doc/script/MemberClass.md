# Member Class

A class defined within a [class](Class.md) or a [singleton](Singleton.md) is called a **member class**. A member class belongs to the [object scope](Scopes.md). A member class can also be declared as a **meta member class**, which belongs to the [class scope](Scopes.md). Example:

```altscript
class ParentClass
{
   class ChildClass                 // a member class of 'ParentClass'
   {
      class GrandChildClass         // a member class of 'ChildClass'
      {
      }
   }
   meta class SharedResourceClass   // a meta member class of 'ParentClass'
   {
      meta class SubSharedResourceClass   // a meta member class of 'SharedResourceClass'
      {
      }
   }
}
```
One typical reason for using member classes is a way of logically grouping classes that represent the behavior of their enclosing class. For example, an `Account` class may have member classes such as `deposit`, `withdraw`, and `balance`:
```altscript
class Account
{
   func deposit (value: double):double { /*code to modify balance and return the value of balance*/ }
   func withdraw (value: double):double { /*code to modify balance and return the value of balance*/  }
   func balance ():double { /*return the value of balance* }
}
```
Note that `func deposit` defines a member function which is actually a member class written in a [function form](FunctionClass.md'). For details about the concept of function, see [Function Class and Functions](FunctionClass.md).

Another typical reason for using member classes is to group a set of related classes for common utilities into a class or a singleton. For instance, the class of `Trigonometry` may provide meta member classes of `sin', 'cos', etc.: 
```altscript
class Trigonometry
{
   meta func sin(x double):double;
   meta func cos(x:double):double;
   ...
}
```

##  Using Member Classes

A **member class call** is to use the member class's [constructor](Constructor.md) to create a member object in an instance of the enclosing class or to use the member class's [functor](Functor.md) to create a function routine to perform the specified task in an instance of the enclosing class. See [Routine Type and Routine Call](Routine.md) for detail.

Because a member class is declared in the object scope, objects that are instances of a member class exist within an instance of the enclosing class. Therefore, you cannot call a member class through its enclosing class, instead, you call a member class through an instance of its enclosing class.
```altscript
a_child := ParentClass.ChildClass()    // Error:  Member class is used without an object of the owner type
a_child := ParentClass().ChildClass()  // Okay:  Create a parent object and then a child object within the parent
```
The instance of the enclosing class used to call the member function is called the **owner** of the member class call, and the instance of the member class is created within the owner.

If the member class call appears within the class body, you do not need to give the owner, the current instance of the enclosing class is implicitly used for the owner of the call. Consider the following example:
```altscript
class ParentClass
{
   class ChildClass            // a member class of 'ParentClass'
   {
   }
   a_child := ChildClass()    // call ChildClass using the self instance
                              // self.ChildClass()
}
```
When we use `ParentClass()` to create an instance of the class `ParentClass`, we need to initialize the instance. During the initialization, the member name `a_child` declared in the object scope is initialized by calling the member class `ChildClass`, and the member class call `ChildClass()` uses the instance of `ParentClass` just created.

In case we need to use a member class for the declared type in a name declaration, where an instance of the enclosing object is not involved, we should use [scope selector](OperatorSelector.md):
```altscript
a_grand_child : ParentClass::ChildClass::GrandChildClass;    // just declare the type for the name 'a_grand_child'
a_grand_child = ParentClass().ChildClass().GrandChildClass();
```
The [member selector](MemberSelector.md) operator `.` can only be used when the selected item belongs to its own scope. The class scope belongs to a class and the object scope belongs to the instance of the class. Since `ChildClass` is not in the class scope of `ParentClass`, the following declaration is invalid:
```altscript
a_grand_child : ParentClass.ChildClass.GrandChildClass;    // Error: The member is not in class scope.
```
Note that this member class concept is similar to [Java's nested class](https://docs.oracle.com/javase/tutorial/java/javaOO/nested.html#inner-class-and-nested-static-class-example), except for a subtle difference in how a member class is accessed when the instance of the enclosing is not involved. Java uses the member selector, but  using member selector here will cause an error.

##  Using Meta Member Classes

A meta member class is declared in the class scope,  objects that are instances of a meta member class exist within the enclosing class. Therefore, you can access a meta member class through its enclosing class
```altscript
some_shared_resource := ParentClass.SharedResourceClass()    // create some shared resource for the parent class `ParentClass`
```
`some_shared_resource` is created as a shared resource for all parent instances. The `ParentClass` is the **owner class** of the meta member class call that creates an child instance within the class `ParentClass`. The child instance does not belong to any individual instance of the `ParentClass` class.

In case we need to use a meta member class for the declared type in a name declaration, we need to use [member selector](OperatorSelector.md) because a meta member class is the class member of its enclosing class:
```altscript
some_resource : ParentClass.SharedResourceClass.SubSharedResourceClass;
some_resource = ParentClass.SharedResourceClass.SubSharedResourceClass();
```
If the meta member class call appears within the class body, we do not need to give the owner class, the enclosing class is implicitly used for the owner class of the call. Consider the following example:
```altscript
class ParentClass
{
   meta class SharedResourceClass   // a meta member class of 'ParentClass'
   {
   }
   meta some_shared_resource := SharedResourceClass()    // call SharedResourceClass using the selfclass
                                                         // selfclass.SharedResourceClass()
}
```
Sometimes you can write the code to access a meta member class through an instance of its enclosing class, but the compiler will actually convert it to the type of the instance, that is, the enclosing class, to access the meta member class. Example:
```altscript
class test
{
    class Nested
    {
        meta class MetaMember       // a meta member class of 'Nested'
        {
        }
    }
    n := Nested();                 // create an instance of 'Nested'
    meta_member := n.MetaMember(); // Use the 'Nested' instance to create a MetaMember instance
}
```
Here an instance of `Nested` is used to create an instance of `MetaMember`, but the actual owner used for the `MetaMember` instance creation is the type of `n`, that is, the class `Nested`.

Note that this meta member class concept is different to [Java's static nested class](https://docs.oracle.com/javase/tutorial/java/javaOO/nested.html#inner-class-and-nested-static-class-example), where a static nested class is behaviorally a top-level class that has been nested in another top-level class for packaging convenience. Here, a meta member class is not behaviorally a top-level class. Because the object [ownership rule](Ownership.md) requires that each object, except for the root, must have a parent object that owns the object, instances of a meta member class has an owner and the owner is the enclosing class.

##  Member Functions

If a member class is a [function class](FunctionClass.md), it can be written in a set of separated [functions](FunctionClass.md). These functions are member functions. Like a member class, a member function is called through an instance of its enclosing class and a meta member function is called through its enclosing class.

A member function can have an [override]([FunctionOverride](https://en.wikipedia.org/wiki/Method_overriding) (implementation replacement) in a derived subclass. A member function can also defer its implementation to derived classes (deferred member function).  See [Member Functions](MemberFunctions.md) for more description.

## Access Outer Names

A member class has direct access to names defined in its enclosing class. However, a meta member class can have access only to names defined in class scope (meta names). 

Consider the following classes:
