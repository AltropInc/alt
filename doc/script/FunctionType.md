# Functor Type

A functor type defines a set of execution objects that have an common input and output interface associated with a block of code used to
take the inputs, carry out a course of actions, and generate the output as specified.

A functor type starts with the key word `fn`, followed by an input and output interface, and then a block of code:
```altscript
fn (x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
```
where `(x: int...)` is the input specification, `:int` is the output specification, and `{ sum:=0; ... sum; }` is the code block
that specifies the course of actions.

A functor type without a code block is an abstract functor type which cannot be used to instantiate any execution objects.

An execution object is instantiated from a functor type by providing a set of input values that matches the specified input interface:
```altscript
fn (x: int...):int { sum:=0; foreach(e in x) sum+=e; sum } (1,2,3,4)
```
The above execution is instantiated by providing an input value of an integer stream `(1,2,3,4)`. And the output of the execution is an integer
of the sum value `10`.

We can pass functor types as first-class values that can be dynamically created, destroyed, passed to into an input interface, returned as an
output value, and have all the rights as other variables have.

Here is the example to pass a functor type to the name of `f`, and then use the name `f` to generate the execution to take the input `(1,2,3,4)`
and generate the sum `10`:
```altscript
f := fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
sum := f(1,2,3,4);
```
In the above name declaration of `f`, the type of `f` is inferred to `fn(x: int...):int`, which is an abstract functor type, and `f` is
initialized to a concrete functor type that has code block to calculate the integer stream summary, which is equivalent  to:
```altscript
f : fn(x: int...):int = fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
sum := f(1,2,3,4);
```
If we already know the interface of the functor type for a augument, we do not need to repeat the interface specification in the concrete functor type
that is provided as a value to the augument. Therefore, the following code is also valid:
```altscript
f : fn(x: int...):int = { sum:=0; foreach(e in x) sum+=e; sum }
sum := f(1,2,3,4);                              // sum gets the output value 10
f = { sum:=0; foreach(e in x) sum+=e*e; sum }   // assign f to a different concrete functor type
sum2 := f(1,2,3,4);                             // sum2 gets the output value 30
```
Here is the example to pass a functor type as an input:
```altscript
func apply (x:int...; f: fn(x:int):int): int
{
    sum:=0;
    foreach(e in x) sum += f(x);
    sum;
}
sumf := apply((1,2,3,4), {x*x});  // pass  fn(x:int):int):int{x*x} to the f input argument in apply
```
And here is the example to pass a functor type as an output:
```altscript
func bar(): fn(x: int...): int
{
    return { sum:=0; foreach(e in x) sum+=e; sum };
}
f := bar();
sum := f(1,2,3,4);
```



