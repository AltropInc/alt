# Iterable Types

An iterable is an instance capable of returning its members one at a time, permitting it to be iterated over in a [foreach-loop](StatementsLoop.md). Familiar examples of iterables include arrays, streams, tuples, and strings - any such instance can be iterated over in a foreach-loop. An [object](Object.md) is also an iterable, permitting it to be iterated over its child objects. The following types are iterable:

* A type implements the `iterable` interface
* A tuple
* An [object class](Object.md)
* A type has a delegated iterable member
* A [data class](KindsOfClasses.md) that has custom iterator implementation

## The `iterable` Interface

The `iterable interface` represents a collection of iterable elements in a specific type, given by the type parameter `element_type`:

```altro
interface class iterable #(type element_type: any)
{
    type iterator = iterator#(selfclass);
    func begin(): iterator;
    func rbegin(): iterator;
}
```
The `iterable` interface also specifies an `iterator` type which is used to create an iterator for accessing an element in the collection. The `begin` function returns the iterator points to the first element in the selection. The `rbegin` function returns the iterator points to the last element in the selection, and the iterator obtained from `rbegin` is a backward iterator, that is, it advances in a backward direction. 

The `iterator` calss is parametrized by the iterable collection:
```altro
class iterator #(type iterable_type: iterable)
{
    func value(): iterable_type.element_type;
    func ended(): bool;
    func valid(): bool;
    func next();
    func reverse();
    func index(): int;
    func direction(): int;
}
```
An iterator has the following member functions:

| name      | meaning                                                                 |
|:--------- |:----------------------------------------------------------------------- |
| value     | gest the value/object referred by the iterator                          |
| ended     | tells if the iterator reaches to the end and it referes to null         |
| valid     | tells if the iterator points to a valid element, valid()=!ended()       |
| index     | returns the order of the element the iterator refers to in the iterable |
| next      | advances the iterator for the next or the previous element depending in its direction |
| reverse   | reverses the direction of the iterator |
| direction | returs a positive integer for forward direction, negative for backward |

Arrays, streams, and strings are built-in types that implement the iterable interface.

The simple way to iterate the elements of an iterable is via the [foreach loop](StatementsLoop.md). Below is an example showing how to iterate the elements of a string via the foreach loop.
```altro
unicode_chars: char...;
foreach (ch in "greet 世界") 
{
    unicode_chars += ch;
}
```
Here the string "greet 世界" contains 8 characters in a [variable-length character encoding using UTF-8](https://en.wikipedia.org/wiki/UTF-8). The loop iterates in each character in the string (not each byte) and put each character into a stream of characters in 32-bit unicode format.

For any type that implement the iterable interface, you can also iterate their elements by obtaining an iterator from it by calling `begin` or `rbegin` function, and  then iterate through that iterator using the functions provided by the iteractor. Here is an example do the similar thing via the for loop:
```altro
  utf8_string := "greet 世界";
  unicode_chars: char...;
  for (ch:=utf8_string.rbegin(); ch.valid(); ch.next()) 
  {
      unicode_chars += ch;
  }
```
Note that this time the caharacters in `unicode_chars` are in reverse order.

