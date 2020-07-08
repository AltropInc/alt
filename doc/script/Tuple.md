# Tuple

## Tuple type

A tuple is a container of fixed number of heterogeouns elements arranged in a specified order that cannot be changed.

The tuple elements can be either named or unamed, but all tuple elements must be ether all named or all unnamed.
A tuple is written in a list of element types with round brackets, for instance:

```altscript
(int, string, double)
(position: (x:int, y: int), height: int);
```

A tuple type can be parameterized. The list of type paramters is written with round brackets prefixex with #. For instance,

```altscript
#(type ElementType: Numeric)(first: ElementType, second: ElementType)
```
A paramterized tuple type is often used to specify the type dependeny among tuple elements. In the above example, the type of the first
and the second elements are related. Althong it tells you that tehy can be any numeric types, but once the ElementType is bound to
a concreted numeric type, say, int, they has to be both in int. Example:

```altscript
#(type ArrayType: array)(first: ArrayType.ElementType, second: ArrayType)
```
Here the type of the first tuple element must be the element type of the array type for the second tuple element. Consider another example:

```altscript
#(type T1, T2: Any, size: int)(first: array#(T1, size), second: array#(T2, size))
```

Here the second array is the tuple must have the same size of the first array.

A paramterized tuple type is an absctact type if any of its type parameters are unbound. Therefore, a paramterized tuple type
can be used to declare a polymorphic variable but cannot used to instantiate a tuple value. Example:

```altscript
type MyTupleType = #(type ArrayType: Array)(first: Array.ElementType, second: Array);
my_tuple: MyTupleType;          // my_tuple is a polymophic tuple which currently has a null value
my_array: int[5] = (1,2,3,4,5);
my_tuple = (my_array.first(), my_array);  // my_tuple has a value of type (int, Array#(int,5))
```
## Tuple Subtype

Tuple types cannot be covariant in their element types but can be covariant in their type paramters.
For instance, (int[5], int[5]) cannot be a subtype of (array, array), but can be a subtype of
#(type T: array)(T, T).




