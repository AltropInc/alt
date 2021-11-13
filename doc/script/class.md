# Class

A class describes a set of objects that have a common structure and a common set of behaviours. The object belonging to the class is called an instance of the class. In ALT, a class is also referred to as a type. They are the same concept.

The object structure tells how the structural representation of the object is composed in terms of a set of member objects. While this definition is recursive, there are a set of primitive classes whose structural representations are built-in values without member objects. Examples are classes for integers, floating-point numbers, enumeration values, and characters. Take the example of a `ParticleSystem`, it contains a set of particles as its member objects. while each particle is composed of primitive values to represents its size, color, and other attributes:

```altscript
class Partcle
{
  size: int;
  color: Color;
  position: (float, float, float);
  life: duration;
}
class ParticleSystem
{
  particles: Particle...;
  emit_position: (float, float, float);
  emit_direction: (float, float, float);
}
```

The object behaviour tells how the object can act in response to a set of inputs in terms of a set of member classes. The class `ParticleSystem`, for example, may use a member class of `MyParticle` to describe the composition of its own specialized version of particle, and a functional member class `emit` (member function) to tell how a set of particles can be generated:

```altscript
class ParticleSystem
{
  class MyParticle is Partcle
  {
    enter (minmum_to_life_span, maximum_to_life_span: int)
    {
      // construct MyParticle
    }
  }
  class emit is func
  {
    enter (minmum_to_generate, maxmum_to_generate: int)
    {
      // generate partciles
    }
  }
  particles: MyParticle...;
  emit_position: (float, float, float);
  emit_direction: (float, float, float);
}
```

## Defining a Class

A class always starts with the keyword `class` followed by the name of the class:

```altscript
class ClassName
{
}
```

A class can be created or derived from an existing class. The existing class from which the derived class is created through the process of inheritance is known as a base class or superclass. A derived class is also known as subclass. The relationship between a derived class and a base class is an “is a” relationship. A subclass is a subtype of its base, i.e. any instance of a subclass is also an instance of its base. By using inheritance to derive a new class from its base, all of the members in the base can be reused to make the new class and the only thing needed to consider is the extension part of the new class and the existing part needs to be changed in the base. For example, consider a base class, Vertebrate , which is used to create two derived classes, Reptile and Mammal. Reptile is a Animal and Mammal is a Mammal. Both have few common features:  they have a backbone and are air-breathing.  But each type can have features that are unique to its specialization and are different from the features of the base class: mammals have hairs and are warm-blooded while reptiles have scales and are cold-blooded:

```altscript
class Vertebrate
{
  object Backbone: SpinalColumn {}
  func breathe (media: Air)
  {
    // move air into lungs to take in oxygen and give out carbon dioxide
  }
}
class Mammal: Vertebrate
{
  object Hairs: ThreadLikeBodyCovering {}
  func thermoRegulate(environment_temperature: float)
  {
    // adjust metabolism to maintain body temperature in a given range
  }
}
class Mammal: Vertebrate
{
  object Scales: FlattenedRigidBodyCovering {}
}
```

If a class can have only one instance, the class is a singleton class, and the unique instance is called singleton. The instance is defined together with its class. A singleton starts with the keyword `object` followed by the name of the singleton, as shown in the above example. `Backbone`, `Hairs`, and `Scales` are singletons. The definition of a singleton body is the same as the definition of a class body except for a few limitations, for example, the constructor of a singleton should have no input because the singleton is created in-place and its constructor cannot be called from the outside.

Also please note that the function `breathe` and `thermoRegulate` use a different format than the function `emit' used in the `ParticleSystem` example. The member class `breathe` and `thermoRegulate` use a simple form to define derived classes:

```altscript
BaseClassName DerivedClassName ConstructorInterface ConstructorBoday
```

Using this simple form,  the member class `emit` in `ParticleSystem` can be written as below:

```altscript
  func emit (minmum_to_generate, maxmum_to_generate: int)
  {
    // generate partciles
  }
```

where `func' is the base class, `emit` is the derived class, `(minmum_to_generate, maxmum_to_generate: int)` is the constructor interface, and `{ }` is the constructor body. We will discuss this simple form in detail later.

## Constructor and Destructor

Constructors defined in a class are used in the process of creating instances of the class. A class can specify a set of constructor interfaces with different inputs to create the instance of the class. A constructor is introduced by the keyword `enter`:

```altscript
type position = (x, y: float);
type area_size = (width, height: float);
class Rectangle
{
  enter(origin: position; size: area_size) { }
  enter(top_left, bottom_right: position) { }
  enter(x, y, width, height: float) { }
}
```

A default constructor is the constructor which doesn't take any input. If you define any constructor explicitly in a class, then you will have to call one of the constructors with matched inputs to initialize the instance of the class, including a constructor in all of its base classes. For example:

```altscript
class Base
{
  enter(x: int) { }
}
class Derived: Base
{
  enter(x, y: int) { }  // <- Error: No default constructor in base class. Explicit base constructor call is required
}
```

Because the `Base` class explicitly defines a constructor that takes an integer value as input, the `Derived' must call the constructor in the base unless the `Base' provides a default constructor. Either,

```altscript
class Base
{
  enter(x: int) { }
  enter() { }
}
class Derived: Base
{
  enter(x, y: int) { }  // <- okay, the default constructor in the base is called automatically
}
```

or

```altscript
class Base
{
  enter(x: int) { }
}
class Derived: Base
{
  enter(x, y: int) super(x) { }  // <- okay, the constructor that takes an integer value in the base is called explicitly
}
```

The base class can also select one of the constructors in base as the default constructor:

```altscript
class Base
{
  enter(x: int) { }
}
class Derived: Base(0)        // <- the constructor that takes an integer value 0 in the base is used as the default constructor
{
  enter(x: int) super(x) { }  // <- okay, the constructor in the base is called explicitly and takes the input value given by x
  enter(x, y: int) { }        // <- okay, the selected default constructor in the base is called automatically and takes the input value 0
}
```

If a derived class does not have any member extension or modification and only provide constructors to create instances of the class, the class definition can use a simple form for each constructor interface. For instance:

```altscript
class Base
{
}
class Derived: Base
{
  enter(x: int) { }
  enter(x, y: int) { }
  enter(x, y, z: int) { }
}
```

Only constructors are provided in the `Derived` class. Therefore, we can use set of a simple forms for the `Derived` class:

```altscript
class Base
{
}
Base Derived(x: int) { }
Base Derived(x, y: int) { }
Base Derived(x, y, z:: int) { }
```

In particular, a sealed class is a class that can only be used to derive subclasses with new constructors. Therefore, we can always use simple forms to derive classes from a sealed base class.

The destructor, on the other hand, is used to destroy the class instance. A destructor is introduced by the keyword `exit`:

```altscript
class ClassName
{
  exit() {}
}
```

The destructor should not have any input, and is automatically invoked, including all destructors in bases classes, when the instance of the class is in destroy. The destructor in the derived class is invoked first, then, the one defined in the base.

## Functional Class

A functional class is used to create callable objects that perform some functionality described by its constructor body. When a callable object finishes its performance, its duties are done and it is deleted. Examples of such instantiations are a procedure to open a file, a process to display an image, a thread to deal with an event, a scientific calculation, and an activity to generate a signal. The interface of the constructor of a functional class can have both inputs and output. If the interface has output, an output object or value is generated. Consider:

```altscript
class sum: func
{
  enter (values: int...): int
  {
    sum := 0;
    foreach (v in values) sum += v;
    return sum;
  }
  enter (strs: string...): string
  {
    sum: string;
    foreach (str in strs) sum.append(str);
    return sum;
  }
}
```

The base class `func' is a built-in functional class that supports traditional function call protocol, which is an efficient and sequential procedure using the same stack frame technique. Because `func' is a sealed class, we can use simple class form to define function classes:

```altscript
func sum(values: int...): int
{
  sum := 0;
  foreach (v in values) sum += v;
  return sum;
}
func sum(strs: string...): string
{
  sum: string;
  foreach (str in strs) sum.append(str);
  return sum;
}
```

## Member Class Overriding

A member class can have a virtual constructor interface defined in the base class that can be overridden in a derived class. When you refer to a derived class object using a variable declared in the base class, you can use the virtual constructor interface for that object and execute the overridden version of the constructor in derived class to create a member object:

```altscript
class base
{
  class member
  {
    virtual enter(x: int) {}
  }
}
class derived: base
{
  class member
  {
    virtual enter(x: int) {}
  }
}
```

When a variable of the base class, `x`, refers to a derived class object:

```altscript
x: base = derived();
x.member(3);
```

`x.member(3)` creates a member object using the member class declared in the `derived` class.

The virtual constructor interface can only be used for member classes, and the overridden process is done through their enclosing class hierarchy. The ALT virtual constructor interface is more general than the virtual functions in other traditional programming languages. When member classes are written in simple forms, the virtual constructor interface looks exactly like the traditional virtual functions:

```altscript
class base
{
  virtual func print()
  {
     // print base class information
  }
};
class derived: base
{
  virtual func print()
  {
     // print derived class information
  }
}; 
```

## self, selfclass, owner, ownerclass

The keyword `self' is the reserved object name used within a class body to represent the actual instance created by this class or by a derived class of this class. It is similar to Java's `this' reference or C++'s `this` pointer. However, there is a major distinction: `self' appearing in a function body refers to the instance of the function instead of the instance of the class that encloses the function. The reason for this distinction is that ALT treats a member function the same as a member class. The function body is just a simple form of member class. To access the instance of the class that encloses the function within the function body, use the keyword `owner', which is the reserved object name to represent the enclosing object of the `self' instance.

The keyword `selfclass` represents the actual type of the instance represented by  `self'. Therefore, `selclass` appearing in a class body may refer to the type of this class or a subclass of this class. The keyword `ownerclass` represents the actual type of the instance represented by `owner'.




