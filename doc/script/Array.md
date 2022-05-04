# Array

An array is a fixede-length sequence of elements in a specified type. Array elements are arranged in an order that can be accessed through an index starting from 0
to the length of the array (\[0, length).
The `array` type is subtype of `iterable`, a parameterized class with type parameter `element_type` and `length`:
```altscript
class array #(type element_type: any; length: int): iterable #(element_type);
```
A concrete array type is created by binding type parameters `element_type` and `length`. For instance:
```altscript
array #(int, 4)       // an array that has 4 integers, where the element_type is bound to int, and the length is bound to 4
array #(numeric, 5)   // an array that has 5 numeric numbers.  The element_type is bound to numeric, and the length is bound to 5
```
The square brackets `[]` can be used to declare a concrete `array` type:
```altscript
int[4]        // an integer array that has 4 elements
numeric[5]    // a number array with 5 mixed numeric numbers
int[]         // an abstarct integer array where the length is undetermined
```

## Creating Array

An array object (or value) can be created by the array constructor:
```altscript
s:= int[6]((1,2,3,4,5,6));
```
An array object can also be created by automatic conversion from an object in a compatable data structure:
```altscript
s: int[6] = (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into an integer array
```
When all expressions in a tuple value are constant literals, you can add a keyword `lit` to tell the parser to read the value directly into the array
without a tuple-to-array conversion:
```altscript
s: int[6] = lit (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into a int array
```
This will significantly reduce the parsing time.

## Passing Array Object

Arrays are mutable objects. You can change the value of the elements contained in an array. When you create a stream value through a constructor,
a value conversion or a function output, and assign the value to a name, the name owns the stream value:
```altscript
s := int[6](1,2,3,4,5,6);
```
In this case, an array is generated from the array constructor and assigned to the name `s`. The name `s` owns the array value.

Array values follow the [pass-by-reference](https://en.wikipedia.org/wiki/Evaluation_strategy#Call_by_reference) property. When you provide an array value
referred by a name `s` to another name `t`, the name `t` is just a reference to the same array value owned by the name `s`.
```altscript
t := s;
```
the name `t` refers to the same array but does not own the value. When elements changed in the array via either of the names:
```altscript
s[0] = -1;
t[1] = -2;
```
Both `s` and `t` refers to the same array with the changed value (-1,-2,3,4,5,6).

When a non-owner name owns or receives a different array value, it does not affect the owner of the original value:
```altscript
s := int[4](1,2,3,4);      // `s` owns (1,2,3,4);
t := s;                    // `t` refers to the same array (1,2,3,4), which is owned by `s`. But `t` does not own;
t = (-1, -2, -3, -4);      // `t` owns (-1, -2, -3, -4), and `s` keeps the original value (1,2,3,4);
```
When an owner name owns or receives a different stream value, or the owner name goes out of its scope, its originally owned stream value is destroyed
before it accepts the new value or it dies. And in this case, all other alive non-owner names that referred to the destroyed stream will become null:
```altscript
s := int[4](1,2,3,4);      // `s` owns (1,2,3,4);
t := s;                    // `t` refers to the same array value (1,2,3,4);
s = (-1, -2, -3, -4);      // `s` owns (-1, -2, -3). (1,2,3,4) is destroyed, and `t` is set to null;
```

## Array Iteration

You can iterate or loop over each element of an array in a forward or backward direction using `foreach`:
```altscript
s : int[4] = (1, 2, 3, 4);
sum:int = 0;
foreach (x in s) { sum += x; }              // sum is added to 10
foreach backward (x in s) { sum -= x; }     // sum is back to zero
```
You can also use stream iterator in any loop statement:
```altscript
s : int[4] = (1, 2, 3, 4);
sum:int = 0;
for (x:=s.begin(); x.is_valid(); x.next()) { sum += x; }     // forward, sum is added to 10
for (x:=s.rbegin(); x.is_valid(); x.next()) { sum -= x; }    // backward, sum is back to zero
```

## Access Array Elements

You can access an array element by referring to its index number. The indexes in arrays start with 0, meaning that the first element has index 0,
and the second has index 1 etc. Accessing an array element is done by using the built-in array method `[]`:
```altscript
func [(index: int)]: element_type
```
Here is the example of calling the array method `[]`:
```altscript
s : int[4] = (1, 2, 3, 4);
first_ele := s[0];                  // first_ele gets the first element from the array `s`, i.e. 1;
second_ele := s[1];                 // second_ele gets the second element from the array `s`, i.e. 2;
```
If an index greater than the array length is specified when retrieving an array element, an exception is raised with an error of "Index out of boundary".

To change the element value contained in an array, the same built-in array method `[]` is used but using a different interface:
```altscript
func [(index: int)]: ref#(element_type)
```
In this case, an element reference is returned rather than the element value. Here is the example of using this interface to change the element value:
```altscript
s : int[4] = (1, 2, 3, 4);
s[0] = -1;                 // the first element is changed to -1;
s[1] = -2;                 // the second element is changed to -2;
```

Which interface of the built-in array method `[]` to use is determined by whether a reference is required in the context of the code. Where ever a name expression
or an expression of a reference is required, the interface that returns a reference is used and otherwise, the interface that returns the element value is used.

## Multi-dimensional Arrays

Here is the general form of a multidimensional array declaration:
```altscript
type_name[length1][length2]...[lengthN]
```
For example, the following declaration creates a 2 dimensional (3x4) integer array:
```altscript
s : int[3][4] = ((1,0,-1),(2,0,-2),(3,0,-3),(4,0,-4));
```
and the following declaration creates a 2 dimensional non-regular integer array where each sub-array in the contained array can have different lengths:
```altscript
s : int[][4] = ((1,),(2,-2),(3,0,-3),(4,0,0,-4));
```
To access elements from multidimensional arrays we can use the built-in array method `[]` repeatedly:
```altscript
s : int[3][4] = ((1,0,-1),(2,0,-2),(3,0,-3),(4,0,-4));
t := s[0];        // t gets the first sub-array (1,0,-1) in s
u := s[0][1];     // u gets the second element 0 in the first sub-array (1,0,-1) in `s`, which is equivalen to t[1]
```
Please note that accessing elements from a multidimensional array is going from the outermost to the innermost structure, while the declaration of
a multidimensional array is going from the innermost to the outermost. This looks like the process of packing and unpacking - the declaration is the
packing process where you pack the innermost first while the element assessment is the unpacking process where you unpack the outermost first.

## Array Operations

* `func at (index: int): ref#(element_type)` --
    returns the element reference at the specified position given by the index. If the index is not within the range of the container,
    an exception is raised with error "Index out of boundary".
```altscript
s : int[4] = (1,2,3,4);
s.at(1);                    // returns 2
s.at(1) = 6                 // sets element at 1 to 6
```
* `func front (): ref#(element_type)` --
    returns a reference to the first element in the array.
* `func back (): ref#(element_type)` --
    returns a reference to the last element in the array.
```altscript
s := int[4](1,2,3,4);
s.front();    // returns a reference to the element 1
s.back();     // returns a reference to the element 4
```
* `func value_front (): element_type` --
    returns the first element in the array.
* `func value_back (): element_type` --
    returns the last element in the array.
```altscript
int[4](1,2,3,4).value_front();    // returns 1
int[4](1,2,3,4).value_back();     // returns 4
```
* `func begin (): iterator` --
    returns an iterator to the first element of the array.
```altscript
s := int[4](1,2,3,4);
s.begin();     // returns an iterator that points to the first element 1
```
* `func rbegin (): iterator` --
    returns an iterator to the last element of the array.
```altscript
s := int[4](1,2,3,4);
s.rbegin();     // returns a reverse iterator that points to the last element 4
```
* `func find (e: element_type; start: int=0; end: int=-1): int` --
    returns the lowest index starting from 0 in the stream where the specified element is first placed. `start` and `end` give the index range where the
    element to be searched.
```altscript
int[6](1,2,3,4,3,1).find(3);     // returns 2, the lowest index starting from 0 where the element 3 is placed
```
* `func rfind (e: element_type; start: int=0; end: int=-1): int` --
    returns the highest index starting from 0 in the stream where the specified element is first placed. `start` and `end` give the index range where the
    element to be searched.
```altscript
int[6](1,2,3,4,3,1).rfind(3);     // returns 4, the highest index starting from 0 where the element 3 is placed
```
