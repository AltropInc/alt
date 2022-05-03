# Stream

A stream is a variable-length sequence of elements of the same type, in which elements are arranged in an order that can be accessed through an index starting from 0.
The `stream` type is subtype of `iterable`, a parameterized class with type parameter `element_type`:
```altscript
class stream #(element_type: any): iterable #(element_type);
```
A concrete stream type is created by binding the  type parameter `element_type` to a type. For instance:
```altscript
stream #(int)       // an integer stream, where the element_type is bound to int
stream #(numeric)   // a number stream mixed with different numeric data.  The element_type is bound to numeric
```
The ellipse token `...` can be used to declare a concrete `stream` type:
```altscript
int...       // an integer stream
numeric...   // a number stream mixed with different numeric data
int......    // a stream of integer streams
```

## Creating Streams

A stream object (or value) can be created by the stream constructor:
```altscript
s:= stream#(int)((1,2,3,4,5,6));  // or s:= int...((1,2,3,4,5,6));
```
A stream object can also be created by automatic conversion from an object in a compatable data structure:
```altscript
s: int... = (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into a int stream
```
When all expressions in a tuple value are constant literals, you can add a keyword `lit` to tell the parser to read the value directly into the stream
without a tuple-to-stream conversion:
```altscript
s: int... = lit (1,2,3,4,5,6);      // convert a tuple value (1,2,3,4,5,6) into a int stream
```
This will significantly reduce the parsing time.

## Passing Stream Object

Streams are mutable objects. You can add and delete elements in a stream. When you create a stream value through a constructor, a value conversion or a function output, and assign the value to a name, the name owns the stream value:
```altscript
s := range(0, 10).gen();
```
In this case, a stream is generated from range \[0,10) and assigned to name `s`. The name `s` owns the stream.

Stream objects follow the [pass-by-reference](https://en.wikipedia.org/wiki/Evaluation_strategy#Call_by_reference) property. When you provide stream value referred by a name to another name, the name that receives the passed stream value is just a reference to the same stream value owned by someone else.
For instance, when the stream owned by `s` is passed to another name:
```altscript
t := s;
```
the name `t` refers to the same stream but does not own the value. When new elements added in the stream via either of the names:
```altscript
s.append(10,11,12);
t.append(13,14);
```
Both `s` and `t` refers to the same stream (0,1,2,3,4,5,6,7,8,9,10,11,12,13,14). Adding or deleting elements in a steam will not invalidate any name that refers to the stream.

When a non-owner name owns or receives a different stream value, it does not affect the owner of the original value:
```altscript
s : int... = (1,2,3,4);      // `s` owns (1,2,3,4);
t := s;                      // `t` refers to the same stream (1,2,3,4), which is owned by `s`. But `t` does not own;
t = (-1, -2, -3);            // `t` owns (-1, -2, -3), and `s` keeps owning (1,2,3,4);
```
When an owner name owns or receives a different stream value, or the owner name goes out of its scope, its originally owned stream value is destroyed before it accepts the new value or it dies. And in this case, all other alive non-owner names that referred to the destroyed stream will become empty:
```altscript
s : int... = (1,2,3,4);      // `s` owns (1,2,3,4);
t := s;                      // `t` refers to the same stream (1,2,3,4) - t.length()==4.
s = (-1, -2, -3);            // `s` owns (-1, -2, -3). (1,2,3,4) is destroyed, and `t` becomes empty - t.length()==0;
```

When the value from an inside scope passed to a name in the enclosing (outside) scope, the ownership is transferred to the outside name. To be more accurate, if the destination name declared in a scope that outlives the scope in which the source name is declared, the destination name will take the ownership:
```altscript
s : int...;
{
    t := (1,2,3,4);      // `t` owns (1,2,3,4);
    s := t;              // 's' takes the ownership. `t` still refers to (1,2,3,4) but no longer own
}                        // `t` is out of scope; 's' keeps the ownership of (1,2,3,4)
```

## Stream Iteration

You can iterate or loop over each element of stream in forward, backward direction using `foreach`:
```altscript
s : int... = (1, 2, 3, 4);
sum:int = 0;
foreach (x in s) { sum += x; }              // sum is added to 10
foreach backward (x in s) { sum -= x; }     // sum is back to zero
```
You can also use stream iterator in any loop statement:
```altscript
s : int... = (1, 2, 3, 4);
sum:int = 0;
for (x:=s.begin(); x.is_valid(); x.next()) { sum += x; }     // forward, sum is added to 10
for (x:=s.rbegin(); x.is_valid(); x.next()) { sum -= x; }    // backward, sum is back to zero
```
Be careful when you add or delete elements of a stream when you're iterating over it, because adding or deleting elements can invalidate all iterators you got
before the modification.


## Stream Operations

* `func [(index: int)]: ref#(element_type)`
* `func at (index: int): ref#(element_type)` --
    returns the element reference at the specified position given by the index. If the index is not within the range of the container,
    an exception is raised with error "Index out of boundary".
```altscript
s : int... = (1,2,3,4);
s[0];                       // returns 1
s.at(1);                    // returns 2
s[0] = 5;                   // sets element at 0 to 5
s.at(1) = 6                 // sets element at 1 to 6
```
* `func length (): int` --
    returns number of elements in the stream
```altscript
stream#(numeric)(1, 2.5, 3, 4).length();     // returns 4
```
* `func empty (): bool` --
    returns true if the stream has no elements.
```altscript
stream#(int)(1, 2, 3, 4).empty();     // returns false
```
* `func front (): ref#(element_type)` --
    returns a reference to the first element in the stream.
* `func back (): ref#(element_type)` --
    returns a reference to the last element in the stream.
```altscript
s := int...(1,2,3,4);
s.front();    // returns 1
s.back();     // returns 4
```
* `func append (e: element_type): ownerclass`, or
  `func += (e: element_type): ownerclass` --
    appends the given element `e` at the end of the stream. Returns the owner stream.
* `func append (other: ownerclass): ownerclass`, or
  `func += (other: ownerclass): ownerclass` --
    appends another stream of the same type of the owner stream the end and returns the owner stream.
```altscript
s : int... = (1,2,3,4);
s.append(5,6,7).append(8);   // s get the value (1,2,3,4,5,6,7,8)
```
* `func + (e: element_type): ownerclass` --
    returns a new stream that combines this stream with a new element at the end
* `func + (other: ownerclass): ownerclass` --
    returns a new stream that combines this stream with another stream at the end
```altscript
s : int... = (1,2,3,4);
t := s + (5,6,7) + 8;   // t gets the value (1,2,3,4,5,6,7,8), s is unchanged
```
* `func begin (): iterator` --
    returns an iterator to the first element of the stream. If the stream is empty, it returns an invalid iterator.
```altscript
s := int...(1,2,3,4);
s.begin();     // returns an iterator that points to the first element 1
```
* `func rbegin (): iterator` --
    returns an iterator to the last element of the stream. If the stream is empty, it returns an invalid iterator.
```altscript
s := int...(1,2,3,4);
s.rbegin();     // returns a reverse iterator that points to the last element 4
```
* `func find (e: element_type; start: int=0; end: int=-1): int` --
    returns the lowest index starting from 0 in the stream where the specified element is first placed. `start` and `end` give the index range where the
    element to be searched.
```altscript
int...(1,2,3,4,3,1).find(3);     // returns 2, the lowest index starting from 0 where the element 3 is placed
```
* `func rfind (e: element_type; start: int=0; end: int=-1): int` --
    returns the highest index starting from 0 in the stream where the specified element is first placed. `start` and `end` give the index range where the
    element to be searched.
```altscript
int...(1,2,3,4,3,1).rfind(3);     // returns 4, the highest index starting from 0 where the element 3 is placed
```
* `func remove (e: iterator): iterator` --
    remove the element pointed by the given iterator and returns the iterator pointing to the next element. The returned iterator is invalid if
    next element is not available. 
```altscript
s := int...(1,2,3,4,3,1);
for (e:=s.begin(); e.is_valid();)
{
    if (e.value()==3) e = s.remove(); else e.next();
}
// removed all 3s in 's', the result is (1,2,4,1)
```
* `func erase (start:int; end:int=-1)` --
    remove the elements from start index until the end index. If the end index is not given, remove elements from start index all the way to the end.
```altscript
int...(1,2,3,4,3,1).erase(2,4) // the result is (1,2,3,1)
```
* `func [(start: int) .. (end: int)]: ownerclass`
* `func substream (start:int; end:int=-1): ownerclass` --
    returns a substream indexed from start to end. The element at the end position is not included. If the end position is given to -1,
    the end position is at the end of the element.
```altscript
int...(1,2,3,4)[1..3];          // returns a substream with elements (2,3).
int...(1,2,3,4).substream(1);   // returns a substream with elements (2,3,4).
```
* `func split (index:int): (ownerclass, ownerclass)` --
    returns a pair of substreams split by the specified index. The element at the index position falls into the second substream of the pair. 
```altscript
x, y := int...(0,1,2,3,4).split(2);          // 'x' gets elements (0,1), and 'y' gets elements (2,3,4).
```
