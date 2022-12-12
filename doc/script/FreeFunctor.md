# Free Functor

A free functor is a [function routine type](Routine.md) that does not belong to any enclosing class. It can be passed as a first-class object that can be dynamically created, destroyed, passed to a function, returned as a value, and have all the rights as other variables have. This is a term often referred as [lambda function or anonymous function](https://en.wikipedia.org/wiki/Anonymous_function) in other programming languages.

Here is an example to pass a free functor to a variable named as `f`, and then use the name `f` to create function routine ([function call](Functor.md)) to take the input `(1,2,3,4)` and generate a summary of `10`:
```altscript
f := fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
sum := f(1,2,3,4);
```

## Abstract Free Functor

In declaration:
```altscript
f := fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum };
```
the type of `f` is inferred to `fn(x: int...):int`, which is an abstract free functor. An abstract free functor is a free functor without a code block associated with its interface and it is often used as a declared type of an argument:
```altscript
f: fn(x: int...):int;
```
An abstract functor cannot be used to instantiate any routine (routine type call). Therefore, any argument specified by an abstract routine type cannot be used to for a function call:
```altscript
f: fn(x: int...):int;
sum := f(1,2,3,4);      // Error:  Call an abstract routine type f
```
The argument specified by an abstract routine type needs to be initialized to a concrete free functor before it can be used:
```altscript
f: fn(x: int...):int;
f = fn(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum };
sum := f(1,2,3,4);      // Okay, sum gets value of 10
```
We can pass a concreate free functor with matched interface to an argument of an abstract free functor. In addition, we can also pass:
* a concreate free functor with an implicit interface
* a function class
* a member function class
* a meta member function class

## Pass a Free Functor with Implicit Interface

When we pass a free functor to an argument typed by an abstract functor, we already know the interface of the functor for the augument. Therefore,  we do not need to repeat the interface specification in free functor to be passed to the argument. Therefore, we can give a free functor by providing the code block only (a free functor with an implicit interface):
```altscript
f: fn(x: int...):int;
f = { sum:=0; foreach(e in x) sum+=e; sum };
```
or,
```altscript
f: fn(x: int...):int = { sum:=0; foreach(e in x) sum+=e; sum };
```
Here is an example to pass a free functor as an input:
```altscript
func apply (x:int...; f: fn(x:int):int): int
{
    sum:=0;
    foreach(e in x) sum += f(x);
    sum;
}
sumf := apply((1,2,3,4), {x*x});  // pass  fn(x:int):int):int{x*x} to the f input argument in apply
```
And here is an example to pass a free functor as an output:
```altscript
func bar(): fn(x: int...): int
{
    return { sum:=0; foreach(e in x) sum+=e; sum };
}
sum := bar()(1,2,3,4);
```

## Pass a Function Class

Consider a [function class](Functor.md) `sum` that has two functions:
```altscript
func sum (values: int...): int { s:=0; foreach (v in values) s += v; s }
func sum (strs: string...): string  { s:string; foreach (v in strs) s = s+v; s }
```
and two variables declared using a abstract functor type:
```altscript
for_each_int_do: fn(values: int...): int;
for_each_str_do: fn(values: string...): string;
```
When we pass the function class `sum` to the variable `for_each_int_do`,  a function with the matched interfaces `(values: int...): int` is selected: 
```altscript
for_each_int_do = sum;                  // The sum's functor 'fn(values: int...):int' is selected
s := for_each_int_do (int...(1,2,3,4)); // 's' gets values of 10
```
When we pass the function class `sum` tothe variable `for_each_str_do`,  a function with the matched interfaces `(values: string...): string` is selected: 
```altscript
for_each_str_do = sum;                   // The sum's functor 'fn(values: string...):string' is selected
s := for_each_int_do (int...(1,2,3,4)); // 's' gets a merged string of all string elements in the given stream
```

## Pass a Member Function Class

When a function class is [member function class](MemberClass.md), the first parameter in a functor type must match the owner of the member function class, and the rest parameters in the functor match to the selected functor interface of the member function class. Example:
```altscript
class EnclosingClass
{
    factor := 5;
    func multiple (x: int): int { x*factor }              // A member function that multiplies x by factor
    f : fn(obj: EnclosingClass; x: int): int = multiple;  // a free functor variable initialized by the member function class
    m := f(self, 2);                                      // m gets value 10
}
```
Here, `multiple` is a function of a member function class which multiplies the input `x` by the member data `factor`. When use `multiple` to initialize `f` whihc is declared as an abstract functor `fn(obj: selfclass; x: int): int`, the first parameter of the functon, `obj`, is used to match for the owner of the member function class `multiple`, i.e. the class `EnclosingClass`. When we make the call to `f`,  the actual parameter `self` is provided, which represents an instance of the `EnclosingClass`.

## Pass a Meta Member Function Class

If the member function class is a meta member function class, however, no owner parameter should be involved in matching a functor type interface because a meta member is defined in the class scope, not the object scope:
```altscript
class test
{
    meta factor := 5;
    meta func multiple (x: int): int { x*factor }   // A meta member function that multiplies x by factor
    f : fn(x: int): int = multiple;                 // a free functor variable initialized by the meta member function class
    m := f(2);                                      // m gets value 10
}
```

## Captures

Because a free functor is not a member of any class, what if it needs to access values of constants and variables from the surrounding context in which the functor is defined? A free functor can capture these values from the surrounding context where it is defined. The function routine instantiated by the functor can then refer to and modify these captured values, even if the original scope that defined these values no longer exists. Therefore we can do things like this:
```altscript
func check_1s() : fn(i: int): bool
{
    ones: int... = (1, 11, 111, 1111);
    return { foreach (n in ones) if (i==n) return true; false };
}
is_1s: bool = check_1s()(111);
```
The function `check_1s` returns a functor `fn(int):bool` to check if the given input `i` is one of the values listed in `ones`. In the block code of the
functor to be returned, the local value of a local integer stream `ones` is accessed. After the functor is returned from `check_1s`, the block scope that
encloses the variable `ones` exits and the local variable `ones` no longer exists. However, the returned functor captures the ownership of the integer stream, so the destruction of the local variable `ones` will not deallocate the integer stream.

A functor captures values of primitive types such as integers, enumerations, and booleans. These captured values are copies and the modification of these
captured values will not alter the original values in the enclosing environment:
```altscript
i := 5;
func bar() : fn():int
{
    return {++i};  // return the functor 'fn():int{++i}' in which 'i' is captured by value (copy)
                   // the operator ++ increases the captured value by 1 but does not change the original value
}
j := bar()();     // j gets the value 6
```
A functor captures the ownership of composite types such as arrays, streams and tuples if the original composite value is owned by a local variable declared in a block:
```altscript
func bar(): fn(): int
{
    i := (5,5);          // local tuple value (5,5) is stored in the name 'i'
    return { ++i[0] };   // the functor captures the 'i' value by reference and owns the value
                         // The original 'i' no longer owns the tuple value
}                        // 'i' is out of its scope and goes away, but the tuple value still exists because it is
                         // now owned by the captured name in the returned functor
n:=bar()();              // 'n' gets the value 6
```
If the composite value is owned by an object or type (in object or class scope), the functor will not capture the ownership of a composite value
In case the original composite value goes away, the value captured becomes null or empty too.
```altscript
object test
{
    i := int...(5,5);       // stream value (5,5) is stored in the name 'i' in the object scope
    func bar() : fn(): int
    {
        return { ++i[0] };  // the functor captures the 'i' value by reference 
                            // but does not own the value
    }
    i = null;               // stream value stored in the name 'i' is cleared
    n:=bar()();             // An exception of 'Index on empty container' is raised
}
```

## Functor with Parametric Interface

The input and output interface of a functor can be parameterized:
```altscript
stream_sum := fn#(type T:stream#(type element_type: numeric))(x: T): T.element_type;
stream_sum = { sum: T.element_type; foreach (e in x) sum+=e; sum };
sum := stream_sum(int...(1,2,3,4));   // sum gets value 10
stream_sum = { sum: T.element_type; foreach (e in x) sum+=e*e; sum };
drawable.z_order = foo(double...(1.1,2.2,3.3,4.4));   // sum2 gets value 36.3
```
Here `stream_sum` is a generic functor with type parameter T in the interface. The input `x` can be any stream of numeric numbers, and it returns the sum of all elements contained in the stream.

Sometimes a functor can appear in a parametric interface even though it is not parameterized itself:
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
Here `sum_func` has a generic input interface that takes any type of stream and generates an integer output using the functor 'f', which takes the element type of the stream and convert to an integer that is related to the element. The last `sum_func` call generates a compile error because the return type is string, which is not the output type specified by the functor 'f'.

