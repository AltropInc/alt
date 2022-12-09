# Convertible Types

[Type conversion or typecasting](https://en.wikibooks.org/wiki/Computer_Programming/Type_conversion) refers to changing the value of one data type into another. **Convertible type rules** determine if the value of one type can be converted to the value of another type. There are two kinds of type conversions: **implicit conversion** and **explicit type-casting**. Implicit conversions are automatically performed when a value is copied to a convertible type. For example:
```altro
a: short = 10;
b: int = a;
```
Here, the value of `a` is promoted from `short` to `int` without the need of any explicit type conversion. Implicit conversions affect some fundamental data type such as numerical types, character types, and some composite types and object references. Converting to `int` from some smaller integer type, or to `double` from `float` is guaranteed to produce an integrated value in the destination type without any information loss. But not all conversions between types may preserve this integrity. Some of these conversions may imply a loss of precision, for example, convert a `int` value to a `short`, or a `float` value to an integer. These conversions will not be performed in an implicit way, and explicit [type-casting](Typecasting.md) must be used to force the conversion:
```altro
a: int = 10;
b: short = cast(short, a);
```

## Numeric Conversion

[Numeric types](TypeNumeric.md) such as integers and floating numbers are convertible to each other. Numeric type conversion can be classified into widening type conversion and narrowing type conversion.

### Widening Conversion

**Widening Conversion** converts a smaller numeric type to a larger or the same type size without data loss. Or in other words, converting a value of T1 to a value of T2 is a widening conversion if we can convert the value of T2 back to the original value of T1. For example, converting a 4-byte integer to a 8-byte integer is a widening conversion because a 8-byte integer is able to hold any value of a 4-byte integer.

Altro performs widening conversion automatically. The following automatic widening conversions are supported:
```
tiny  ⟶ short  ⟶ int  ⟶ long  ⟶ llong  ──┐
 ⥮         ⥮        ⥮       ⥮        ⥮      ├──► float ⟶ double ⟶ ldouble
utiny ⟶ ushort ⟶ uint ⟶ ulong ⟶ ullong ──┘
```
In the above chart, for example, the value of a tiny or utiny can be automatically converted to any other primitive numeric type. Note that the conversion between signed and unsigned may cause sign change, but the bitwise data is retained because the value of the original type can be recovered when we convert the value back. For example,
```altro
  x: short = -2;
  y: ushort = x;  // after converting from ushort to short, y gets value 65534
  z: short = y;   // after converting back from ushort to short, z gets value -2
```

### Narrowing Conversion

On the other hand, **Narrowing Conversion** is a typecasting that may cause data loss and the converted value may not be able to be converted back to the original value. For example, converting a 8-byte integer to a 4-byte integer is a narrowing conversion.
 
Narrowing conversions must be explicitly stated, otherwise, Altro parser will report an error about potential data loss. Explicit [type cast](TypeCast.md) can be stated by [cast expression](Expressions.md), for instance,
```altro
i: int = 65537;
j: short = cast(short, i);
```
Here the 4-byte integer value `65537` is forced to be converted into a 2-byte integer and the value in j is 1, which is a result of data loss. Without `cast`, an error will be reported.

Converting a floating-point number to an integer is always a narrowing conversion because the fractional portion of the floating-point number is discarded and lost during the conversion. Explicit cast must be used for this conversion:
```altro
x = 2.14;
y: int = cast(int, x);   // y gets value 2
```

### Type Promotion

**Type promotion** is an automatic widening conversion when a binary operation is used on two operands and one of the operand will be promoted (widened) to macth the other. For instance, in the expression
```altro
4 + 4.2
```
The owner (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. On the other hand,
```altro
4.2 + 4
```
The input (integer 4) will be promoted to double before the expression is executed. And the result of the execution will be in the type of double. For type promotion rules, see [Type Promotion](TypePromotion.md).

Type promotion can also happen when a prefix operation is used on an operand (the owner that performs the operator) and the operand will be promoted to the type required by the operator. For example, in the prefix expression `∛9`, the operand is an integer `9`, which is promoted to a value of double `9.0` before the operator `∛` is applied.

## Charater and String Conversion

The character type `char` (32-bit unicode) and `utf8` (UTF-8 encoded char) are interchangeable:
```altro
c1: utf8 = 'A';
c2: char = c1;
```
A character cannot be converted to an integer. The method `code` must be called in ordder to get its integral code:
```altro
c: char = 'A';
A_code : uint = 'A'.code();  // A_code gets value 65
```
Because `char` and `utf8` are interchangeable, a string and a stream of `char` are also interchangeable:
```altro
ss: char... = "Hello 世界";
s: string = ss;
```
Here `ss` is a stream that contains eight 32-bit unicode characters which is initialialized by the value converted from an UTF-8 string "Hello 世界". And `s` is a string that gets the initial value converted from a stream of characters contained in `ss`.

## Tuple, Array, and Stream Conversion

Values of tuple, array and stream can be converted to each other under certain conditions.

### Tuple to Tuple Conversion

A value of tuple type T1 can be converted to another tuple type T2 if the number of elements contained in T1 is equal or greater than the number of elements contained in T2, and each tuple element in T1, if used in conversion,  is convertible to the type of the corresponding element in T2. For instance:
```altro
t1: (int; string) = (1, "text");
t2: (double; char[]) = t1;
```
In the above conversion, the integer value of the first element in t1 is convereted the double value for the first element in T2; and string value of the second element in t1 is convereted the array of char for the second element in T2. Note that `char[]` is an abstarct array type, and the actual array type convereted is `char[4]`.

If element conversion involves narrowing conversion, explicit type cast must be used:
```altro
t1: (double; string) = (1, "text");
t2: (int; char[]) = cast((int; char[]), t1);
```
### Tuple to Array Conversion

A tuple value can be converted to a array if the number of elements contained in the tuple value is equal or greater than the length of the array, and each tuple element, if used in conversion,  is convertible to the element type of the array. For instance:
```altro
t: (int; int; string) = (1, 2, "text");
a: int[2] = t;
```
If element conversion involves narrowing conversion, explicit type cast must be used:
```altro
t: (int; int; string) = (1, 2, "text");
a: short[2] = cast(short[2], t);
```

### Reference Conversion (Upcasting and Downcasting)

A variable of [non-value class](ValueClass.md) is actually a [reference variable](TypeReference.md) that points to the instance (object or value) of the class stored in a heap space. Whenever an instance of non-value class is passed from one variable to another, a copy of the reference value is created which points to the same location of the instance in heap memory as the original reference variable. This process is called [passed by reference](ArgumentPassing.md).

A reference variable in a non-value class can pass its reference value to another reference variable in a different non-value class. For example:
```altro
class Person {}
class Student is person {}
s := Student();
p : Person = s;
```
Here the reference to a `Student` instance is generalized to a `Person` instance without the need of any explicit type conversion. Implicit reference conversion is supported when passing a reference of a subtype to a reference variable declared in its supertype. This reference conversion is also referred as **upcasting**(generalization or widening), which is a casting from a lower type to a higher type in the class [inheritance tree](InheritanceTree.md) or in the super/subtype tree. 

On the other hand, passing a reference of a supertype to a reference variable declared in one of its subtype is called **downcasting** (specialization or narrowing), which cannot be done automatically. Explicit type cast must be used:
```altro
p : Person = Student();
s : Student = cast(Student, p);
t : Teacher = cast(Teacher, p);
```
The cast expression is used here to check the type  of 'p': if `p` refers to the instance of the type given in the cast expression, the reference will be passed, otherwise, a null reference will be passed. In the above example, `s` gets a reference of `Student` referred to by `p`, but `t` gets a null reference.


