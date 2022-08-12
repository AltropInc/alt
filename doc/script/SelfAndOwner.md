# self, selfclass, owner, ownerclass

## self

The keyword **self** is a reserved name used within a class body to refer current instance created by this class or by a derived class of this class. Example:
```altro
class Base
{
    a:int;
    ctor(a: int) { self.a = a; }     
}
class Derived is Base
{
    b:int;
    ctor(a, b: int) super(a) { self.b = b }
}
d := Derived(1,2);
```
When an object is created by the constructor call `Derived(1,2)`, the `self` names appearing in constructors of `Base` and `Derived` all refer to the instance of `Derived`, which is the same instance referred by the name `d`.

In most cases, the name `self` can be ignored in a class body when it is used to access a [member name](ClassMember.md) in the object or calss [scope](Scopes.md), unless the member name is shadowed by a name in an inner scope.  the name `self` is often used in a constructor or functor when we have to pass the reference of the current instance into a function. Example: 
```altro
class Base
{
    a:int;
    ctor(a: int) { self.a = a; }     
}
func compare_base (x, y: Base) : int { x.a <=> y.a }
class Derived is Base
{
    b : int;
    fn (other: Derived) : int       // a functor
    {
       compare_base(self, other)    // pass the current instance into a function
    }
    ctor(ai, bi: int) super(ai)     // a constructr
    {
       b = bi                       // 'self' is not require to access the name 'b' in object scope
                                    // because 'b' is not shadowed by any name in a local scope
    }
}
```

It is similar to Java's this reference or C++'s this pointer. However, there is a major distinction: self appearing in a function body refers to the instance of the function instead of the instance of the class that encloses the function. The reason for this distinction is that ALT treats a member function the same as a member class. The function body is just a simple form of member class. To access the instance of the class that encloses the function within the function body, use the keyword owner, which is the reserved object name to represent the enclosing object of the self instance.

The keyword selfclass represents the actual type of the instance represented by self. Therefore, selfclass appearing in a class body may refer to the type of this class or a subclass of this class. The keyword ownerclass represents the actual type of the instance represented by owner.
