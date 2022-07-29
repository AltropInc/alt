# Argument Passing

Passing arguments into an input interface during a [function or constructor call](Routines.md) can either be "pass-by-value" or "pass-by-reference".

All instances of [value classes](KindsOfClasses.md) such as int, char, enum etc are passed by value. That is,
a copy (or converted copy) of the instance (the value) is passed into the function and hence
the change of the value is not reflected outside from the caller. "pass-by-value" accept values
of [subtypes](Subtypes.md] and [convertable types](ConvertableTypes.md]. For instacne, you can pass an integer value to a double
precision parameter because an interger value can be converted to double precision. You can also
pass an integer to a parameter in declared type `Numeric`, because integer is a subtype of
`Numeric`.

For all instances of non-value classes, the former paramter is specified to be passed
by reference, which means that values are not copied when they are passed into the interface. Modifications
to mutable paramters (such as arrays and tuple) made within a [routine call](Routine.md) will be visible to the caller.
"pass-by-refenrece" accepts references of [subtypes](Subtypes.md] only. [Convertable types](ConvertableTypes.md] are not accepted. Consider:

```altscript
// this accepts all values convertable to tuple(int, int, int)
func f1(x : tuple(char, char, char)) { system.in.read(x); }
x1 := "xyz";
f1(x1);  // error, "xyz" is not subtype of tuple(char, char, char)
x2 := ('A', 'B', 'C', 'D');
f1(x2);  // okay, tuple(char, char, char, char) is subtype of tuple(char, char, char)
```

In case we need to pass convertible values to a function, we can explicitly declare a formal paramter
as a pass-by-value paramter by add keyword val before the paramter name:

```altscript
// this accepts all values convertable to tuple(int, int, int)
func f2(val x : tuple(char, char, char)) { system.out.println("x = {x}"); }

f2("xyz");  // okay, "xyz" is converted to ('x', 'y', 'z')
f2("xy");   // okay, "xy" is converted to ('x', 'y', '\0')
f2("ABCD"); // okay, "ABCD" is converted to ('A', 'B', 'C')
```

A value parameter is constant and temporary. It is the in the scope of the function call (the activity created
by the function call). Therefore, the value associated with the value parameter will be destroyed
upon the completion of the execution.

Note, even if the actual value passed to a value paramter does not require conversion, a clone (copy) will still
be used. Therefore, value parameter could be expensive for large data.