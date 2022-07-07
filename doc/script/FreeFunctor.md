# Free Functor

A free functor is a [function routine type](Routine.md) ((abbreviation: [functor](Functor.md)) that does not belong to any enclosing class. It can be passed as a first-class object that can be dynamically created, destroyed, passed to a function, returned as a value, and have all the rights as other variables in the programming language have.

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
f := fn(x: int...):int;
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
We can pass a concreate free functor with matched interface to an argument of an abstract free functor. In addition, we can alos pass:
* a concreate free functor with an implicit interface;
* a function class; or
* a member function class

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
When we pass the function class `sum` tothe variable `for_each_int_do`,  a function with the matched interfaces `(values: int...): int` is selected: 
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

