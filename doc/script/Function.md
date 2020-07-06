# Function

## Functional

The Functional class is an abstract class for callable objects which are instantiated by
"function call".  An Functional class is used to create callable objects that performs some
functionality described by the enter block. When they finished their performance,
their duties are done and they are deleted. Examples of functional instantiations are a
function to open a file, a process to display an image, a thread to deal with an event,
a scientific calculation, and an activity to generate a signal. For instance,

```altscript
func print(text: string)
{
    system.out.println(string);
}
```

declares a functional class print (the base class is finc, a concrete class thta implementates
the abstract Functional). And this can be tralated to:

```altscript
class print is func
{
  enter (text: string)
  {
      system.out.println(string);
  }
}
```
The function call:

```altscript
print("Hello, World!");
```
instantiates a callable object to perform the task to print "Hello, World!" to system.out.

## The func class

The class func is a built-in subclass of Functional which impelemts a traditional function call
where the caller is blocked until the function call completes, and if output is specified, the
output is generated.

## Arguments Passing

Function arguments can either be "pass-by-value" or "pass-by-reference". For all instances of
value classes such as int, char, enum etc are passed by value. That is, a copy (or converted copy) of the instance
(the value) is passed in the function and hence the change of the value is not reflected outside
from the caller. For all instances of non-value classes, if the type if the actual instance is
a subtype of type decalred by the formal paramter, it will be passed by reference. If the type of
the actual instance is convertable to the type decalred by the formal paramter, it will be passed
by value. Otherwise, the actual instance cannot be passed and it is considered a type mis-match
error.



