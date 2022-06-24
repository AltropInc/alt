# Functor Type

A functor type defines a set of execution objects that have an common input and output interface associated with a block of code used to
take the inputs, carry out a course of actions, and generate the output, if any, as specified.

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
If we already know the interface of the functor type for an augument, we do not need to repeat the interface specification in the concrete functor type
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

# Captures

A functor type can capture constants and variables from the surrounding context in which it’s defined. The execution object instantialted by the functor type
can then refer to and modify the values of those captured values, even if the original scope that defined these values no longer exists.
Therefore we can do things like this:
```altscript
func check_1s() : fn(i: int): bool
{
    ones: int... = (1, 11, 111, 1111);
    return { foreach (n in ones) if (i==n) return true; false };
}
is_1s: bool = check_1s()(111);
```
The function `check_1s` returns a functor type `fn(int):bool` to check if the given input `i` is one of the values listed in `ones`. In the execution body of the
returned functor type, the local value of a local integer stream `ones` is accessed. After the functor type is returned from `check_1s`, the block scope that
encloses `ones` exits and the local variable `ones` no longer exists. However, the returned functor type captures the ownership of the integer stream, so the destruction of the local variable `ones` will not deallocate the integer stream.

A functor type captures values of primitive types such as integers, enumerations, and booleans. These captured values are copies and the modification of these
captured values will not alter the original values in the enclosing environment:
```altscript
i := 5;
func bar() : fn():int
{
    return {++i};  // return the functor type 'fn():int{++i}' in which 'i' is captured by value (copy)
                   // the operator ++ increases the capyured value by 1 but does not change the original value
}
j := bar()();  // j gets the value 6
```
A functor type captures the ownership of composite types such as arrays, streams and tuples if the original composite value is owned by a local variable declared in a block:
```altscript
func bar(): fn(): int
{
    i := (5,5);          // local tuple value (5,5) is stored in the name 'i'
    return { ++i[0] };   // the functor type captures the 'i' value by reference and owns the valaue
                         // The original 'i' no longer owns the tuple value
}                        // 'i' is out of its scope and goes away, but the tuple value still exists becuase it is
                         // now owned by the captured name in the returned functor type
n:=bar()();              // 'n' gets the value 6
```
If the composite value is owned by an object or type (in object or class scope), the functor type will not capture the ownership of a composite value
In case the original composite value goes away, the value captured becomes null or empty too.
```altscript
object test
{
    i := int...(5,5);       // stream value (5,5) is stored in the name 'i' in the object scope
    func bar() : fn(): int
    {
        return { ++i[0] };  // the functor type captures the 'i' value and owns the valaue  by reference 
                            // but does not own the value
    }
    i = null;               // stream value stored in the name 'i' is cleared
    n:=bar()();             // An exception of 'Index on empty container' is raised
}
```

# Pass a Class to Functor Type

A class may contain several enter interfaces (constructors). Each enter interface (non-deferred) is actually a concrete functor type. Here is the example  of a function class `sum`:
The function class `sum` has two enter interfaces, one takes the input of an integer stream and another takes the input of a string stream:
```altscript
func sum (values: int...): int { s:=0; foreach (v in values) s += v; s }
func sum (strs: string...): string  { s:string; foreach (v in strs) s = s+v; s }
```
When we pass a class/function name to an abstract functor type (a functor type without a block of code), one of the matched enter interfaces of the class will be selected:
```altscript
for_each_int_do: fn(values: int...): int = sum;  // The sum's interface 'enter(values: int...):int' is selected
                                                 // and is assigned to 'for_each_int_do'
s = for_each_int_do (int...(1,2,3,4));           // 'for_each_int_do' is performed as a summary and 's' gets values of 10
```
When a class is member class or a function is a member function, the first parameter in a functor type must match the owner of the member, and the rest parameters in the functor type match to the enter interface of the member class or function. Example:
```altscript
class test
{
    factor := 5;
    func multiple (x: int): int { x*factor }   // multiple x by factor
    func apply_f(x: int; f: fn(owner: ownerclass; x: int): int): int { f(owner, x) }  // apply f to x
    m := apply_f(2, multiple);  // m gets value 10
}
```
In the above example, `multiple` is a member function where the input `x` is multiplied by the class member `factor`. When use `multiple` for the input parameter `f` in the function `apply_f`, the first parameter of `f` is used to match for the owner of `apply_f`. In this case, `apply_f` and `multiple` have the same owner and they match.

When the member is a meta member, however, no owner parameter should be involved in matching a functor type interface because a meta member is defined in class scope, not object scope:
```altscript
class test
{
    meta factor := 5;
    meta func multiple (x: int): int { x*factor }   // multiple x by factor
    func apply_f(x: int; f: fn(x: int): int): int { f(x) }  // apply f to x
    m := apply_f(2, multiple);  // m gets value 10
}
```
Because the enter interface of a non-functional class cannot have an output, a non-functional class can only pass to a functor type that do not have output or have an output in the type of `Object`. Here is an example:
```altscript
class test
{
    class member_class_1
    {
        enter(x:int) { /* initialize instance of member_class_1 using an integer here */}
        enter(x:string) { /* initialize instance of member_class_1 using a string here */}
    }
    func create_member #(type T: any)(x: T; f: fn(ow: ownerclass; x: T):Object) : Object
    {
        f(owner, x);
    }
    member1 := create_member("A Member Name", member_class_1); // A member object of 'member_class_1' is created in the instance of 'test'
}
```
Here is the version using meta member class:
```altscript
class test
{
    meta class member_class_1
    {
        enter(x:int) { /* initialize instance of member_class_1 using an integer here */}
        enter(x:string) { /* initialize instance of member_class_1 using a string here */}
    }
    func create_member #(type T: any)(x: T; f: fn(x: T):Object) : Object
    {
        f(x);
    }
    member1 := create_member("A Member Name", member_class_1); // A meta member object of 'member_class_1' is created in the class 'test'
}
```

# Generic Functor Type

The input and output interface of a functor type can be parameterized:
```altscript
stream_sum := fn#(type T:stream#(type element_type: numeric))(x: T): T.element_type;
stream_sum = { sum: T.element_type; foreach (e in x) sum+=e; sum };
sum := stream_sum(int...(1,2,3,4));   // sum gets value 10
stream_sum = { sum: T.element_type; foreach (e in x) sum+=e*e; sum };
drawable.z_order = foo(double...(1.1,2.2,3.3,4.4));   // sum2 gets value 36.3
```
Here `stream_sum` is a generic functor type with type parameter T in the interface. The input `x` can be any stream of numeric numbers, and it returns the sum of all elements contained in the stream.

Sometimes a functor type can appear in a generic interface even though it is not parameterized itself:
```altscript
func sum_func #(type T:stream)(x:T; f: fn(e:T.element_type):int): int
{
    sum : int = 0;
    foreach (e in x) sum += f(e);
    sum
}
int_sum := sum_func(int...(1,2,3,4), {e});                            // int_sum gets value of 10
str_length_sum := apply(string...("1234", "xyz"), {e.length()});      // str_length_sum gets value of 7
str_sum := sum_func(string...("1234", "xyz"), {e});
                                               ^ ---------------------// Compile error: Returned type is wrong
```
Here `sum_func` has a generic input interface that takes any type of stream and generates an integer output using the functor type 'f', which takes the element type of the stream and convert to an integer that is related to the element. The last `sum_func` call generates a compile error because the return type is string, which is not the output type specified by the functor type 'f'.
