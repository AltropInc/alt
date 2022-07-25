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
Note that `func deposit` actually defines a function which is a class written in a [simple class form](SimpleClassForm.md'). For details about the concept of function, see [Functor](Functor.md).

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

Because a member class is declared in the object scope, you cannot call a member class through its enclosing class, instead, you call a member class through an instance of its enclosing class.
```altscript
a_child := ParentClass.ChildClass()    // Error:  Member class is used without an object of the owner type
a_child := ParentClass().ChildClass()  // Okay:  Create a parent object and then a child object within the parent
```
In case we need to use a member class for the declared type in a name declaration, where an instance of the enclosing object is not involved, we should use [scope selector](ScopeSelector.md):
```altscript
a_grand_child : ParentClass::ChildClass::GrandChildClass;    // just declare the type for the name 'a_grand_child'
a_grand_child = ParentClass().ChildClass().GrandChildClass();
```
The [member selector](MemberSelector.md) operator `.` can only be used when the selected item belongs to its own scope. The class scope belongs to a class and the object scope belongs to the instance of the class. Since `ChildClass` is not in the class scope of `ParentClass`, the following declaration is invalid:
```altscript
a_grand_child : ParentClass.ChildClass.GrandChildClass;    // Error: The member is not in class scope.
```
Note that this member class concept is similar to [Java's nested class](https://docs.oracle.com/javase/tutorial/java/javaOO/nested.html#inner-class-and-nested-static-class-example), except for a subtle difference in how a member class is accessed when the instance of the enclosing is not involved. Java uses the member selector, which will cause an error here.

##  Using Meta Member Classes

A meta member class is declared in the class scope, you can access a meta member class through it enclosing class
```altscript
some_shared_resource := ParentClass.SharedResourceClass()    // create some shared resource for the parent class `ParentClass`
```
`some_shared_resource` is created as a shared resource for all parent instances, and it is created as a child within the class `ParentClass` and does not belong to any individual instance of the class.
Here is another example of using nested meta member classes:
```altscript
some_resource : ParentClass.SharedResourceClass.SubSharedResourceClass;
some_resource = ParentClass.SharedResourceClass.SubSharedResourceClass();
```
Note that this meta member class concept is different to [Java's static nested class](https://docs.oracle.com/javase/tutorial/java/javaOO/nested.html#inner-class-and-nested-static-class-example), where a static nested class is behaviorally a top-level class that has been nested in another top-level class for packaging convenience. A meta member class is not behaviorally a top-level class. Because the object [ownership rule](Ownership.md) requires that each object, except for the root, must have a parent object that owns the object, a meta member class has an owner and the owner is its enclosing class. Therefore, all instances of a meta member class are created in its enclosing class.


