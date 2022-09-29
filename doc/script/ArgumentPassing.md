# Argument Passing

Passing instances among arguments can either be **pass-by-value** or **pass-by-reference**. 

All instances of [value types](ValueClass.md) such as int, char, enum etc are passed by value. That is, a copy (or converted copy) of the instance (the value) is passed from one argument to another. As result, the two arguments hold distinct copies of the value. Changing value through one argument will not affect the value held by the other argument. Therefore, when a caller pass a value instance into an input interface during a function or constructor call, the change made within the call will not be reflected outside from the caller.

All instances of non-value classes are passed by reference, which means that values are not copied when they are passed from one argument to another. A modification
to an instance of a non-value class through one argument will be sensed by all other arguments that have the same reference. Therefore, when a caller pass a non-value instance into an input interface during a function or constructor call, the change made within the call will be visible to the caller.

Pass-by-value accepts values of [subtypes](Subtypes.md] and [convertible types](ConvertableTypes.md]. For instance, you can pass an integer value to a double
precision parameter because an integer value can be converted to double precision. You can also pass an integer to a parameter in declared type `numeric`, because integer is a subtype of `numeric`.

Pass-by-reference accepts references of [subtypes](Subtypes.md] only. [Convertible types](ConvertableTypes.md] are not accepted.

Consider:
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
