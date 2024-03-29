# Routine Interface

A **routine interface** is a [type signature](https://en.wikipedia.org/wiki/Type_signature) specifies an common inputs and possible outputs interface for a [routine type](routine.md) - a [constructor](Constructor.md) or a [functor](Functor.md). It defines the order and the type of the input arguments that are required to be passed into the routine in a routine call, and possibly, the type of the output returned by the routine.

Consider the following interface defined a [function class](FunctionClass.md) :
```altro
(values: int...): int
```
The interface specifies an interface that takes a stream of integers and generates an output of an integer. The input argument `values` is called the **formal parameter**, and the actual value we pass into a function call for input argument `values` is the **actual parameter**. Let's say, the interface is provided for a function class `sum`:
```altro
func sum (values: int...): int
```
In a function call:
```altro
sum ((1,2,3,4));
```
the integer stream `(1,2,3,4)` passed into the function call is the actual parameter for `values`. 

## Formal Parameter Specification

The specification of formal parameters generally takes the following form:
```altro
(n1: t1; n2: t2; ... nn: tn)
```
where ni (1<=i<=n) is the name of the input argument and the ti (1<=i<=n) is the type of the argument. Note that the separator between two input arguments is a semicolon, not comma. Comma is only used when two or more consecutive arguments have the same type:
```altro
(n1, n2: t12; ... nn: tn)
```
Examples:
```altro
(x:int; a_string:string)
(x,y: int; a_string_tuple:(string; string; string))
(x,y: int; a_string_tuple_with_named_elements:(first,second,third:string))
```

## Actual Parameters

Actual parameters are the actual values passed into a routine call. They are represented by a list of expressions in parentheses:
```altro
(1; "Hello")  // actual parameters for (x:int; a_string:string)
(3; ("Hello","world","!")) // actual parameters for (x,y: int; a_string_tuple:(string; string; string))
```
When a routine type is called, the formal parameter is temporarily "bound" to the actual parameter. The routine generated by the call uses the formal parameter name to access the actual value that the caller wants the routine to use.

Actual parameters are set left-to-right to match the formal parameters. You can also pick the match order by explicitly binding the name of the formal parameter with the value of the actual parameter. For example:
```altro
(a_string="Hello"; x=1)  // actual parameters given by names for (x:int; a_string:string)
```

## Default Parameter

A formal parameter can have a default value. If the actual parameter is not given in a function call, the default value will be used:
```altro
func foo (val: int; factor:int = 1)
```
Here the formal parameter `factor` has a default value `1`. In a function call
```altro
foo (3)
```
only the actual parameter `3` is given for the formal parameter `val`. The actual parameter used for the formal parameter `factor` will be the default value `1`.

If there are parameters without defaults after a parameter with default, giving actual parameters by the order will prohibit you from using the default. In this case, you have to give actual parameters by name, and this way, you can omit any parameter with a default. For example:
```altro
func foo (val1: int; factor1:int = 1; val2: int; factor2:int = 1);
foo (val=3; val2=6)
```

## Single Formal Parameter

If the input interface has only one formal parameter, you can omit the parentheses for the actual parameter:
```altro
func foo (values: int...);
foo (1,2,3,4);
```
Note that the parentheses in `(1,2,3,4)` is the part of the actual parameter itself (an integer stream), not the parentheses used for function calls. The more obvious example can be written this way:
```altro
x := (1,2,3,4);
foo x;
```

## Output Type

The output type of the interface is given after the formal parameter specification for the input and starts with a colon:
```altro
(values: int...): int
```
A routine type with an output type specified must return a value in the specified type at the end of the call. The following function has an output in the type of `int`:
```altro
func Σ(values: int...): int
{
    s:int=0;
    foreach (v in values) s+=v;
    return s
}
```
And the expression of the function call `Σ(1,2,3,4)` represents a value of an int 10, which is the summary of (1,2,3,4). If the last statement is an expression, the `return` statement can be replaced by the expression directly:
```altro
func Σ(values: int...): int
{
    s:int=0;
    foreach (v in values) s+=v;
    s
}
```
The interface of a [constructor](Constructor.md) does not specify output type, however, a constructor call always implicitly assumes that an object of the enclosing class be returned. See [Constructor](Constructor.md) for detail.

## Multiple Outputs

An interface can have multiple output types presented in a tuple type:
```altro
func floor_and_ceiling(d: double): (double; double) { (⌊x⌋, ⌈x⌉) }
```
The function floor_and_ceiling takes a real number as input and gives a tuple of two real numbers as output. Strictly speaking, a function should only return one value. However, the tuple result of a function call can be assigned to multiple variables:
```altro
floor, ceiling := floor_and_ceiling(3.14);
```
Here is the declaration of two variable names with a single initialization that returns a tuple value. The declared variable `floor` gets a real number 3 and the variable `ceiling` gets a real number 4. Note that a comma after a name suggests a name declaration. If variable names are already declared, you need to use parentheses to suggest a tuple expression, not a name declaration:
```altro
floor, ceiling: double;
(floor, ceiling) = floor_and_ceiling(3.14);
```

## Member Function Interface

When the interface is ued for a [member function](MemberCClass.md), call to the member function must provide the instance of the enclosing class, that is, the owner:
```altro
owner.member_function(actual_paramter1, ..., actual_paramtern)
```
The member selector `.` is actually not required for a member function call:
```altro
owner member_function(actual_paramter1, ..., actual_paramtern)
```
Here is the concrete example of a member function and its call:
```altro
class NameList
{
    name: string;
    ctor(n: string) { name = n }
    func + (other: NameList): NameList { NameList(name + ", " + other.name) }
}
nl := NameList("John");
n2 := NameList("Mary");
n3 := n1 + n2;
```
In the member function call `n1 + n2`, `n1` is the owner and `n2` is the actual parameter (input) and `+` is the member function name. The return of the call is a new `NameList` with name `"John, Mary"`.

## Prefix Member Function Interface

In a member function call, we sometimes prefer to put the member function name before the owner (the instance of the enclosing class):
```altro
member_function owner (actual_paramter1, ..., actual_paramtern)
```
Or when no actual parameter is required,
```altro
member_function owner
```
A prefix interface with a member function starts with the keyword `prefix`:
```altro
class IntPair
{
    i,j := 0, 0;
    prefix func ++ (): IntPair { ++i; ++j; owner }  // increases owner's i and j, returns the owner
}
n1 := IntPair();
n2 := ++n1;   // call prefix function '++'
```
While we call a non-prefix memmber function, we give the owner first, and then the memmber function name. In calling a prefix function, we put the owner after the memmber function name, as shown in the expression `++n1`, where `++` is the member function name and `n1` is the expression of the owner.

## Multi-Section Member Function Interface

A member function interface can consist of multiple sections separated by suffix names. For instance,
```altro
class IntStream
{
    i_stream: int...
    ctor(istr: int...) { i_stream = istr }
    func aggregate () from (start:int) to (end:int) with (agg_func: fn(x:int):int) :int
    {
        sum:=0;
        for (i:=start; i<end; ++i) sum += agg_func(i_stream[i]);
        sum
    }
}
ns := IntStream(1,2,3,4);
aggregate_power_2 := ns aggregate from 1 to 3 with {x²};
```
Here the member function has an input interface `(start:int; end:int; agg_func: fn(x:int):int)` that is separated into 4 sections where the first section is empty. A suffix name is used to separate between sections. When calling the member function, the actual parameters must be grouped into specified sections and separated by suffix names in the same way that specifies the input interface.

A suffix name can be appended at the end of the input interface. For instance, the built-in class `stream` defines the indexing member function like this:
```altro
class stream #(type element_type: any): iterable #(element_type)
{
    func [(index: int)]: ref#(element_type);
}
```
Here the member function `[` has one section but a suffix `]` is appended at the end of the input interface. When call this member function, the suffix must be provided after the actual parameter for the index:
```altro
i_stream = (1,2,3,4);
e := i_stream[3];
```
Here is another example using prefix interface:
```altro
sealed class double: real
{
    prefix func |()|: double;
}
```
Here the prefix member function `|` has one empty section with a suffix `|` is appended at the end of the input interface. When call this prefix member function, the owner value appears after the function name, and the suffix `|` must be attached after the owner value:
```altro
p := -3.14;
abs_p := |p|;
```
Because the input interface is empty and the member function is a prefix function, you do not need to provide in parentheses with empty actual parameters.


