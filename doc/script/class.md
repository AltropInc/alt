# Class

A **class** describes a set of objects that have a common structure and a common set of behaviours. The object belonging to the class is called an **instance** of the class. A class in Altro is also referred to as a type. They are the same concept. A class always starts with the keyword `class` followed by the name of the class:
```altscript
class ClassName
{
}
```
The object structure tells how the structural representation of the object is composed in terms of a set of member object/value declarations. While this definition is recursive, there are a set of primitive classes whose structural representations are built-in values without member object/value declarations. Examples are classes for integers, floating-point numbers, enumeration values, and characters. Take the example of a `ParticleSystem`, it contains a set of particles and each particle is composed of primitive values to represents its size, color, and other attributes:

```altscript
class ParticleSystem
{
    particles: (size: float; color: Color; position: (x,y,z:float);  life: float)...;
    emit_position: (x,y,z:float);
    emit_direction: (x,y,z:float);
}
```
The object behaviour tells how the object can act in response to a set of inputs in terms of a set of member funcions. A member funcions is a function class nested in its enclosing class. The class `ParticleSystem`, for example, has a function class `emit` (member function) to tell how a set of particles can be generated:
```altscript
class ParticleSystem
{
    particles: (size: float; color: Color; position: (x,y,z:float);  life: float)...;
    emit_position: (x,y,z:float);
    emit_direction: (x,y,z:float);
  
    func emit(minmum_to_generate, maxmum_to_generate: int) : int
    {
       // generate some partciles and putthem in 'particles' 
       // returns number of particles generated
    }
}
```
Using the class, we can create objects of the class:
```altscript
ps := ParticleSystem();  // A ParticleSystem object is created and referred by the name 'ps'
```
Then, we can call member funcions of the object to activate some behavior within the object:
```altscript
generated := ps.emit(1,10);  // Let the ParticleSystem object generate 1 to 10 particles
```

##  Inheritance and Subclasses

Every time we introduce a new class of objects, we do not want to describe all the structure and behaviours of each object from scratch. We would like to have a mechanism to build a new class on a existing class so that te new class contains all the features of the existing class in addition to its own. This mechanism is called **inheritance**. The class which is newly created is known as the **subclass** or **derived class** of the existing class on which the new class is based. and the existing class is the **superclass** or **base class** of the newly created class. The relationship between a derived class and a base class is an [**is-a** relationship](https://en.wikipedia.org/wiki/Is-a). A subclass is a **subtype** of its base, i.e. any instance of a subclass is also an instance of its base. The following code establishes an explicit inheritance relationship between classes B and A:
```altscript
class A { }
class B is A { }   // or class A: B { }
```
By using inheritance to derive a new class from its base, all of the members in the base can be reused to make the new class and the only thing needed to consider is the extension part of the new class and the existing part needs to be changed (overridden) in the base. Consider the following pseudo code, a base class, Vertebrate, used as a base class to create two derived classes, Reptile and Mammal. Reptiles and mammals have few common features as vertebrates:  they have a backbones and are air-breathing.  But each type can have features that are unique to its specialization and are different from the features of the base class: mammals have hairs and are warm-blooded while reptiles have scales and are cold-blooded:
```altscript
class Vertebrate
{
    backbones: SpinalColumns;
    func breathe (media: Air)
    {
       // move air into lungs to take in oxygen and give out carbon dioxide
    }
}
class Mammal is Vertebrate
{
    hairs: ThreadLikeBodyCoverings;
    func thermoRegulate(environment_temperature: float)
    {
        // adjust metabolism to maintain body temperature in a given range
    }
}
class Reptile is Vertebrate
{
    scales: FlattenedRigidBodyCoverings;
}
```
See [**Class Inheritance**](Inheritance.md) for more description, including multiple inheritance and member class inheritance.

##  Member Classes

A class defined within a class or a singleton is called a [**member class**](MemberClass.md). While subclasses via inheritance establish an **is-a** relationship, member classes via composition defines a [**has-a** relation](https://en.wikipedia.org/wiki/Has-a) in which instances of the member class "belongs to" (is part or member of) an instance of its enclosing class. The member class is known as **child class** of its enclosing class and the enclosing class is the **owner class** or **parent class** of the member class.  The instance of a member class is known as **child object** of the instance of its enclosing class and the instance of the enclosing class is its **owner** or **parent**.

Let us revist the `ParticleSystem` example, particles contained in a particle system can be decribed by a memmber class `Particle`:
```altscript
class ParticleSystem
{
    class Particle
    {
        size: float;
        color: Color;
        position: (x,y,z:float);
        life_span: float;
        age: float;
    }
}
```
When we create a particle via the member class `Particle`, we need to provide the owner of the particle, and in this case, the owner must be an instance of the `ParticleSystem`:
```altscript
ps := ParticleSystem();  // A ParticleSystem object is created and referred by the name 'ps'
p := ps.Particle();      // Create a particle inside the ParticleSystem object referred by the name 'ps'
```
Particles are created as child objects inside an instance of `ParticleSystem`.

It is quite often the case that we need to mix inheritance and composition. Here is an example:
class SplitParticleSystem is ParticleSystem
{
    class SplittableParticle is Particle
    {
        split_age: float;
    }
}
```

See [**Member Class**](MemberClass.md) for more description.

## Singleton

If a class can have only one instance, the class is a singleton class, and the unique instance is called singleton. The instance is defined together with its class. A singleton starts with the keyword `object` followed by the name of the singleton, as shown in the above example. `Backbone`, `Hairs`, and `Scales` are singletons. The definition of a singleton body is the same as the definition of a class body except for a few limitations, for example, the constructor of a singleton should have no input because the singleton is created in-place and its constructor cannot be called from the outside.

## Constructor and Destructor

Constructors defined in a class are used in the process of creating instances of the class. A class can specify a set of constructor interfaces (interface overloading) with different inputs to create the instance of the class. A constructor is introduced by the keyword `enter`:

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

Because the `Base` class explicitly defines a constructor that takes an integer value as input, the `Derived` must call the constructor in the base unless the `Base' provides a default constructor. Either,

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

The destructor, on the other hand, is used to destroy the class instance. A destructor is introduced by the keyword `exit`:

```altscript
class ClassName
{
  exit() {}
}
```

The destructor should not have any input, and is automatically invoked, including all destructors in bases classes, when the instance of the class is in destroy. The destructor in the derived class is invoked first, then, the one defined in the base.

<h3 id="Simple-Class-Form-and-Sealed-Class">Simple Class Form and Sealed Class</h3>

If a derived class does not have any member extension or modification and only provides constructors to create instances of the class, the class definition can use a simple form for each constructor interface. For instance:

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

Only constructors are provided in the `Derived` class. Therefore, we can use set of a simple form for the `Derived` class:

```altscript
class Base
{
}
Base Derived(x: int) { }
Base Derived(x, y: int) { }
Base Derived(x, y, z:: int) { }
```

In particular, a sealed class is a class that cannot be extended with new members in derived classes. However, new constructors with different interfaces can be added to a sealed class. Therefore, we cannot use a sealed class as a base class in class inheritance, but we can use simple class form to generate derived class from a sealed base class with a set of different constructors:

```altscript
sealed class SealedBase
{
    y: int;
    z: int;
}
class Derived: SealedBase           // Error: Sealed class cannot be inherited
{
    enter(a: int) { y=a; z=a; }
    enter(a, b: int) { y=a; z=b; }
}
SealedBase Derived (a: int) { y=a; z=a; }      // okay
SealedBase Derived (a, b: int) { y=a; z=b; }   // okay
```

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

The above code cannot be compiled because the base class `func` is sealed class. We can only use simple class form to define function classes:

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

The class `func` a built-in functional class that supports traditional function call protocol, which is an efficient and sequential procedure using the same stack frame technique.

## Member Interface Overriding, Deferred Member Interface, and Abstract Class

A member class can have a virtual constructor interface defined in the base class that can be overridden in a derived class. When you refer to a derived class object using a variable declared in the base class, you can use the virtual constructor interface for that object and execute the overridden version of the constructor in derived class to create a member object.

The virtual constructor interface can only be used for member classes derived from a sealed class, and the overridden process is done through their enclosing class hierarchy. Since the member class is derived from a sealed class, only simple class form can be used for virtual member interface:

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

A member class can have a deferred virtual constructor interface, which is a virtual constructor interface without the implementation block:

```altscript
class abstract_base
{
  deferred func print();
};
class derived: abstract_base
{
  func print()
  {
     // print derived class information
  }
};
```

An abstract class is a class that cannot be instantiated, but they can be subclassed. If a class contains a member class with any deferred constructor interface, this class is an abstract class. If its derived class does not override this deferred constructor interface with an implementation block, the derived class is an abstract  class too. In the above example, the class `abstract_base` and `abstract_derived` are abstract classes, and only the class `derived` can be used to instantiate objects. A class can also be declared abstract using the keyword `abstract` even though it does not have any member classes with deferred constructor interfaces. In addition, a parametric class (see [Parametric Class](@ref man-parametric-classes)) is also an abstract class. In summary, a class is abstract if

 * It contains a member class with any deferred constructor interface or inherits such interface that is not overriden
 * It is declared abstract using the keyword `abstract`.
 * It is a parametric class (see [Parametric Class](@ref man-parametric-classes))

The ALT virtual constructor interface is more general than the concept of virtual functions in other traditional programming languages, because the overriding mechanism is not just limited to functions:

```altscript
sealed class sealed_base
{
}
class base
{
    deferred sealed_base member (x: int);
}
object derived is base    // a singleton derived from base
{
    sealed_base member(x: int)
    {
    }
}
x: base = derived;   // a polymorphic variable of 'base' and it actually refers to the 'derived' object
enter ()
{
    x.member(3);     // create a 'member' object in the 'derived' object using the constructor provided in the 'derived' object
}
```

## Interface Class and Its Implementation

A class can be declared as an interface class using the keyword `interface`. In addition, an interface class cannot have any member object/value declaration. It cannot have any constructor or destructor either. An interface class can only have member classes with deferred virtual constructor interfaces, although the keyword `deferred` is not required: 

```altscript
interface class ButtonInterface
{
   func onButtonPress();
   func onButtonRelease();
};
```

The interface of member class (function) `onButtonClick` is deferred and does not have implementation. An interface class is an abstract class because all member class constructor interfaces are deferred.

An interface class can inherit multiple interfaces (multiple bases). However, an interface class cannot be inherited by non-interface derived classes, it can only be implemented by non-interface derived classes:

```altscript
class Drawable2DButton is Drawable2D implements ButtonInterface
{
   func onButtonPress()
   {
   }
   func onButtonRelease();
   {
   }
};
class Drawable3DButton is Drawable3D implements ButtonInterface
{
   func onButtonPress()
   {
   }
   func onButtonRelease();
   {
   }
};
```

A derived class can have only one base class by inheritance but can implement multiple interfaces. The derived class is a subclass of its base class, and also a subclass of any interface class that is implemented. `Drawable2DButton` is a subclass of `Drawable2D` and is also a subclass of `ButtonInterface`.

Example of an derived object (singleton) that inherits one base and implements two interfaces:

```altscript
sealed class sealed_base
{
    y: int;
    z: int;
}
interface class interface1
{
    deferred sealed_base member1 (x: int)
}
interface class interface2
{
    deferred sealed_base member2 (x: int)
}
object derived is Drawable implements interface1, interface2
{
    sealed_base member1(x: int)
    {
        y = x;
    }
    sealed_base member2(x: int)
    {
        z = x;
    }
}
x1: interface1 = derived;
x2: interface2 = derived;
enter ()
{
    x1.member1(3);
    x2.member2(4);
}
```


## self, selfclass, owner, ownerclass

The keyword `self` is the reserved object name used within a class body to represent the actual instance created by this class or by a derived class of this class. It is similar to Java's `this` reference or C++'s `this` pointer. However, there is a major distinction: `self` appearing in a function body refers to the instance of the function instead of the instance of the class that encloses the function. The reason for this distinction is that ALT treats a member function the same as a member class. The function body is just a simple form of member class. To access the instance of the class that encloses the function within the function body, use the keyword `owner`, which is the reserved object name to represent the enclosing object of the `self` instance.

The keyword `selfclass` represents the actual type of the instance represented by  `self`. Therefore, `selfclass` appearing in a class body may refer to the type of this class or a subclass of this class. The keyword `ownerclass` represents the actual type of the instance represented by `owner`.




