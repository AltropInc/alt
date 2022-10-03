# Stateless Class

A stateless class is a data class that has no data members in instance scope,  thus an instances of a stateless class has no data members, and its storage size is zero. In addition, a stateless class cannot have constructors and destructor, but the class may have meta members in class scope, and can also have functors.

There is no keyword reserved for stateless classes as long as they are not inherited from the `object` class and they meet the decription above. The `object` class cannot be a stateless class because instances (objects) of an object class resides in an object tree with parent and children nodes.

A stateless class is often used to define functors so that calling these functors defined in the class (function call) does not require an instance of the class. Instead, you call the functor directly using the class name. See [Function Call Using Class Name](Functor.md).

The class `func` is a built-in stateless functional class which implements a calling protocol using an efficient and sequential procedure based on the call stack technique for a traditional function call. Using the `func` class, we can derive subclasses in terms of functions:
```altro
func sum(values: int...): int
{
    sum := 0;
    foreach (v in values) sum += v;
    sum
}
```
where `sum is a subclass derived from `func` with a functor in the interface `(int...):int`. Calling the functor defined in `sum` can use the class `sum` directly without instantiation becuase `sum` is an stateless class:
```altro
sum(1,2,3,4)
```
For more information about functions, see [Function Class and Functions](FunctionClass.md)
