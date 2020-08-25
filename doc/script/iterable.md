# Iterable, Containner and Generator

## Iterable and Iterator

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

Iterator is a member class of iterable in class scope. i.e. an Iterater class is always accessed
through a iterable class: IterableClass.Iterator. For instance:

```altscript
type T = tuple (x:int, y:double, z: string);
t : T = (1, 2.1, "text");
t_first : T.Iterator = t.begin();
t_last : T.Iterator = t.rbegin();
```

An iteractor has the following member classes:

| name     | scope  | meaning                                                              |
|:-------- |:------ |:-------------------------------------------------------------------- |
| value    | object | get the value/object referred by the iterator                        |
| ended    | object | tells if the iterator reaches to the end and it referes to null      |
| index    | object | returns the order of the element the iterator refers in the iterable |
| name     | object | returns the name, if any, of he element in the iterable              |

An example of usage:

```altscript
t : tuple (x:int, y:double, z: string) = (1, 2.1, "text");
for (iter:=t.begin(); !iter.ended(); ++iter)
{
    system.out.println(iter.index(), ' ', iter.nmae(), ": ", iter.value())
}
```
The output is:
```altscript
1 x: 1
2 y: 2.1
3 z: text
```


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



