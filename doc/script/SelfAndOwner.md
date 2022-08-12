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

In most cases, the name `self` can be ignored in a class body when it is used to access a [member name](ClassMember.md) in the object or class [scope](Scopes.md), unless the member name is shadowed by a name in an inner scope.  the name `self` is often used in a [constructor](Constructor.md) or [functor](Functor.md) when we have to pass the reference of the current instance into a function. Example: 
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
The name `self` can also be use in [constructor delegation](ConstructorDelegation.md):
```altro
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (top, left, width, height: double)
    {
        top_left = (top, left);
        bottom_right = (top + width, left + height);
    }
    ctor () self(0,0,0,0) {}   // delegated constructor call using 'self'
    ctor (origin: (x:double; y:double); width, height: double) self(origin.x, origin.y, width, height)
    {
    }
}
```

## owner
The keyword **owner** is a reserved name used within a member class body to refer the enclosing instance of the current instance created by the member class or by a derived class of the member class. Example:
```altro
class OuterClass
{
    a:int = 0;
    ctor(a: int) { self.a = a; }     
    class InnerClass
    {
        a:int;
        ctor(a:int)
        {
            if (a<0) self.a = owner.a; else self.a = a;
        }
    }
}
```
Here in the constructor `InnerClass`, `self.a` denotes the name `a` declared in the [object scope](Scopes.md) of the `InnerClass` class, and `owner.a` denotes the name `a` declared in the [object scope](Scopes.md) of the `OuterClass` class.

Like `self`, the name `owner` can be ignored in a class body when it is used to access a [member name](ClassMember.md) in the object or class [scope](Scopes.md) of an enclosing class, unless the member name is shadowed by a name in an inner scope:
```altro
class OuterClass
{
    a:int = 0;
    ctor(a: int) { self.a = a; }     
    class InnerClass
    {
        b:int;
        ctor(b:int)
        {
            if (b<0) self.b = a; else self.b = b;  // the name `a` in the enclosing class cane be referred
                                                   // withing using 'owner' because it is not shadowed.
        }
    }
}
```

Please note that a [member function](FunctionClass.md) is considered a member class. Therefore, access names declared in the enclosing class must use `owner`. The `self` name inside a member function denotes the current instance of the member function class:
```altro
class OuterClass
{
    a:int = 0;
    func > (other: OuterClass) { owner.a > other.a }
    func < (other: OuterClass) { self.a < other.a }
                                      ^ ------------- Error: Name undefined
}
```
The superclass of the meber function `<` is `func`, which is a [stateless class](StatelessClass.md] that has no members in its object scope. Accessing the name `a` by using `self` is to find the name `a` in the instance of a stateless class and is an error.

There a major distinction in accessing enclosing names within a member function between altro and other programming lauguages such as Java and C++. The distinction is caused by the concept of a member function that is not considered a member class in other programming lauguages. In altro, `self` appearing in a member function refers to the instance of the function class itsef, not the instance of the class that encloses the member function. To access the instance of the class that encloses the function within the function body, `owner` has to be used.

## selfclass

The keyword `selfclass` represents the actual type of the instance represented by `self`. Therefore, `selfclass` appearing in a class body may refer to the type of this class or a subclass of this class. Example:
```altro
class PointAtLeast2D
{
    a, b: int = 0;
    ctor(other: selfclass)   // selfclass denotes the class of `self`
    {
        a = other.a;
        b = other.a;
    }
    ctor() {}
}
```
Note that using `selfclass` has a stricter type checking rule than using the class name. Constdier:
```altro
class Point2D
{
    a, b: int = 0;
    ctor(other: Point2D)   // selfclass denotes the class of `self`
    {
        a = other.a;
        b = other.a;
    }
}
```
While the constructor `ctor(other: Point2D)` can accept an instance of any subclass of `Point2D`, the constructor `ctor(other: selfclass)` in the class `PointAtLeast2D` can accept an instance of the same type to the type of `self`. Consider:
```altro
sealed class Point2D is PointAtLeast2D
{
    ctor(other: selfclass) super(other) {}
    ctor() {}
}
class PointAtLeast3D is PointAtLeast2D
{
    c: int = 0;
    ctor(other: selfclass) super(other) { c = other.c }
    ctor() {}
}
sealed class Point3D is PointAtLeast3D
{
    ctor(other: selfclass) super(other) { }
    ctor() {}
}
p2  := Point2D();
p3  := Point3D();
p3a :  PointAtLeast3D = Point3D();
p_copy1 := Point3D(p2);           // Error: No matched IO interface is found to call Point3D
                                  // Note that the type p2 is not Point3D
p_copy2 := PointAtLeast3D(p3a);   // Error: No matched IO interface is found to call PointAtLeast3D
                                  // Note that the type p3a can be any subtype of PointAtLeast3D
p_copy3 := Point3D(p3);           // Okay because Point3D is sealed, the type of p3 is exactly Point3D 
```

## ownerclass

The keyword `ownerclas`s represents the actual type of the instance represented by `owner`.
