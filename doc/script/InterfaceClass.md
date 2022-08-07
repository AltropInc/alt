# Interface Class

A class can be declared as an interface class using the keyword `interface`. An interface class can only have member classes with deferred [functor](Functor.md) interfaces, although the keyword `deferred` is not required in an interface class.

cannot have any member object/value declaration. It cannot have any constructor or destructor either. An interface class can only have member classes with deferred virtual constructor interfaces, although the keyword `deferred` is not required: 

```altscript
interface class ButtonInterface
{
   func onButtonPress();
   func onButtonRelease();
};
```

An interface class can inherit multiple interfaces (multiple bases). However, an interface class cannot only be inherited by non-interface classes using `implements` clause:
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
