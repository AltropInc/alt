## Parametric Types

A parametric type is an abstract type. This definition differs from the concept of parametric type in other programming langauges. A parametric type is a type,
which has its position the type system's subtype hierarchy and can be used declare a polymorphic variable. For instance, array is an parametric type with two
parameters: ElementType and Length:

```altscript
class array #(type element_type: any, length: uint);
```

All concreate array types are subtypes of array:

```altscript
array of (int, 4);  // or simply,
int[4]
```

In a concrete type of a parametric type, all type paramters are boundeither to a type or a vlaue. Type parameters of parametric type can also be reconstrained,
but not bound, to form a a subtype. For instance:

```altscript
array #(type element_type: numeric);   // (1)
array #(type element_type: integral);  // (2)
```

Where (2) is a subtype of (1), and (1) is a subtype of array. Therefore, Alt's type parameters are covariant. However, once a type parameter is bound, it cannot be
rebound or recontrained to form a subtype. Therefore,

```altscript
array #(type element_type=numeric);    // (3)
array #(type element_type=integral);   // (4) 
```
Where (4) is not a subtype of (3), but (3) and (4) are subtypes of (1), and (4) is a subtype of (2). int[4] is subtype of (2) but not a subtype of (4). Consider:

```altscript
x : int[4];
y : numeric[4];
z : array #(type element_type: numeric);
z = x;  // okay, z is a polymorphic array reference now refers to a value of int[4]
y = x;  // error, int[4] is not a subtype of array #(type element_type=numeric, length=4)
        // note: y is not polymorphic reference though its elements are polymorphic references
z = y;  // okay, z refers to a value of numeric[4];
```

A type paramter is a member of a type. Therefore array.element_type is a valid expression. An array is a conatainer for elements of the same type,
you can set the element to the value of its element_type. For instance:

```altscript
z : array;       // a polymophic reference to any array
z[0] = "text"    // error, cannot decide whether typeof(z).element_type is string
if (z[0] is string => z0) z0 = "text"   // okay
func ele_assign(x: array, uint index, ele: typeof(x).element_type)
{
    if (index <= typeof(x).length)
    {
        x[index] = ele;   // okay
    }
}
x: string[4];
ele_assign(x,0,"text");   // okay
ele_assign(x,0,3.14);     // error
```

