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
| value     | gest the value/object referred by the iterator (dereferencing)          |
| ended     | tells if the iterator reaches to the end and it referes to null         |
| valid     | tells if the iterator points to a valid element, valid()=!ended()       |
| index     | returns the order of the element the iterator refers to in the iterable |
| next      | advances the iterator for the next or the previous element depending in its direction |
| reverse   | reverses the direction of the iterator |
| direction | returs a positive integer for forward direction, negative for backward |

Dereferencing an iterator by calling `value` is unnecessary in most cases where dereferencing is done implicitly. However, if there is a member name conflict, explicit dereferencing has to be used to resolve the conflict. For instance, if the type of the value referred by an iterator `iter` has the same member name, say, `reverse` used in iterator itself, then you have to write `iter.value().reverse()` to indicate that the member function `reverse` in the value is called, rather than the `reverse` of the iterator itself as in `iter.reverse()`.

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

For any type that implement the iterable interface, you can also iterate their elements by obtaining an iterator from it by calling `begin` or `rbegin` function, and  then iterate through that iterator using the functions provided by the iteractor. Here is an example do the similar thing via the for loop, and this time characters in the `unicode_chars` are in reverse order:
```altro
  utf8_string := "greet 世界";
  unicode_chars: char...;
  for (ch:=utf8_string.rbegin(); ch.valid(); ch.next()) 
  {
      unicode_chars += ch;
  }
```
Note that the iterator `ch` is used the same way as a [reference](TypeReference.md). The iterator `ch` points to the first byte of a byte sequence for a character in UTF-8 encoding and dereferencing iterator `ch` to get the value of the character (in `utf8` type) is done implicitly. Explicit dereferencing by calling `ch.value()` is not necessary. Also, note that the conversion from `utf8` to `char` (32-bit unicode character type) is also done implicitly here.

## Iteration on Tuple and Object


