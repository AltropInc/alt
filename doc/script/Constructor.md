# Constructor

A constructor (abbreviation: ctor) is a special [routine type](Routine.md) (construction routine type) which is called to create an object of the class that contains the constructor:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
```
The constructor in the class `Box` is a routine type specified after the key word `ctor`. It takes the input of the box origin, and the width and height of the box, and uses the input parameters to initialize the box by setting the values for the top-left and bottom-right corners of the box.

## Constructor Call

To use the constructor of a class to create an object, we give the class name and provide a list of expressions for the inputs (if any), thus the expression `Box((0,0), 2, 4)` creates an object of `Box` from the origin (0,0) with width 2 and height 4. This process is referred as `contructor call`.

The interface of a constructor does not specify output type, however, a constructor always implicitly returns an object of the enclosing class. Therefore, the constructor call `Box((0,0), 2, 4)` returns an object of `Box`:
```altscript
box := Box((0,0), 2, 4);
```
If the class of the constructor is a [member class](MemberClass.md), the owner for the constructor call must be given, and the owner type must be enclosing class of the member class. For instance,
```altscript
object test
{
    class Box     // 'Box' is member class
    {
        top_left: (double; double);
        bottom_right: (double; double);
        ctor (origin: (x:double; y:double); width, height: double)
        {
            top_left = origin;
            bottom_right = (origin.x + width, origin.y+ height);
        }
    }
    object BoxFactory
    {
        box1 := Box((0,0), 2, 4);        // Error: Member class is used with an object not in the owner type
        box2 := owner.Box((0,0), 2, 4);  // Okay
    }
    box3 := Box((0,0), 2, 4);            // Okay
}
```
Here in creating `box1`, the owner is not given, and the default owner is the object in the current context (`self`), which is in the type of `BoxFactory`. This is an error. In creating `box2`, the owner is given to the parent object of the object in the current context (`owner`),  which is in the type of enclosing class of `Box`. In creating `box3`, the owner is not given, and the default owner is the object in the current context which is in the same type of the enclosing class of the `Box`.

However, if the class of the constructor is a free class, the owner for the constructor call is always the object in the current context.

## Constructor Interface Overloading

More than one constructor can be used in a class, as long as each constructor has a different list of argument types. This concept is known as `Constructor Overloading`. While creating the object, input arguments must be passed to let the script parser know which constructor routine needs to be called. A constructor routine must be uniquely selected depending upon the number and type of input arguments passed, which must match the specified interface of the constructor. Example:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor()
    {
        top_left = (0,0);
        bottom_right = (0,0);
    }
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
empty_box := Box();        // use ctor()
box1 := Box(0, 0, 2, 4);   // use ctor(top, left, bottom, right: double)
box2 := Box((0, 0), 2, 4); // use ctor(origin: (x:double; y:double); width, height: double)
```
Each constructor must  have a different list of argument types. The argument names are not counted for the difference:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(top, left, width, height: double)          // Error: Same interface already implemented
    {
        top_left = (top, left);
        bottom_right = (top + bottom, left + right);
    }
}
```
If a constructor routine cannot be uniquely selected based on the provided input type, an error of ambiguity will be reported by the parser:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(top, left, bottom, right: int)
    {
        top_left = (top, left);
        bottom_right = (top, left);
    }
}
box1 := Box(0, 0, 2, 4);           // use ctor(top, left, bottom, right: int)
box2 := Box(0.0, 0.0, 2.0, 4.0);   // use ctor(top, left, bottom, right: double)
box3 := Box(0.0, 0.0, 2, 4);       // Error: Call of overloaded routine is ambiguous: Box
box4 := Box(0.0, 0, 2, 4);         // use ctor(top, left, bottom, right: int) because inputs has more integers
box5 := Box(0.0, 0.0, 2.0, 4);     // use ctor(top, left, bottom, right: double) because inputs has more doubles
box6 := Box(0l, 0l, 2l, 4l);       // Error: Call of overloaded routine is ambiguous: Box
                                   // because an implicit conversion of long int to int is equally as valid as an
                                   // implicit conversion of long int to double
box7 := Box(0, 0, 2l, 4l);         // use ctor(top, left, bottom, right: int)
```

## Default Constructor

A default constructor is the constructor that either has no input parameters:
```altscript
  class Box
  {
      top_left: (double; double);
      bottom_right: (double; double);
      ctor()    // This is the default constructor
      {
          top_left = (0,0);
          bottom_right = (0,0);
      }
  }
```
or, if it has parameters, all the parameters have default values:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double = 0,0,0,0) // This is the default constructor
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
}
```
Only one default constructor is allowed in a class. If both the default constructor without any parameter and the default constructor with all the parameters having default values are provided, it will cause an error of duplicated default constructor:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor ()  // This is the default constructor
    {
        top_left = (0, 0);
        bottom_right = (0, 0);
    }
    ctor(top, left, bottom, right: double = 0,0,0,0) //  Error: Default constructor is already defined
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
}
```

## Constructor Chaining

Constructor chaining occurs through inheritance when subclass constructor calls super class’s constructor:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
        super(origin, width, height)   // call the super class Box's ctor here
    {
        box_color = color;
    }
}
```
This ensures the initialization of the data members of the super class on the creation of a subclass object.

If the superclass defines a constructor that is not default and the superclass does not explicitly define a default constructor, the subclass must call the constructor in the super:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
         // Error: No default constructor in super class. Explicit super constructor call is required
    {
        box_color = color;
    }
}
```
But if the super class provides a default constructor:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
         //  Okay, the default constructor in the super class Box is called automatically
    {
        box_color = color;
    }
}
```
The subclass can also select one of the constructors in the super class as the default constructor:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (top, left, width, height: double)
    {
        top_left = (top, left);
        bottom_right = (top + width, left + height);
    }
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box(0,0,0,0)  // The constructor ctor(top, left, bottom, right: double) in super is selected as
                                // the defulat constructor for ColoredBox
{
    box_color: Color;
    ctor (color: Color) // Okay, Box(0,0,0,0) is automatically called as the default constructor in the super class
    {
        box_color = color;
    }
}
```

## Constructor Delegation

A class have multiple constructors that do similar things. Sometimes it is useful to make a constructor to call another constructor of the same class, for the purpose of avoiding repetitive code. This feature is called **Constructor Delegation**.
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor (top, left, width, height: double)
    {
        top_left = (top, left);
        bottom_right = (top + width, left + height);
    }
    ctor () self(0,0,0,0) {}
    ctor (origin: (x:double; y:double); width, height: double) self(origin.x, origin.y, width, height)
    {
    }
}
```altscript
The delegated constructor call starts with the keywoad`self` for the constructor in the same class. It is simular to constructor chaining where the keyword `super` is used for the constructor in the base class.

It is very important to note that constructor delegation is different from calling a constructor from inside the body of another constructor, which is not recommended because doing so creates another object and initializes it, without doing anything to the object created by the constructor that called it. Consider:
```altscript
object test
{
    class Box
    {
        top_left: (double; double);
        bottom_right: (double; double);
        ctor (top, left, width, height: double)
        {
            top_left = (top, left);
            bottom_right = (top + width, left + height);
        }
        ctor ()
        {
            owner.Box(0,0,0,0);  // a new Box instance is created in the enclosing object
        }
    }
}
```
Also, recursive constructor calls is considered an error:
```altscript
class foo 
{
    ctor(x: int);
    ctor() self(0)
    {
    }
    ctor(x, y: int);
    ctor(x: int) self (0,0)
    {
    }
    ctor(x, y: int) self()  // Error:  recursive constructor invocation is not allowed:
    {
    }
}
```

## Meta Constructor

A meta constructor is a `class construction routine type` used to initialize meta members of its enclosing class:
```altscript
  class Box
  {
      meta box_count : int;
      top_left: (double; double);
      bottom_right: (double; double);
      meta ctor()
      {
          box_count = 0;
      }
      ctor (origin: (x:double; y:double); width, height: double)
      {
          ++box_count;
          top_left = origin;
          bottom_right = (origin.x + width, origin.y+ height);
      }
  }
```
Meta members belong to the class scope. A meta constructor cannot have any input parameters in the interface, and it cannot be explicitly called.  One class can have only one meta constructor.

## Free Constructor

A [free constructor](FreeConstructor.md) is a [construction routine type](Routine.md) that does not belong to any class. Because a constructor is used to create instances from its enclosing class and a free constructor has no associated enclosing class, a free constructor is always abstract. It can only be used as a declared type for an argument, and the argument of a free constructor can be used only if it is bound to a concrete constructor that belongs to an enclosing class.

A typical reason for using a free constructor is to declared a polymorphic constructor argument, just like a polymorphic function argument where a [FreeFunctor.md](FreeFunctor.) is used, so the actual class used for creating an object through the argument will be determined at run time. See [Free Constructor](FreeConstructor.md) for more information.

## See also:
* [Routine type](Routine.md)
* [Free constructor](FreeConstructor.md)
* [Class](Class.md)
* [Inheritance](Inheritance.md)
* [Member class](MemberClass.md)
