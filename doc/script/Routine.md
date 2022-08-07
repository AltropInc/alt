# Routine Type and Routine Call

A `routine type` defines a callable unit that have an common input and output interface associated with a block of code used to take the inputs, carry out a sequence of actions, and generate the output, if any, as specified. This may be called [subroutine](https://en.wikipedia.org/wiki/Subroutine), function, [method](https://en.wikipedia.org/wiki/Method_(computer_programming)), or procedure. We emphasize type for this concept here because a `routine type` has instances. The instance of a routine type is the actual routine, the executable object, created to perform the task specified by the routine type. The process to create executable objects (routines) from a routine type is called `routine call`, where we provide a set of input values required by the routine type. A routine is not limited to a stateless executable like a program execution invoked by a subroutine call in the traditional concept. Many kinds of routines require states when created. For instance, a concurrent routine needs states to indicate its life stage (started, blocked, waiting, timed waiting, terminated, etc.). This is the reason that we need to treat this concept to be a type.

A routing type is specified by an input and output interface followed by a block of code. For example, the following routine type:
```altscript
(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
```
specifies a routine that takes the input of an integer stream, calculates the sum of the integer stream , and returns the sum as its output. The input is given in a pair of parentheses as in `(x: int...)`. The output type, if any, is given after the colon following the input. The sequence of actions is given by a code block which is a set of statements placed in a pair of curly brackets.

If a routine type is specified within a class, the class is the `enclosing class` of the routine type. Routine types specified in the class constitutes the behavior of the class as well the behavior of objects created from the class. There are two kinds of routine types: `construction routine type` and `function routine type`. A construction routine type, also referred as [constructor](Constructor.md), tells how the object of the class is initialized, and a function routine type,
also referred as [functor](Functor.md), tells what function that the object of the class can perform.

A routine type can also be free, which is not a member of any class. A free routine type can be passed as a first-class object that can be dynamically created, destroyed, passed to a function, returned as a value, and have all the rights as other variables have. See [Free Constructor](FreeConstructor.md] and [Free Functor](FreeFunctor.md] for details.

## Construction Routine Type (Constructor) and Constructor Call

A construction routine type, also referered as [constructor](Constructor.md), is a routine type prefixed with the keyword `ctor`. For example, a constructor in a `Point` class can be
```altscript
class Point
{
    ctor (x, y: double) { /* initialize x and y coordinates of the point using the input x and y */ }
}
```
The constructor in the class `Point` defines a callable unit that takes the input of x  and y coordinates to initialize the Point object. To create a construction  routine from the constructor to perform the initialization specified by the constructor, we provide the enclosing class and an expression for the input values. This instantiation process is referred to as [constructor call](Constructor.md):
```altscript
Point(0,0)
```
This creates a construction routine that initializes x and y coordinates of a `Point` object using the provided input (0,0). Since a construction routine type always returns the initialized object as its output, the construction routine type does not have the output type in its specification.

See [Constructor](Constructor.md) for more description.


## Function Routine Type (Functor) and Function Call

A function routine type, also referred as [functor](Functor.md), is a routine type prefixed with the keyword `fn`. Consider a functor defined in the class Σ:
```altscript
class Σ
{
    fn (x: int...): int { sum:=0; foreach(e in x) sum+=e; sum }
}
```
The functor in the class `Σ` defines a callable unit that takes the input of a stream, calculates the summary of the stream elements, and returns the value of the summary. To create a function routine from the functor to perform the task specified by the functor, we provide the object of the enclosing class and the expression for the input values. This instantiation process is referred to as `function call`:
```altscript
Σ()(1,2,3,4)
```
where `Σ()` is a constructor call to generate an object of Σ and then use the Σ object to create a function routine that takes the input (1,2,3,4) and calculates  the sum of the integers in the input. The expression `Σ()(1,2,3,4)` returns a value 10.

In some cases, a function call can use the class name directly, i.e. we can use `Σ(1,2,3,4)` instead of `Σ()(1,2,3,4)` for a function call. See [Functor](Functor.md) for more description.

## Terminology Summary

* **routine** an executable unit created from a routine type to perform the task specified
* **routine type** defines a callable unit with a input and output interface associated with a code block
* **free routine type** A routine type that is not a member of any class.
* **routine call**: an action to create an executable routine from the routine type.
* **constructor**: a [construction routine type](Constructor.md) defined to initialize an object.
* **functor**: a [function routine type](Routine.md) defined to perform a specific task.
* **construction call**: the action to use a constructor to create a construction routine to initialize an object.
* **function call**: an action to use a functor to create a function routine to perform the specified task.

## See also:
* [Constructor](Constructor.md)
* [Functor](Functor.md)
* [Free functor](FreeFunctor.md)
* [Free constructor](FreeConstructor.md)
* [Member Class](MemberClass.md)

