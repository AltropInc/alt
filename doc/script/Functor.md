# Functor

A functor (abbreviation: fn) is a [function routine type](Routine.md) which is called to accomplish a specific task by processing the input data and generating a result, if any, in the output type as specified. A functor, unless it is abstract, is always specified within a class, and the class is called the `enclosing class` of the functor. A functor starts with the key word `fn`, followed by an input and output interface, and then a block of code. Consider:
```altscript
class Σ
{
    fn (x: int...): int { sum:=0; foreach(e in x) sum+=e; sum }
}
```
The functor in the class `Σ` is a routine type specified after the key word `fn`. It takes the input of a stream, and calculates the summary of the stream elements, and returns the value of the summary.

## Function Call

A `function call` is to use a functor of a class to create a function routine to perform the specified task. To do this, we give the object of the enclosing class and provide a list of expressions for the inputs (if any), thus the expression:
```altscript
Σ()(1,2,3,4)
```
is a function call where `Σ()` is a constructor call to to generate an object of Σ and the Σ object is used to create a function routine that takes the input (1,2,3,4) and calculate the sum of the integers in the input. The expression `Σ()(1,2,3,4)` returns a value 10.

## Function Call Using Class Name

In some cases, a function call can use the class name directly.

One case is when the enclosing class is a [stateless class](StatelessClasse.md). A stateless class defines only functors and meta members. It has no members oror constructors defined in the object scope. Therefore  we do not need to create an object of the class to perform the function call. For instance:
```altscript
class Σ implements functional   // Σ is a subclass that implements the functional interface
{
    fn (x: int...): int { sum:=0; foreach(e in x) sum+=e; sum }
}
sum := Σ(1,2,3,4);  // the function call uses the class name Σ directly
```
Here the class Σ is a stateless class and it implements the interface [functional](Interfacefunctional.md). The expression of the function call `Σ(1,2,3,4)` uses the class name `Σ`, instead of `Σ()`.

Another case is when the provided input parameters do not match any constructor routine type but a matched function routine type is found. In this case, an object of the class is automatically created to perform the function call. In order to make this possible, the class must provide a [default constructor](DefaultConstructor.md). Example with an explicit default constructor:
```altscript
class Σ
{
    multiple : int;
    fn (x: int...):int { sum:=0; foreach(e in x) sum += e * multiple; sum }
    ctor() { multiple = 1 }
}
msum := Σ(1,2,3,4);
```
Example with implicit default constructor:
```altscript
class Σ
{
    multiple : int = 1;
    fn (x: int...):int { sum:=0; foreach(e in x) sum += e * multiple; sum }
}
msum := Σ(1,2,3,4);  // msum gets the value 80
```
In either of the above examples, the object of `Σ` is created automatically using the default constructor in the expression `Σ(1,2,3,4)', therefore, the expression  `Σ(1,2,3,4)` is equivalent to `Σ()(1,2,3,4)`. `Σ` is not an immaterial class because it has a data member `multiple`. An object of `Σ` must be created in order to complete the function call to `fn (x: int...):int`. 

In case of both functor and contructor provides the same interface:
```altscript
class Σq
{
    coefficients : int[4];
    fn (x: int[4]):int { sum:=0; for (i:=0; i<4; ++i) sum += x[i] * coefficients[i]; sum }
    ctor(c: int[4]) { coefficients = c }
}
```
The expression of a function call Σq(1,2,3,4) can cause an ambiguity error because the parser does not know whether a constructor call or a function call should be used. To avoid the ambiguity error, the keyword `ctor` can be used to let the parser know that a constructor call is used, or the keyword `fn` can be used to let the parser know that a function call is used:
```altscript
Σq ctor(1,2,3,4) fn(1,2,3,4)
```
In this case, the keyword `fn` is not necessary, because `Σq ctor(1,2,3,4)` is an expression of an object and what follows is always a function call:
```altscript
Σq ctor(1,2,3,4)(1,2,3,4)
```

## Free Functor

A free functor is a function routine type that does not belong to any enclosing class. It can be passed as a first-class object that can be dynamically created, destroyed, passed to a function, returned as a value, and have all the rights as other variables in the programming language have. for instance:
```altscript
f := fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
sum := f(1,2,3,4);
```
Here `f` is a variable intialized with a free functor to calculate the summary of an integer stream. The expression `f(1,2,3,4)` is a function call to use the variale `f` to create an function routine from the free functor stored in the variable. See [Free Functor](FreeFunctor.md) for more details.

## Abstract Free Functor

An abstract free functor is a free functor without a code block associated with its interface and it is often used as a declared type of an argument. Any argument specified by an abstract routine type cannot be used to for a function call unless it is bound to a concrete routine type with a block of code specified. For instance:
```altscript
f : fn(x: int...):int;
sum := f(1,2,3,4);      // Error: Call an abstract routine type f
```
See [Free Functor](FreeFunctor.md) for more details.

## Terminology Summary

* **functor**: a [function routine type](Routine.md) defined to perform a specific task.
* **function routine**: a functor instance (object) created to perform the task specified by the functor.
* **function call**: an action to use a functor to create a function routine.
* **function class**: a [class](Class.md) that defines functors only.
* **function**: a separated form for a functor defined in a function class.
* **function overloading**: multiple functions with different input interfaces for a function class.
* **free functor**: a functor that does belong to a function class.
* **abstract free functor**: a free functor with interface only and without a code block.

## See also:
* [Routine type](Routine.md)
* [Free functor](FreeFunctor.md)
* [Function class and functions](FunctionClass.md)
* [Stateless Class](StatelessClasse.md)
