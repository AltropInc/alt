# Function Class and Functions

A **function class** is a class that defines only [functors (funcion routine types](Functor.md) in its class body. For instance:
```altscript
class AFuncClass: SuperClassName
{
  fn(x: int) { }
  fn(x, y: int) { }
  fn(x, y, z: int) { }
}
```
The function class `AFuncClass` has three [functors](Functor.md) with different interfaces.

If a function class is a [member class](MemberClass.md), the function class can be written in a set of separated forms, and each form specifies one functor interface and its related code block. This separated form is called **function**, sometimes also referred as **member function**. Each function starts with its super class name and followed by the function class name and then followed by the interface and the code block:
```altscript
SuperClassName AFuncClass(x: int) { }
SuperClassName AFuncClass(x, y: int) { }
SuperClassName AFuncClass(x, y, z:: int) { }
```
Here the function class `AFuncClass` is defined by three functions, each with a different interface and the related code block. Note that this concept is closely related to the mechanism of [function overloading or method overloading](https://en.wikipedia.org/wiki/Function_overloading) in some other programming languages to create multiple functions of the same name with different interface and implementations. The difference here is the emphasis on the fact that functions with the same name actually belong to the same function class.

## The Func Class

The class `func` is a built-in functional class which implements a calling protocol using an efficient and sequential procedure based on the call stack technique for a traditional function call. Use the `func` class, we can defines functions as below:
```altscript
func sum(values: int...): int
{
  sum := 0;
  foreach (v in values) sum += v;
  sum
}
func sum(strs: string...): string
{
    merged: string;
    foreach (s in strs) merged = merged + s;
    merged
}
```
Here `sum` is a function class. It is derived from the superclass `func`. The `sum` class has two functions, one takes an integer stream and generates output of the summary of the stream elements, the other takes a string stream and generates output of a merged string from the given string stream.

The class `func` is a [stateless class](KindsOfClasses.md) which contains no members in object scope. Therefore we perform the calling to a function derived from the `func` class, we use the function class name directly without giving the instance of the function class:
```altscript
int_sum := sum(1,2,3,4);
str_conjuction := sum("Hello","World");
```
See [Function Call Using Class Name](Functor.md) for more information.

The class `func` is also a [sealed](KindsOfClasses.md] class in which only [functors](Functor.md) can be defined. Therefore we can only write  `func` classes in function form. Writing a `func` classes in a regular class form is an error:
```altscript
class sum : func   // Error: Sealed class cannot be inherited
{
  fn (values: int...): int { ... }
  fn (strs: string...): string { ... }
}
```

## See also:
* [Routine type](Routine.md)
* [Functor](Functor.md)
* [Member class](MemberClass.md)
* [Input and output interface](InputOutputInterface.md)
* [Arguments passing](ArgumentsPassing.md)
