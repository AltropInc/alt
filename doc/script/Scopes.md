# Scope

A declaration introduces a [name](Names.md) in a scope. The scope is the region of a program text in which the declared name is valid. A scope has a lifetime. When the scope is ending its life, all names introduced in the scope will go away and values associated with these names will also go away.

There are four kinds of scopes: instance scope, class scope, input/output scope, and block scope.

The instance scope or the class scope is an owner scope created for the lifetime of its associated owner, i.e. a class instance (object or data) or a class itself. Names introduced in an owner scope are stored in a space allocated in the common value pool (or [heap memeory](https://www.geeksforgeeks.org/stack-vs-heap-memory-allocation/) for its associated owner. When the owner is created, the space required by the storage of the names within the scope are allocated in the pool. When the owner is deleted, the space allocated for the owner is released and all names introduced in the owner scope are gone.

The input/output scope and block scope are local scopes, kind of temporary scopes created for a [routine call](Routine.md). Names introduced in a local scope are stored in a call [stack memeory](https://www.geeksforgeeks.org/stack-vs-heap-memory-allocation/). When the execution enters into a local scope, the space required by the storage of the names within the scope are pushed into the stack. When the execution exits from a local scope, the space pushed for the scope is popped out and all names introduced in the local scope are gone. The stack naturally maintains a nested structure so that local sopes can be nested inside one another.

## Instance Scope

A name, except for a meta name, declared in a class body is local to an instance of this class. If a class defines objects, we often use object scope for instance scope. A name declared in a [singleton body](Singleton.md) is local to this singleton object. Since a singleton implicitly defines a class, meta names in a singleton object are the same as names that are not meta. The class instance or the singleton is the owner object of the name. The name must be accessed through its owner object:
```altro
class test
{
    object foo      // 'foo' is a singleton object.
    {
        n : int;    // The name 'n' is in the object instance scope.
                    // 'n' also belongs to the class scope of the singleton class.
    }
    foo.n = 1;      // Access the name 'n' through its enclosing object 'foo'.
    class bar       // 'bar' is a class.
    {
        m : int;    // The name 'm' is in the object instance scope of 'bar'.
    }
    bar.m = 2;      // This is an error. 'm' must be accessed through an instance of 'bar'.
    b:=bar();       // The name 'b' is in the instance scope of 'test', and initialized with
                    // an instance of 'bar'.
    b.m = 2;        // Access the name 'm' through a 'bar' instance referred by 'b'.
}
```
If the expression of the name appears with the class body, the owner of the name is the [`self`](SelfAndOwner.md) object:
```altro
class test
{
    class bar           // 'bar' is a member class of 'test'.
    {
        m: int;         // The name 'm' is in the object scope of 'bar'.
        m = 1;          // Access the name 'm' through the 'self' object: self.m.
    }
    class sub_bar: bar  // 'sub_bar' is a member class derived from 'bar'.
    {
        m = 1;          // Accessthe  name 'm' in the base through the 'self' object: self.m.
    }
}
```
If the expression of the name appears with a member class body, the owner of the name is the [`owner`](SelfAndOwner.md) object:
```altro
class test
{
    n : int;            // The name 'n' is in the object scope of 'test'.
    class foo           // 'foo' is a member class.
    {
        m: int;         // The name 'm' is in the object scope of 'foo'.
        class bar       // 'bar' is a member class of 'foo'.
        {
            k: int;     // The name 'k' is in the object scope of 'bar'.
            k = 1;      // Access the name 'k' through the 'self' object: self.k.
            m = 1;      // Access the name 'm' through the 'owner' object: owner.m.
            n = 1;      // Access the name 'n' through the 'owner.owner' object: owner.owner.n.
        }
    }
}
```

## Class Scope

Names declared as class parameters or meta members are local to this class. The name is accessed through the (enclosing) class:
```altro
class test
{
    class bar          // 'bar' is a member class of 'test'.
    {
        meta n : int;  // The meta name 'n' is in the object scope of 'bar'.
    }
    bar.n = 2;         // 'n' is accessed through the class 'bar'.
    b:=bar();          // The name 'b' is in the object scope of 'test', and initialized with
                       // an instance of 'bar'.
    b.n = 2;           // Though 'n' is accessed through an instance of 'bar', the actual owner
                       // used to access the meta name 'n' is the type of 'b'. Therefore,
                       // The expression 'b.n' is equivalent to 'bar.n'.
}
```
If the expression of the meta name appears with the class body, the owner of the name is the [`selfclass`](SelfAndOwner.md) type:
```altro
class test
{
    class bar           // 'bar' is a member class.
    {
        meta m: int;    // The name 'm' is in the object scope of 'bar'.
        meta ctor()
        {
            m = 1;      // Access the name 'm' through 'selfclass': selfclass.m.
        }
    }
}
```
If the expression of the meta name appears with a member class body, the owner of the name is the [`ownerclass`](SelfAndOwner.md) type:
```altro
object test
{
    meta n : int;         // The meta name 'n' is in the class scope of the singleton class.
                          // The name 'n' is also in the object scope of the singleton.
    class foo             // 'foo' is a member class of 'test'.
    {
        meta m: int;      // The meta name 'm' is in the class scope of 'foo'.
        class bar         // 'bar' is a member class of 'foo'.
        {
            meta k: int;  // The meta name 'm' is in the class scope of 'bar'.
            meta ctor()   // The meta constructor for 'bar'.
            {
                k = 1;    // access name 'k' through 'selfclass': selfclass.k.
                m = 1;    // access name 'm' through 'ownerclass': ownerclass.m.
                n = 1;    // access name 'n' through 'ownerclass.ownerclass', i.e.,
                          // ownerclass.ownerclass.n
            }
        }
    }
}
```
Please note, a meta member class can only access meta members in its enclosing class. Consider:
```altro
object test
{
    class foo              // 'foo' is a member class of 'test'.
    {
        meta m: int;       // The meta name 'm' is in the class scope of 'foo'.
        n: int;            // The name 'n' in the object scope of 'foo'.
        meta class bar     // 'bar' is a meta member class of 'foo'.
        {
            ctor()         // a constructor for 'bar' instance.
            {
                m = 10;    // Okay, 'm' is a meta member of the enclosing class 'foo'
                n = 10;    // Error: Accessing name 'n' that is not in the class scope
            }
        }
    }
}
```
However, when a meta member class is enclosed in a singleton, then, accessing a meta name or a non-meta name will have no difference:
```altro
object test
{
    object foo              // 'foo' is a singleton object in 'test'.
    {
        meta m: int;       // The meta name 'm' is in the class scope of 'foo'.
        n: int;            // The name 'n' in the object scope of 'foo'.
        meta class bar     // 'bar' is a meta member class of 'foo'.
        {
            ctor()
            {
                m = 10;    // Okay
                n = 10;    // Alos okay 
            }
        }
    }
}
```

## Block Scope

A block is a compound statement, beginning at opening of the block by ‘{‘ and ending at the end of the block by ‘}’. A name declared in a block is local to that block and can be used only in this block or blocks enclosed (nested blocks). It can be used only after the point of the name declaration.

The block scope starts when the program execution enters into the block, and will end of its life when the program execution exits from the block. For instance,

```altro
{                              // A block scope starts.
    s1: string;                // A name 's1' is introduced in the block.
    s2: string;                // A name 's2' is introduced in the bloc.k
    {                          // A nested block starts
        t1:= "Hello, World!"   // A name 't1' is introduced in the inner block, and owns value "Hello, World!"
        t2:= "Hello, World2!"  // A name 't2' is introduced in the inner block, and owns value "Hello, World2!"
        s1 = t1;               // The name 't1' transfers its value to 's1' declared in outer scope
        s2 = t2.substring(7);  // The name 's2' gets a copy of substring from t2, "World2!"
    }                          // The inner block ends, and t2 and its value dropped.
                               // The name t1 is also dropped but it does not own the value because its ownership was transferred to s1.       
}                              // The outer scope exits, name s1 and s2 and their values are dropped.
```
For ownership transfer among names in different scopes, see [Ownership](Ownership.md).

## Input/Output Scope

The input/output scope (abbreviation: IO scope) is an input and output parameter scope for [routine](Routine.md) instantiation. When we create an object through a [constructor call](Constructor.md) or do a [function call](Functor.md), we provide a number of input parmaters and expect an output. Names of input paramters in the IO block never owns the value unless the provided actual parameter is a literal value or it holds value of pass-by-value type (primitive values). The output paramter is a hidden name. It can either owns its own value or refer to a value passed from one of input paramters or a value owned by names in object or class scope. If the value is owned by the output and is not transfered on exit of the IO scope, the output value will be dropped. The output cannot refer to a value owned by any name introduced in the function body block scope.

For instance

```altro
func hello (x: string) : string
{
   str := "Hello ";
   str + x;     // return the result of x+y
}
func clone (x: string) : string
{
   x;     // return the value x
}
```

When the function hello is called, say hello("World"), we get the following scopes:

```altro
// for s:= hello("World");       // Call 'hello' with a string literal
hello:
{                                // The IO scope of the function 'hello' starts here
    x:= "World";                 // x refers to the string literal "World"
    output: string;              // The output is a hidden name
    {                            // A nested block scope of the function 'hello' starts here
        str := "Hello ";         // A name 'str' is introduced in the block scope and refers "Hello " 
        output = hello + x;      // 'output' owns the result of hello + x and refers to "Hello World"
    }                            // A nested block scope ends here and the name 'str' is no longer valid.
    s := output;                 // The output value is transferred to to 's'.
}                                // The IO scope ends here and the name 'x' is no longer valid.
                                 // If the output value is not transferred to other name after the
                                 // function call, the value referered by 'output' will be dropped
```

## Scope Selector
If a name decalred in an outer scope is obscured by the same name in an innder scope, it can be still used when it is qualified by the name of its enclosing class with a scope select operator ::

## self, owner, selfclass, ownerclass, and root
When the self appears in the block scope of a class constructor (enter block), it refers to the instance of this class. When the owner appears
in the block scope of an enter block of a member class, it refers the instance of the class that owns the member. For instance:

```altro
class A
{
    class bar is func
    {
        enter (x: string) : string
        {
            self;   // refers to the instance of bar
            owner;  // refers to the instance of A
        }
    }
    
    func foo (x: string) : string
    {
       self;   // refers to the instance of foo
       owner;  // refers to the instance of A
    }
}
```
The self in func foo is potentially surprising and thus worth noting. Altro treats a function as a class, and a member function as a
member class. Thus declarations of bar and foo within the class A are equivalent. This is an important difference than any of the other 
OO languages. Since foo is a class, the instance of foo can have its own members and the self reference within foo is used to access foo's
member. This unification of function and class opens door to user-defined functional objects.

The selfclass appears in the block scope of a class's enter block referes to the atucal class of the instance of this class. The ownerclass
appears in the block scope of a member class's enter block referes to the actual class from witch the instance that owns the member is created.
For instance:

```altro
class A
{
    class bar is func
    {
        enter (x: string) : string
        {
            selfclass;   // refers to bar because any subclass of func is final
            ownerclass;  // refers to the class of the instance that owns the bar instance
                         // It is not necessarily A, rather, can be any subclass of A
        }
    }
    
    enter()
    {
        selfclass;   // refers to the class of the instance currently executing the enter block, and
                     // and it can any subclass of A
    }
}
```

The root reference refers to an implicit global object. An Alt application (executable) has one and only one root object. All application objects
are created within the root object.
