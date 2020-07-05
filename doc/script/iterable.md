# Iterable, Containner and Generator

## Iterable

An Iterable is a collection of element objects that can be visited one by one through an iterator.
Let's say, library is an iterable collection (class name Library) of books, we can do:

```altscript
foreach book in library
{
    // code to do something on book 
}
```

This can be translated into:

```altscript
book_iter := library.begin();  // book_iter: Library.Iterator = library.begin();
while book_iter                // book_iter.valid()
{
    book := *book_iter;        // book: Library.ElementType = book_iter.get();
    // code to do something on book 
    ++book_iter;               // book_iter.next();
}
```

An iterable class has the following members:

| name        | scope  | meaning                                                              |
|:----------- |:------ |:-------------------------------------------------------------------- |
| Iterator    | class  | The iterator class for the iterable                                  |
| ElementType | class  | The element type of the iterable collection                          |
| begin       | object | function returns to the iterator to the first element                |
| rbegin      | object | function returns to the reverse iterator to the last element         |

Note, rbegin returns a valid iterator only when the iterable support bidirectional iteration.

## Container

A Container is an iterable collection that actually stores element values in its collection,
and it can tell the number of elements in the collection without the need to count the elements
through an iteration. The Iterator class of a container has the write capibility, i.e. it can
be used to change the value of the element in the container. Most containers support bidirectional
iteration.

A container has the following additional members:
| name        | scope  | meaning                                                              |
|:----------- |:------ |:-------------------------------------------------------------------- |
| length      | object | function returns number of elements                                  |
| empty       | object | function to tell if the collection is empty                          |

## Generator

A Generator is an iterable collection that generates element values on a certain rule without a
physical storage for the contained elements. The Iterator class of a generator is read only.



