# Class

A **class** describes a set of objects that have a common structure and a common set of behaviors. The object belonging to the class is called an **instance** of the class. A class in Altro is also referred to as a type. They are the same concept. A class always starts with the keyword `class` followed by the name of the class:
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
The object behavior tells how the object can act in response to a set of inputs in terms of a set of member functions. A member functions is a function class nested in its enclosing class. The class `ParticleSystem`, for example, has a function class `emit` (member function) to tell how a set of particles can be generated:
```altscript
class ParticleSystem
{
    particles: (size: float; color: Color; position: (x,y,z:float);  life: float)...;
    emit_position: (x,y,z:float);
    emit_direction: (x,y,z:float);
  
    func emit(minmum_to_generate, maxmum_to_generate: int) : int
    {
       // generate some particles and put them in 'particles' 
       // returns number of particles generated
    }
}
```
Using the class, we can create objects of the class:
```altscript
ps := ParticleSystem();  // A ParticleSystem object is created and referred by the name 'ps'
```
Then, we can call member functions of the object to activate some behavior within the object:
```altscript
generated := ps.emit(1,10);  // Let the ParticleSystem object generate 1 to 10 particles
```

## Inheritance and Subclasses

Every time we introduce a new class of objects, we do not want to describe all the structure and behaviors of each object from scratch. We would like to have a mechanism to build a new class on an existing class so that the new class contains all the features of the existing class in addition to its own. This mechanism is called **inheritance**. The class which is newly created is known as the **subclass** or **derived class** of the existing class on which the new class is based. and the existing class is the **superclass** or **base class** of the newly created class. The relationship between a derived class and a base class is an [**is-a** relationship](https://en.wikipedia.org/wiki/Is-a). A subclass is a **subtype** of its base, i.e. any instance of a subclass is also an instance of its base. The following code establishes an explicit inheritance relationship between classes B and A:
```altscript
class A { }
class B is A { }   // or class A: B { }
```
By using inheritance to derive a new class from its base, all of the members in the base can be reused to make the new class and the only thing needed to consider is the extension part of the new class and the existing part needs to be changed (overridden) in the base. Consider the following pseudo code, a base class, Vertebrate, used as a base class to create two derived classes, Reptile and Mammal. Reptiles and mammals have few common features as vertebrates:  they have backbones and are air-breathing.  But each type can have features that are unique to its specialization and are different from the features of the base class: mammals have hairs and are warm-blooded while reptiles have scales and are cold-blooded:
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

Let us revisit the `ParticleSystem` example, particles contained in a particle system can be described by a member class `Particle`:
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
```altscript
class SplitParticleSystem is ParticleSystem
{
    class SplittableParticle is Particle
    {
        split_age: float;
        max_split_number, min_split_number: int;
    }
}
```

See [**Member Class**](MemberClass.md) for more description.


## Constructor and Destructor

Constructors defined in a class are used in the process of creating instances of the class. A class can specify a set of constructor interfaces (interface overloading) with different inputs to create the instance of the class. A constructor is introduced by the keyword `ctor`:

```altscript
type position = (x, y: float);
type area_size = (width, height: float);
class Rectangle
{
    ctor(origin: position; size: area_size) { }
    ctor(top_left, bottom_right: position) { }
    ctor(x, y, width, height: float) { }
}
```

For a complete description of the constructor, see [Constructor](Constructor.md).

The destructor, on the other hand, is used to destroy the class instance. A destructor is introduced by the keyword `dtor`:

```altscript
class ClassName
{
   dtor() { /* code for exit */ }
}
```

The destructor should not have any input, and is automatically invoked, including all destructors in base classes, when the instance of the class is destroyed. The destructor in the derived class is invoked first, then, the one defined in the base.

For a complete description of the destructor, see [Destructor](Destructor.md).

## Declaring Classes

A class declaration consists of a [class header](ClassHeader.md) and a [class body](ClassBody.md) class body. The class header may contain the class modifiers , the class name, the class parameters, the base class, the default constructor in the base to call, and a list of interfaces to be implemented. Here is a sample of class declaration:
```altscript
sealed class MyClass#(type ClassParameter1: any; ClassParameter2: int) is MyBaseClass(x, y) implements MyInterface1, MyInterface2
{
    // class members, constructors, functors and destructor declared in class body
}
```
In the class header,  `sealed` is a [class modifier](ClassModifier.md); `MyClass` is the class name; `ClassParameter1` is a type [class parameter](ParametricClass.md); `ClassParameter2` is a constant value [class parameter](ParametricClass.md); `MyBaseClass` is the [superclass](Inheritance.md) from which this class is derived; `(x, y)` are two parameters used to call one of the constructors in `MyBaseClass`([Selected default constructor](Constructor.md)); `MyInterface1` and `MyInterface2` are two [interface classes]InterfaceClass.md) that this class implements - so you can see that the class header, which is the opening line of the class declaration, can become quite complicated. For the moment, we'll not worry about these extra complications.

The class body is the area between a pair of curly brackets in which a list of [class members](ClassMember.md), [constructors](Constrcutors.md), [functors](Functor.md)  and a [destructor](Destructor.md). For a complete description of a class body, see [Class Body](ClassBody.md).






