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

## Formal Parameters

The specification of formal parameters generally takes the following form:
```altro
(n1: t1; n2: t2; ... nn: tn)
```
where ni (1<=i<=n) is the name of the input argument and the ti (1<=i<=n) is the type of the argument. Note that the separator between two input arguments is a semicolon, not comma. Comma is only used when two or more consecutive arguments have the same type:
```altro
(n1, n2: t12; ... nn: tn)
```
A formal parameter can have a default value. If the actual parameter is not given in a function call, the default value will be used:
```altro
func foo (val: int; factor:int = 1)
```
Here the formal parameter `factor` has a default value `1`. In a function call
```altro
foo (3)
```
only the actual parameter `3` is given for the formal parameter `val`. The actual parameter used for the formal parameter `factor` will be the default value `1`.

Actual parameters are set left-to-right to match the formal parameters. If there are parameters without defaults after a parameter with default, you have to pick the match order by giving the name of the formal parameter. For example:
```altro
func foo (val1: int; factor1:int = 1; val2: int; factor2:int = 1)
```
If you do not want to give actual parameters for all four formal parameters, you can pick the formal parameters in a function call by explicitly giving the formal parameter name:
```altro
foo (val=3; val2=6)
```
If the function interface has only one formal parameter, you can omit  the parentheses for the actual parameter:
```altro
sum (values: int...);
sum (1,2,3,4);
```
Note that the parentheses in `(1,2,3,4)` is the part of the actual parameter itself (an integer stream), not the parentheses used for function calls. The more obvious example can be written this way:
```altro
x := (1,2,3,4);
sum x;
```

