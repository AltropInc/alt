# Constructor

A constructor (abbreviation: ctor) is a special [routine type](Routine.md) (construction routine type) which called to create an object of the class that contains the constructor:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
```
The constructor in the class `Box` is a routine type specified after the key word `ctor`. It takes the input of the box origin, and the width and height of the box, and uses the input parameters to initialize the box by setting the values for the top-left and bottom-right corners of the box.

To use the constructor of a class to create an object, we give the class name and provide a list of expressions for the inputs (if any), thus the expression `Box((0,0), 2, 4)` creates an object of `Box` from the origin (0,0) with width 2 and height 4. This process is referred as `contructor call`.

The interface of a constructor does not specify output type, however, a constructor always returns an object of the enclosing class. Therefore, the constructor call `Box((0,0), 2, 4)` returns an object of `Box`:
```altscript
box := Box((0,0), 2, 4);
```

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
Only one default constructor is allowed in a class. If both the default constructor without any parameter and the default constructor with all the parameters having default values are provided, calling the default constructor without providing any input parameter will cause an error of ambiguity:
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
    ctor(top, left, bottom, right: double = 0,0,0,0) // This is another default constructor
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
}
box := Box();  // Error: Call of overloaded routine is ambiguous: Box
```

## Constructor Chaining

Constructor chaining occurs through inheritance when subclass constructor calls super class’s constructor:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
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

If the super class explicitly defines a constructor that is not the default, the subclass must call the constructor in the super:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
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
unless the super class provides a default constructor:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
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
    top_left: (double; ouble);
    bottom_right: (double; ouble);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box(0,0,0,0)  // The constructor ctor(top, left, bottom, right: double) in super is selected as
                                // the defulat constructor
{
    box_color: Color;
    ctor (color: Color) // Okay, Box(0,0,0,0) is automatically called as the default constructor in the super class
    {
        box_color = color;
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

## See also:
[Routine type](Routine.md)
[Abstract constructor](AbstractRoutineType.md)
[Constructor in simple class form](SimpleClassForm.md)
