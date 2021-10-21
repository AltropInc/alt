## Parametric Types

A parametric type is an abstract type. This definition differs from the concept of parametric type in all other programming langauges. Here, a parametric type is a type,
an abstract type, which has its position in the type system's subtype hierarchy and can be used to declare a polymorphic variable. For instance, array is an parametric type with two parameters: element_type and Length:

```altscript
class array #(type element_type: any, length: uint);
```

All concrete array types are subtypes of array:

```altscript
array #(type element_type=int, length=4);  // (0)  or simply,
int[4]
```

In a concrete type of a parametric type, all type paramters are bound either to a type or a value. Type parameters of parametric type can also be reconstrained,
insted of being bound, to form a subtype. For instance:

```altscript
array #(type element_type: numeric);   // (1)
array #(type element_type: integral);  // (2)
```

Where (2) is a subtype of (1), and (1) is a subtype of array (0). Therefore, Alt's type parameters are covariant.

Once a type parameter is bound, it cannot be rebound or reconstrained to form a subtype. Therefore,

```altscript
array #(type element_type=numeric);    // (3)
array #(type element_type=integral);   // (4) 
```
Where (4) is not a subtype of (3), but both (3) and (4) are subtypes of (1), and (4) is a subtype of (1) and (2). int[4] is subtype of (2) but not a subtype of (4). array #(type element_type=numeric) defines a heterougenous numeric which contains numbers mixed of any numeric types, while array #(type element_type : numeric) is an abstract array that represents any numeric array whose element type can be bound to any numeric type, for instance, a double array with all elements in double precision, a long integer array with all elements in long inetgers, or a numeric array with elements mixed of any numeric types (when the element type is bound to numeric.)

Examples:

```altscript
x : int[4];      // array #(type element_type=int; length=4)
y : numeric[4];  // array #(type element_type=numeric; length=4)
z : array #(type element_type: numeric);
z = x;  // okay, z is a polymorphic array reference and now refers to a value of int[4]
y = x;  // error, int[4] is not a subtype of array #(type element_type=numeric, length=4)
        // note: y is not a polymorphic reference although its elements are polymorphic references
z = y;  // okay, z refers to a value of numeric[4];
```

A type parameter is a member of a type. Therefore element_type can be accessed through a type. For instance:

```altscript
type IntArray = array #(type element_type = int; length=4); // or IntArray = int[4];
```

IntArray.element_type is a valid expression that represents the element type of IntArray, and it is int in this case.

An array is a container for elements of the given element type, you can only set the element to the value of its element_type. For instance:

```altscript
z : array;                              // a polymophic reference to any array
z[0] = "text"                           // error, cannot decide whether typeof(z).element_type is string
if (z[0] is string => z0) z0 = "text"   // okay

func ele_assign #(type T: array)(x: T; uint index; ele: T.element_type)
{
    if (index <= T.length)
    {
        x[index] = ele;   // okay
    }
}
x: string[4];
ele_assign(x, 0, "text");   // okay
ele_assign(x, 0, 3.14);     // error
```

