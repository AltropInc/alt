# Stream

A stream is a varaible-length sequence of elements of the same type that are arraged in an order in which can be accessed through an index starting from 0.
The `stream` type is subtype of `iterable`, a paramterized class with type parameter `element_type`:
```altscript
class stream #(element_type: any): iterable #(element_type);
```
A concreate stream type is created by binding the  type parameter `element_type` to a type. For instance:
```altscript
stream #(int)       // an integer stream, where the element_type is bound to int
stream #(numeric)   // a number stream mixed with different numeric data.  The element_type is bound to numeric
```
The ellipse token `...` can be used to declare a concreate `stream` type:
```altscript
int...       // an integer stream
numeric...   // a number stream mixed with different numeric data
int......    // a stream of integer streams
```

## Creating Streams

A stream object can be created by constructor:
```altscript
s:= stream#(int)((1,2,3,4,5,6));  // or s:= int...((1,2,3,4,5,6));
```
A stream object can also be created by automatic conversion from an object in a compatable data structure:
```altscript
s: int... = (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into a int stream
```
When all exparessions in tuple value are constant literals, you can add a keyword `lit` to tell the parser to read the value directly into the stream
without a tuple conversion:
```altscript
s: int... = lit (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into a int stream
```
This will significanly reduce the parsing time.

##Passing Stream Object

Streams are mutable objects. You can add and delete elements in a stream. Stream objects follow the pass-by-reference property.



