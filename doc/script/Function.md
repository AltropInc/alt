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

Function arguments can either be "pass-by-value" or "pass-by-reference".

For all instances of value classes such as int, char, enum etc are passed by value. That is,
a copy (or converted copy) of the instance (the value) is passed into the function and hence
the change of the value is not reflected outside from the caller. "pass-by-value" accept values
of sybtypes and convertable types. For instacne, you can pass an integer value to a double
precision parameter because an interger value can be converted to double precision. You can also
pass an integer to a parameter in declared type "Numeric", because integer is a subtype of
Numeric.

For all instances of non-value classes,  if the former paramter is specified to accept convertable
values, it will be passed by value.  A converted copy will be created. Otherwise, it will be passed
by reference, which means that values are not copied when they are passed to functions and  modifications
to mutable values (such as arrays and tuple) made within a function will be visible to the caller.
"pass-by-refenrece" accepts references of sybtypes only. Convertible types are not aceepted. Consider:

```altscript
// this accepts all values convertable to tuple(int, int, int)
func print1(x :< tuple(char, char, char)) { system.out.println(x); }

// this accepts values in subtype of tuple(int, int, int)
func print2(x : tuple(char, char, char)) { system.out.println(x); }

print1("xyz");  // okay, "xyz" is converted to ('x', 'y', 'z')
print1("xz");   // okay, "xy" is converted to ('x', 'y', '\0')
print1("ABCD"); // okay, "ABCD" is converted to ('A', 'B', 'C')
print2("xyz");  // not okay, "xyz" is not subtype of tuple(char, char, char)
print2(('A', 'B', 'C', 'D'));  // okay, tuple(char, char, char, char) is subtype of tuple(char, char, char)
```


