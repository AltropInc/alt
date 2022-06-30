# Routine Type

A routine type defines a set of executable objects that have an common input and output interface associated with a block of code used to take the inputs, carry out a sequence of actions, and generate the output, if any, as specified. There are two kinds of routines: `construction routines` and `function routines`.

A routing type is specified by an input and output interface followed by a block of code. For example, the following routine type:
```altscript
(x: int...):int { sum:=0; foreach(e in x) sum+=e; sum }
```
specifies a routine that takes the input of an integer stream, calculates the sum of the integer stream , and returns the sum as its output. The input is given in a pair of parentheses as in `(x: int...)`. The output type, if any, is given after the colon following the input. The sequence of actions is given by a code block which is a set of statements placed in a pair of curly brackets.

The way to create executable objects (routines) from a routine type is determined by the kinds of routine types. There are two kinds of routine types: `construction routine type` and `function routine type`. A routine type, whether it is a function routine type or a construction routine type, is always specified within a class. The class is the `enclosing class` of the routine type. Routine types specified in the class constitutes the behavior of the class as well the behavior of objects created from the class. A construction routine type tells how the object or class is initialized, and a function routine type tells what the function of the object of the class can perform.

## Construction Routine Type

A construction routine type is a routine type prefixed with the keyword `ctor`. For example, a construction routine type in a `Point` class can be
```altscript
class Point
{
    ctor (x, y: double) { /* initialize x and y coordinates of the point using the input x and y */ }
}
```
A construction routine is instantiated from the construction routine type to initialize x and y coordinates of the point using the input x and y. This instantiation process is referred to as `constructor call`. The expression of a constructor call is the enclosing class name followed by a set of actual input values:
```altscript
Point(0,0)
```
This creates a construction routine that initialize x and y coordinates of a `Point` object using the provided input (0,0). Since a construction routine type always returns the initialized object as its output, the construction routine type does not have the output type in its specification.

## Function Routine Type

A function routine accomplishes a specific task by using input data, processing it, and returns a result in the output type as specified. A construction routine type is a routine type prefixed with the keyword `fn`. Consider a function class Σ:
```altscript
class Σ
{
    fn (x: int...): int { sum:=0; foreach(e in x) sum+=e; sum }
}
```
A function routine is instantiated from the function routine type toto perform the task described in the unction routine type. This instantiation process is referred to as `function call`. The expression of a function call is the object of the enclosing class followed by a set of actual input values:
```altscript
Σ()(1,2,3,4)
```
where `Σ()` is a constructor call to to generate an object of Σ and the Σ object is used to create a function routine that takes the input (1,2,3,4) and calculate the sum of the integers in the input. The expression `Σ()(1,2,3,4)` returns a value 10.

If the enclosing class of a function routine type is a subclass of `functional` and the enclosing class has no construction routine type or has a default construction routine type that can be called implicitly, the functional call can use the class name directly:
```altscript
class Σ implements functional   // Σ is a subclass that implements the functional interface
{
    fn (x: int...): int { sum:=0; foreach(e in x) sum+=e; sum }
}
sum := Σ(1,2,3,4);  // the function call uses the class name Σ directly
```
If we write the Σ class in a [simple class form](#Simple-Class-Form-and-Sealed-Class), it may look more familiar:
```altscript
functional Σ (x: int...): int
{
    sum:=0; foreach(e in x) sum+=e; sum
}
sum := Σ(1,2,3,4);
```
This simple form of class declaration makes the class Σ to appear in the same look of a function declaration in other programming laguages.




A routine type without a code block is an abstract routine type which cannot be used to instantiate any executable objects. Therefore, any argument specified by an abstract routine type cannot be used to create an executable unless it is bound to a concrete routine type with a block of code specified.


The process to create executable objects from a routine type is refered as `routine type call`. The expression of a `routine type call` 

# Construction Routine Type

A construction routine type is a routine type prefixed with the keyword `ctor`.

## Object Construction Routine Type (Constructor)

An `object construction routine type`, also referred as `constructor`, is used to create an object from its enclosing class:
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
box3 := Box(0.0, 0.0, 2, 4);       // Error: Call of overloaded routine is ambiguous: Box
box4 := Box(0.0, 0, 2, 4);         // use ctor(top, left, bottom, right: int) because inputs has more integers
box5 := Box(0.0, 0.0, 2.0, 4);     // use ctor(top, left, bottom, right: double) because inputs has more doubles
box6 := Box(0l, 0l, 2l, 4l);       // Error: Call of overloaded routine is ambiguous: Box
                                   // because an implicit conversion of long int to int is equally as valid as an
                                   // implicit conversion of long int to double
box7 := Box(0, 0, 2l, 4l);         // use ctor(top, left, bottom, right: int)
```

## Default Constructor

A default constructor is the constructor that either has no input parameters:
```altscript
  class Box
  {
      top_left: (double; double);
      bottom_right: (double; double);
      ctor()    // This is the default constructor
      {
          top_left = (0,0);
          bottom_right = (0,0);
      }
  }
```
or, if it has parameters, all the parameters have default values:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor(top, left, bottom, right: double = 0,0,0,0) // This is the default constructor
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
}
```
Only one default constructor is allowed in a class. If both the default constructor without any parameter and the default constructor with all the parameters having default values are provided, calling the default constructor without providing any input parameter will cause an error of ambiguity:
```altscript
class Box
{
    top_left: (double; double);
    bottom_right: (double; double);
    ctor ()  // This is the default constructor
    {
        top_left = (0, 0);
        bottom_right = (0, 0);
    }
    ctor(top, left, bottom, right: double = 0,0,0,0) // This is another default constructor
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
}
box := Box();  // Error: Call of overloaded routine is ambiguous: Box
```

## Constructor Chaining

Constructor chaining occurs through inheritance when subclass constructor calls super class’s constructor:
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
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
        super(origin, width, height)   // call the super class Box's ctor here
    {
        box_color = color;
    }
}
```
This ensures the initialization of the data members of the super class on the creation of a subclass object.

If the super class explicitly defines a constructor that is not the default, the subclass must call the constructor in the super:
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
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
         // Error: No default constructor in super class. Explicit super constructor call is required
    {
        box_color = color;
    }
}
```
unless the super class provides a default constructor:
```altscript
class Box
{
    top_left: (double; ouble);
    bottom_right: (double; ouble);
    ctor(top, left, bottom, right: double)
    {
        top_left = (top, left);
        bottom_right = (bottom, right);
    }
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box
{
    box_color: Color;
    ctor (origin: (x:double; y:double); width, height: double; color: Color)
         //  Okay, the default constructor in the super class Box is called automatically
    {
        box_color = color;
    }
}
```
The subclass can also select one of the constructors in the super class as the default constructor:
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
    ctor (origin: (x:double; y:double); width, height: double)
    {
        top_left = origin;
        bottom_right = (origin.x + width, origin.y+ height);
    }
}
class ColoredBox: Box(0,0,0,0)  // The constructor ctor(top, left, bottom, right: double) in super is selected as
                                // the defulat constructor
{
    box_color: Color;
    ctor (color: Color) // Okay, Box(0,0,0,0) is automatically called as the default constructor in the super class
    {
        box_color = color;
    }
}
```

# Function Routine

A function routine is a routine type prefixed with the keyword `fn`. It is always specified within a class.

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
