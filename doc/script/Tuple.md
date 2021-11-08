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

A simple form can be used for a class that does not introduce more members. As in the above example, member classes `MyParticle` and `emit` can use the simple form as shown below:

```altscript
class ParticleSystem
{
  Partcle MyParticle (minmum_life_span, maximum_life_span: int)
  {
    // construct MyParticle
  }
  func emit (minmum_to_generate, maxmum_to_generate: int)
  {
    // generate partciles
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

A class class can be created or derived from a existing class. The existing class from which the derived class is created through the process of inheritance is known as a base class or superclass:

```altscript
class DerivedClassName is ClassName
{
}
```

or

```altscript
class DerivedClassName: ClassName
{
}
```

By using a base class to derive a new class, all of the members in the base can be reused to make the new class and the only thing need to care is the extension part the new class and the existing part needs to be chnaged in base.

A class can specify a set of constructor interfaces to use different inputs to create the instance of the class:

```altscript
type position = (x, y: float);
type area_size = (width, height: float);
class Rectangle
{
  enter(origin: position; size: area_size) { }
  enter(pos1, pos2: position) { }
  enter(x, y, width, height: float) { }
}
```

## Tuple type

A tuple is a container of fixed number of heterogeouns elements arranged in a specified order that cannot be changed.

The tuple elements can be either named or unamed, but all tuple elements must be ether all named or all unnamed.
A tuple is written in a list of element types with round brackets, for instance:

```altscript
(int; string; double)
(position: (x, y: int); height: int);
```

A tuple type can be parameterized. The list of type paramters is written with round brackets prefixex with #. For instance,

```altscript
#(type ElementType: Numeric)(first, second: ElementType)
```
A paramterized tuple type is often used to specify the type dependeny among tuple elements. In the above example, the type of the first
and the second elements are related. Althong it tells you that they can be any numeric types, but once the ElementType is bound to
a concrete numeric type, say, int, they has to be both in int. Example:

```altscript
#(type ArrayType: array)(first: second: ArrayType; second: ArrayType.element_type, )
```
Here the type of the second tuple element must be the element type of the array type for the first tuple element. Consider another example:

```altscript
#(type T1, T2: Any, size: int)(first: array#(T1, size); second: array#(T2, size))
```

Here the second array is the tuple must have the same size of the first array.

A paramterized tuple type is an absctact type if any of its type parameters are unbound. Therefore, a paramterized tuple type
can be used to declare a polymorphic variable but cannot used to instantiate a tuple value. Example:

```altscript
type MyTupleType = #(type ArrayType: Array)(first: Array.ElementType, second: Array);
my_tuple: MyTupleType;          // my_tuple is a polymophic tuple which currently has a null value
my_array: int[5] = (1,2,3,4,5);
my_tuple = (my_array.first(), my_array);  // my_tuple has a value of type (int, Array#(int,5))
```
## Tuple Subtype

When we pass a tuple by reference, the declared type of the passed reference must be a subtype of the
decalred type of the reference that takes the passed reference. By default, a tuple is passed bt reference.

Tuple types cannot be covariant in their element types but can be covariant in their type paramters.
For instance, (int[5], int[5]) cannot be a subtype of (array, array), but can be a subtype of
#(type T: array)(T, T).

Tuple T1 is a subtype of tuple T2 if for each element ai ∈ T1, and the element of the same order bi ∈ T2,
let tai be the decalred type of ai, and tbi e the decalred type of bi, tai and tbi satisfy one of the following cases

Case (1): tbi evaluates to a bound type parameter and tai is an equivalent type to tbi. For instance,

```altscript
type T1 = tuple(int) 
type T2 = tuple#(type T=int)(x:T)  // T1 is subtype of T2
type T3 = tuple(integral) 
type T4 = tuple#(type T=integral)(x:T)  // T3 is subtype of T4, but T1 is not a subtype of T4.
```
The reason why T1 cannot be a subtype of T4 is shown in the following example:

```altscript
n: T1 = {0};
m: T4 = x;     // error: the type of n is not a subtype T4
m.x = 3.14;   // if we allow  m to refer to the value n which is the type of (int), this assigement will make n value invalid.
```
Case (2) tbi evaluates to a unbound type parameter and tai also evaluates to a type parameter, the type paramter must have the same name,
and the constraining type of the type parameter evaluated by tai must be a subtype of the constraining type of the type parameter evaluated by tbi.
For instance:

```altscript
type T1 = tuple#(type T: integral)(x:T, y:T)
type T2 = tuple#(type T: numeric)(x:T, y:T)  // T1 is a subtype of T2
type T3 = tuple#(type T=integral)(x:T, y:T)  // T3 is a subtype of T2
type T4 = tuple#(type T1: integral, type T2: integral)(x:T1, y:T2)  // T4 is not a subtype of T2
```
Case (3) tbi evaluates to a type. tai must evaluates to the same type. For instance:

```altscript
type T1 = tuple (int, string)
type T2 = tuple (int, string)  // T1 is a subtype of T2
```

Case (4) tbi is a type expression T.T1.T2..., where T is a type parameter. This is a depdent type case, so T must be used by a
previous tuple element in T1 in case (1) or (2), say, the previous tuple element in T1 is bj and its declared type is tbj. Substitute T with tbj
in type expression T.T1.T2..., we get tbj.T1.T2..., and let s be the type evaluated by the expression tbj.T1.T2..., substitute tbi with s and
check against tai, it needs to satisfy one of the cases from 1 to 3. For instance:

```altscript
type T1 = tuple (x:array#(int,5), y:int)
type T2 = tuple #(type T: array)(x:T, y:T.ElementType)   // T1 is a subtype of T2
type T3 = tuple (x:array#(int,5), y:double)   // T3 is not a subtype of T2
```

To understand case (4), let us check T2 in the above example: y in t2 is a type T.ElementType, which is a depdent type. T is used by the previous
tuple element x in T1 whose type is array#(int,5)  and array#(int,5).ElementType evaluates to int, then check the declared type y of T1 agains int
which is a match.

## Tuple Convertable Rule

When we pass a tuple by value, the passed value is not necessarily be a subtype, instead, it can any any type that is convertable
to the tuple value. for inatance, the value ((1,2,3,4), 3.14) in the type of ((int, int, int, int), double) is convertable to
tuple #(type T: array)(x:T, y:T.ElementType) and can be passed to a object that accepts the interface defined by tuple #(type T: array)(x:T, y:T.ElementType).

The oject interface is map from a tuple value (input) to an output. The oject interface always use the tuple convertible rules to take the input value.

A value V is convertable to tuple type T, if

(1) V is a array or stream whole element type can be coverted to the type of all tuple elelemnts. Note, string is a special case of stream.
(2) V is tuple value, and for each tuple element e in T, if e does not have initial (default) value, there must be a correspondent element e' in V
such that the type of e' is convertable e type if e is defined as pass-by-value and the type of e' is subtype of e type if e is defined as pass-by-reference .


