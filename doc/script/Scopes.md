# Scope

A decalration introduce a name in a scope. The scope is the region of a program text in which the declaration of the name is valid.
A scope has a life time. When the scope is ending of its life, all names introduced in the scope will go away and values associated
with these name will also go away.

There are four kinds of scope: block, IO, object, and class.

## Block Scope.
A block is a compound statement, beginning at opening of the block by ‘{‘ and ending at the end of the block by ‘}’. A name declared
in a block is local to that block and can be used only in this block or blocks enclosed (nested blocks). It can be used only after
the point of the declaration.

The block scope starts when the program execution enters into the block, and will end of its life when the program execution exits
from the block. For instance

```altscript
{                              // a block scope starts
    s1: string;                // a name 's1' is introduced in the block
    s2: string;                // a name 's2' is introduced in the block
    {                          // a nested block starts
        t1:= "Hello, World!"   // a name 't1' is introduced in the inner block, and owns value "Hello, World!"
        t2:= "Hello, World2!"  // a name 't2' is introduced in the inner block, and owns value "Hello, World2!"
        s1 = t1;               // name 't1' transfers its value to 's1' declared in outer scope
        s2 = t2.clone();       // name 's2' get a value copy from t2. This can be written as s2 = *t2;
    }                          // the inner block ends, and t2 and its value dropped.
                               // t1 is also dropped but it has no value because its value was transferred to s1          
}                              // the outer scope exits, name s1 and s2 and their values are dropped.
```
## IO Scope.
The IO scope is an object instantiation input and output parameter scope. When we create an object or do a function call,
we provide a number of input parmaters and expect an output. Names of input paramters in the IO block nevers owns the value
unless the provided actual parameter is a literal value or it holds value of pass-by-value (primitive values).
The output paramter is a hidden name. It can either owns its own value or refer to a value passed from one of input paramters
or a value owned by names in object or class scope. If the value is owned by the output and is not transfered on exit of the
IO scope, the output value will be dropped. The output cannot refer to a value owned by any name introduced in the function
body block scope.

For instance

```altscript
func hello (x: string) : string
{
   hello := "Hello ";
   hello + x;     // return the result of x+y
}
func clone (x: string) : string
{
   x;     // return the value x
}
```

When the function hello is called, say hello("World"), we get the floowing block scope:

```altscript
// for hello("World");
hello:
{                                // the IO scope of the function hello starts here
    x:= "World";                 // x owns "World" because the actual paramter is a literal
    output: string;              // the output is a hidden name
    {                            // the body block scope of the function some_string_op starts here
        hello := "Hello ";
        output = hello + x;      // output owns the result of hello + x, which is "Hello World"
        exit hello;              // exit to the outer IO scope
    }                            // the name hello and its value is dropped here
}                                // The name 'x' and its value dropped.
                                 // The output value is dropped if the valu is not transferred on exit
                                 
// for s:="World"; hello(s);
hello:
{                                // the IO scope of the function hello starts here
    x:= s;                       // x refers to the value of s, but does not own it
    output: string;              // the output is a hidden name
    {                            // the body block scope of the function some_string_op starts here
        hello := "Hello ";
        output = hello + x;      // output owns the result of hello + x, which is "Hello World"
        exit hello;              // exit to the outer IO scope
    }                            // the name hello and its value is dropped here
}                                // The name 'x' is dropped, but not the value it refers to
                                 // The output value is dropped if the valu is not transferred on exit
                       
// clone("World");
clone:
{                                // the IO scope of the function hello starts here
    x:= s;                       // x refers to the value of s, but does not own it
    output: string;              // the output is a hidden name
    {                            // the body block scope of the function some_string_op starts here
        output = x;              // output copies the value for x because x does not owns the value
        exit hello;              // exit to the outer IO scope
    }                            // the name hello and its value is dropped here
}                                // The name 'x' is dropped, but not the value it refers to
                                 // The output value is dropped if the valu is not transferred on exit

// for s:="World"; clone(s);
clone:
{                                // the IO scope of the function hello starts here
    x:= "World";                 // x owns "World" because the actual paramter is a literal
    output: string;              // the output is a hidden name
    {                            // the body block scope of the function some_string_op starts here
        output = x;              // output owns the value of x, and x no longer has the value
        exit hello;              // exit to the outer IO scope
    }                            // the name hello and its value is dropped here
}                                // The name 'x' is dropped, but not the value
                                 // The output value is dropped if the valu is not transferred on exit

```

## Object Scope.
A name declared in a class body is local to an instance of this class. The name must be used with an instance (the enclosing object)
of this class. It can be used (1) in classes enclosed by this class (nested classes); (2) in classes derived (subclasses) from this class;
and (3) after the “.” operator applied to an instance of this class or of a derived class. When the name is used in cases of (1) and (2),
the enclosing object is self.

The value of the name in a object scope can never be transfered, i.e. the name in an object scope always owns its value. A name in a
block scope can never grab the ownership of a name in a object scope. Names in object scope are reference counted and the object scope
can only goes away when no other names referes to the value of any name in the object scope.

## Class Scope.
Names declared as class parameters or meta members are local to its class. The name must be used with the (enclosing) class.
It can be used (1) in classes enclosed by this class (nested classes); (2) in classes derived (subclasses) from this class; and (3)
after the “.” operator applied to an this class or a derived class. When the name is used in cases of (1) and (2), the enclosing class
is selfclass.

## Scope Selector
If a name decalred in an outer scope is obscured by the same name in an innder scope, it can be still used when it is qualified by the
name of its enclosing class with a scope select operator ::

## self, owner, selfclass, ownerclass, and root
When the self appears in the block scope of a class constructor (enter block), it refers to the instance of this class. When the owner appears
in the block scope of an enter block of a member class, it refers the instance of the class that owns the member. For instance:

```altscript
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
The self in func foo is potentially surprising and thus worth noting. Altscript treats a function as a class, and a member function as a
member class. Thus declarations of bar and foo within the class A are equivalent. This is an important difference than any of the other 
OO languages. Since foo is a class, the instance of foo can have its own members and the self reference within foo is used to access foo's
member. This unification of function and class opens door to user-defined functional objects.

The selfclass appears in the block scope of a class's enter block referes to the atucal class of the instance of this class. The ownerclass
appears in the block scope of a member class's enter block referes to the actual class from witch the instance that owns the member is created.
For instance:

```altscript
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

The root reference refers to an implicit global object. An Alt application (executable has one and only one root object. All application objects
are created with the root object.
