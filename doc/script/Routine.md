# Routine Type

A routine type defines a set of executable objects that have an common input and output interface associated with a block of code used to take the inputs, carry out a sequence of actions, and generate the output, if any, as specified. There are two kinds of routines: `construction routines` and `function routines`.

A routing type is specified by an input and output interface followed by a block of code. For example, the following routine type:
```altscript
(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
```
specifies a routine that takes the input of an integer stream, calculates the sum of the integer stream , and returns the sum as its output. The input is given in a pair of parentheses as in `(x: int...)`. The output type, if any, is given after the colon following the input. The sequence of actions is given by a code block which is a set of statements placed in a pair of curly brackets.

A routine type without a code block is an abstract routine type which cannot be used to instantiate any executable objects. Therefore, any argument specified by an abstract routine type cannot be used to create an executable unless it is bound to a concrete routine type with a block of code specified.

The way to create executable objects from a routine type is determined by the kinds of routine types. There are two kinds of routines: `function routines` and `construction routines`. A function routine accomplishes a specific task by using input data, processing it, and returns a result as output. A construction routine does the initialization of an object or a class by using the input data, and returns the initialized object as its output if it is an object construction routine. It looks like a construction routine is a special case of function routine by this definition. However, how they are specified and invoked are different.

# Constructor Routine

A construction routine is a routine type prefixed with the keyword `ctor`. It is always specified within a class.

## Object Construction Routine (Constructor)

An `object construction routine`, also referred as `constructor`, is used to create an object from its enclosing class:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
```
The constructor in the class `Box` is a routine type specified after the key word `ctor`. It takes the input of the box origin, and the width and height of the box, and uses the input parameters to initialize the box by setting the values for the top-left and bottom-right corners of the box.

To use the constructor of a class to create an object, we give the class name and provide a list of expressions for the inputs (if any), thus the expression `Box((0,0), 2, 4)` creates an object of `Box` from the origin (0,0) with width 2 and height 4. This process is often referred as `contructor call`.

The interface of a constructor does not specify output type, however, a constructor always returns an object of the enclosing class. Therefore, the constructor call `Box((0,0), 2, 4)` returns an object of `Box`:
```altscript
box := Box((0,0), 2, 4);
```

## Class Construction Routine (Meta Constructor)

A `class construction routine`, also referred as `meta constructor`, is used to initialize meta members of its enclosing class:
```altscript
  class Box
  {
      meta box_count : int;
      top_left: (double; double);
      bottom_right: (double; double);
      meta ctor()
      {
          box_count = 0;
      }
      ctor (origin: (x:double; y:double); width, height: double)
      {
          ++box_count;
          top_left = origin;
          bottom_right = (origin.x + width, origin.y+ height);
      }
  }
```
Meta members belong to the class scope. A meta constructor cannot have any input parameters in the interface, and it cannot be explicitly called.  One class can have only one meta constructor.

## Constructor Interface Overloading

More than one constructor can be used in a class, as long as each constructor has a different list of argument types. This concept is known as `Constructor Overloading`. While creating the object, input arguments must be passed to let the script parser know which constructor routine needs to be called. A constructor routine must be uniquely selected depending upon the number and type of input arguments passed, which must match the specified interface of the constructor. Example:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor()
    {
        top_left = (0,0);
        bottom_right = (0,0);
    }
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
empty_box := Box();        // use ctor()
box1 := Box(0, 0, 2, 4);   // use ctor(top, left, bottom, right: double)
box2 := Box((0, 0), 2, 4); // use ctor(origin: (x:double; y:double); width, height: double)
```
Each constructor must  have a different list of argument types. The argument names are not counted for the difference:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(top, left, width, height: double)          // Error: Same interface already implemented
    {
        top_left = (top, left);
        bottom_right = (top + bottom, left + right);
    }
}
```
If a constructor routine cannot be uniquely selected based on the provided input type, an error of ambiguity will be reported by the parser:
```
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor(top, left, bottom, right: int)
    {
        top_left = (top, left);
        bottom_right = (top, left);
    }
}
box1 := Box(0, 0, 2, 4);           // use ctor(top, left, bottom, right: int)
box2 := Box(0.0, 0.0, 2.0, 4.0);   // use ctor(top, left, bottom, right: double)
box2 := Box(0.0, 0.0, 2, 4);       // Error: Call of overloaded routine is ambiguous: Box
box2 := Box(0.0, 0, 2, 4);         // use ctor(top, left, bottom, right: int) because inputs has more integers
box2 := Box(0.0, 0.0, 2.0, 4);     // use ctor(top, left, bottom, right: double) because inputs has more doubles
box2 := Box(0l, 0l, 2l, 4l);       // Error: Call of overloaded routine is ambiguous: Box
                                   // because the distance betwwen long integers and integers  is the same as the one
                                   // between doubles and integers.
```




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
The enter interface of a non-functional class does not have an output specified, and by default, an instance of the non-functional class is created as the output as the result of calling this enter interface. Therefore, a non-functional class can only be passed as a parameter to a functor type that does not have an output or has an output in the type that is the same as or is the super class of the passed non-functional class. Here is an example where the member is created in object scope using a selected member class:
```altscript
class test
{
    class member_base {}
    class member_class_1 is member_base
    {
        enter(x:int) { /* initialize instance of member_class_1 using an integer here */}
        enter(x:string) { /* initialize instance of member_class_1 using a string here */}
    }
    func create_member #(type T: any)(x: T; f: fn(ow: ownerclass; x: T):member_base) : member_base
    {
        f(owner, x);
    }
    member1 := create_member("A Member Name", member_class_1); // A member object of 'member_class_1' is created in the instance of 'test'
}
```
Here is the version using meta member class where the member is created in class scope using a selected meta member class:
```altscript
class test
{
    meta class member_base {}
    meta class member_class_1 is member_base
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
