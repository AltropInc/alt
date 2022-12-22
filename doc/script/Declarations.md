# Name Declaration

A declaration specifies a unique name for the instance of a type, it provides information about what type of instances the name can hold, and optionally the initial instance for the name. The point at which a name is declared is the point at which it becomes visible, or in other words, you can't refer to a name that is declared at some later point in the program context.

In a typical name declaration, you provide the name, its type and optionally the initial value:
```altro
price: double = 3.45;
  │      │        ╰────── initial value
  │      ╰─────────────── type of the value
  ╰────────────────────── the name
```
If you provide the initial value, the type for the name will be automatically inferred:
```altro
price := 3.45;
  │       ╰────── initial value, the type of the name is automatically inferred as double
  ╰────────────── the name
```
When you declare a name, a memory location is set aside for that type and the name is associated with that location, for instance, an double value gets 8 byte of space. The place where the memory allocated is depending on the [scope](Scopes.md) where the name is declared. A name declared in an owner scope (instance scope or class scope) stores value in a location allocated in the common value pool (or heap memeory) attached to its associated owner. When the owner is created, the space required by the storage of the names within the scope are allocated in the pool. When the owner is deleted, the space allocated for the owner is released and all names introduced in the owner scope are gone. Names introduced in a local scope (input/output scope and block scope) store values in a call stack memeory. When the execution enters into a local scope, the space required by the storage of the names within the scope are pushed into the stack. When the execution exits from a local scope, the space pushed for the scope is popped out and all names introduced in the local scope are gone. 

## Name Specifiers

The following name specifiers are provided to specify the attributes associated with a declared name:

**const**<br>
**type**<br>
**meta**<br>
**deferred**<br>

The `const` specifier tells that a name's value is constant and the programmer cannot modify the value after it gets its initial value. Therefore, a const name declaration always has an initial value unless it is deferred in an owner scope or it is declared in an input/output scope.
```altro
const price: double = 3.45;
price = 1.25;   // Error: cannot alter the value of a constant name
```
The `type` specifier tells that a name's value is a type, which is typically used for declaration of a [class parameter](ParametricClass.md).
```altro
type price_type: numeric;
```
The `meta` specifier is used in class body for member name declaration and it tells the name is declared in the class scope, not in the instance scope.
```altro
meta min_price := 0.0;
meta max_price := 1000000.0;
```
The `deferred` specifier is used in class body for a member name declaration and it tells the name will be initialized later in a derived class.
```altro
interface class numeric is comparable
{
   deferred const max_val: numeric; 
}
interface class real is numeric { }
value sealed class double is real
{
   max_val = 1.79769313486231570e+308; 
}
```

## Parallel Name Declaration

You can introduce more than one name in a single declaration:
```altro
x, y, z: int;
```
To get initial values for each name, use an expression list:
```altro
x, y, z: int = 1, 2, 3;
```
Each expression in the initialization list is corresponding to the name in the order of the declaration. If the number of expressions is less than the number of the declared names, the name has no corresponding expression is uninitialized. 
```altro
x, y, z: int = 1;
```
Here only the name `x` is initialized. However, if the type information must be inferred from the declaration, the number of expressions must be eaqual to the number of the declared names:
```altro
x, y, z := 1, "text", math.π;
```
The following declaration is not accepted:
```altro
x, y, z := 1, "text";  // Error: type cannot be inferred for the name: z
```
Providing more expressions than needed is considered an error.

You can also provide a tuple value to initialize all names in the declared name list:
```altro
x, y, z: int = (1, 3, 4);
```
The tuple value is unpacked for each name, and the number of elements of the tuple value must be eaqual to the number of declared names. Using tuple value is a convenient way to use a function output for parallel initialization. For example:
```altro
func foo(): (int; string) { (0, "hello") }
a, b := foo();
```
Here the function foo returns a tuple value in the type of (int; string), and the declared name `a` and `b` are initialized with the first and the second element of the tuple value respectively.

Note that the tuple value is unpacked only when the right side of the parallel declaration assignment contains one expression in a tuple value. If the right side contains a list of expressions, the tuple value will not be unpacked, and the names declared at the left side will be initialized respectively with the expression in the expression list:
```altro
func foo(): (int; string) { (0, "hello") }
a, b := foo(), 3;
```
This is equivalent to:
func foo(): (int; string) { (0, "hello") }
a: (int; string) = foo();
b: int = 3;

## Polymophic Name Declaration

A name is called polymorphic if it can refer to values of different types under different conditions. A polymorphic name is often declared by an abstract type to take values of its subtypes. For instance:
```altro
n: array;      // a polymorphic name n that can take values of any array type
n = (1,2,3);   // n takes an array of integer in the type of int[3]
i : int;
if (n -> t is int[])   // test is n currently refers to an integer array of any length
{
    i = t[1];  // assign the value of second element in the array to integer name i
}
```
In the above example, `n` is a polymorphic name that can refer to a value in any [array type](Array.md). A polymorphic name not only stores value or reference to value, but also stores the type information of the value. When the name `n` takes the value `(1,2,3)`, it contains a reference to the value as well as the type information for the type `int[3]`. The [type checking](TypeIntrospection.md) `n -> t is int[]` is used to check if the value referred by `n` is actually an array of integers

The instance of an [object class](Object.md), or simply, an object, contains its type information. A name declared in an object class may refer to an object of any subclass of the object class. Therefore, a name declared in an object class is polymorphic by itself.
