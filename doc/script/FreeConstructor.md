# Free Constructor

A free constructor is a [construction routine type](Routine.md) that does not belong to any class.  Because a constructor implicitly returns the initialize object from the enclosing class and a free constructor does not have any enclosing class, a free constructor must explicitly specify the output in its interface:

Here is an example of an argument declared as a free constructor:
```altscript
c: ctor(x: int...): Object;
```

Unlike an [free functor](FreeFunctor.md), a free constructor is always abstract. This is because a concrete constructor must always be declared in an enclosing class. We can also pass a class, a member class, or a meta member class to an argument declared as a free constructor.

## Pass a Class to Free Constructor

When we pass a class to an argument declared as a free constructor,  a constructor with the matched interfaces of the class is selected:
```altscript
class Foo
{
    num: int;
    ctor (x: int) { num = x }
    ctor (s: string) { num = s.length() }
}
c : ctor(x: int): Foo = Foo;  // constructor Foo(x:int) is selected
foo := c(40);                 // A Foo object is create and assigned to 'foo', using the constructor Foo(40)
```
When pass the class `Foo` to match a free constructor, in addition to the matched input interface, the output type of the free constructor must be the same or a superclass of `Foo`.

## Pass a Member Class to Free Constructor

When a class is a [member class](MemberClass.md), the first parameter in a constructor type must match the owner of the member class, and the rest parameters in the constructor match to the selected constructor interface of the member class. Example:

```altscript
class ShapeFactory
{
    class Shape {}             // A member class for the base of a concrete shape
    class Box: Shape           // A member class for the concrete shape Box
    {
        top_left: (double; double);
        bottom_right: (double; double);
        ctor (xys: (x:double; y:double)...) { top_left = xys[0]; bottom_right = xys[1] }
    }
    shape_creator: ctor(b: selfclass; xys: (x, y:double)...):Shape = Box;  // shape_creator is initialized by a Box constructor
    shape1 := shape_creator(self, ((1,2),(4,5)));  // A box is created in the 'self' object of the ShapeFactory
}
```

## Pass a Meta  Member Class to Free Constructor

If the member class is a meta member function class, no owner parameter should be involved in matching a constructor type interface because a meta member is defined in the class scope, not the object scope:
```altscript
class ShapeFactory
{
    meta class Shape {}         // A meta member class for the base of a concrete shape
    meta class Box: Shape       // A meta class for the concrete shape Box
    {
        top_left: (double; double);
        bottom_right: (double; double);
        ctor (xys: (x:double; y:double)...) { top_left = xys[0]; bottom_right = xys[1] }
    }
    shape_creator: ctor(xys: (x, y:double)...):Shape = Box;  // shape_creator is initialized by a Box constructor
    shape1 := shape_creator(((1,2),(4,5)));  // A box is created in the 'self' object of the ShapeFactory
}
```

