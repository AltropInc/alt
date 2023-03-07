# Assignment

An assignment sets or resets the value stored or referred in the storage location denoted by a variable name, or in other words, it copies a value or reference into the storage location of the variable. An assignment consists of a left part and a right part separated by an assignment operator. The right part is an expression evaluated to a value in the current state of the program, and the left part is a name expression evaluated to a value storage location, often denoted by a variable name. The assignment operator determines how the value evaluated from the right part is copied into the location evaluated from the left part.

This topic will cover the following:

| Terminology            | Notation           | Description                                                                        |
|:---------------------- |:------------------ |:---------------------------------------------------------------------------------- |
| assignment             | a = expr           | default assignment using copy by value for value classes or by reference otherwise |
| copy assignment        | a @= expr          | copy assignment using copy by value only and the copy is a shallow copy            |
| deep copy assignment   | a @@= expr         | copy assignment using copy by value only and the copy is a deep copy               |
| declaration assignment | a := expr          | a new name declaration being assigned with an initial value or reference           |
| tag assignment         | expr -> a          | a declaration of tag name used as an alias for an expression                       |
| selective assignment   | a <- (expr1,expr2) | assigns the value of 'expr' to 'a' only if 'a' is in the specified type            |
| parallel assignment    | (a, b) = expr      | an assignment in which several variables to be assigned in parallel                |
| parallel declaration   | a, b := expr       | a set of new name declaredn and assigned with an initial composite value           |
| augmented assignment   | a += expr          | an assignment with additional operator to alter the value stored in a variable     |
| chained assignment     | a = b = expr       | the value of expr is assigned to multiple variables a, b, etc.                     |

## Assignment `a = expr` (Default Assignment)

When you assign a variable, if it is the value copied into the variable, this behavior is called **copy by value**. If the value is an instance of a [value class](ValueClass.md) (examples are integers, characters, and enumeration values), the default assignment behavior is copy by value. For example, let `a` and `b` be two variables to store integer values and each has an initial value, 1 and 2 respectively:
```altro
a := 1; 
b := 2;
```
After assignment `a = b`, the value stored in `b`, the integer `2`, is copied into `a`. As a result, the value stored in `a` is now `2`. Although both `a` and `b` hold value `2`, they are independent values, that is, assigning `a` to another different value will have no effect on the value stored in `b`.

When you assign a variable, if it is the reference to a value not the value itself, copied into the variable, this behavior is called **copy by reference**. If the value is an instance of a non value class (examples are arrays, streams, strings, tuples, and objects), the default assignment behavior is copy by reference. For example, let `p` and `q` be two variables to store references to integer arrays. `a` has a null reference and `b` has a reference to an array value `(1,2,3,4)`:
```altro
a : int[4]; 
b : int[4] = (1,2,3,4);
```
After assignment `a = b`, the reference stored in `b` is copied into `a`. As result, the reference stored in `a` refers to the same array value referred by `b`:
```
a ──────┐
        ├───🡪 (1,2,3,4)
b ──────┘
```
When we change array value through `a`:
```altro
a[0] = 5;
```
the value referred by `b` is also changed because it is the same value:
```
a ──────┐
        ├───🡪 (5,2,3,4)
b ──────┘
```
When you assign `a` to another array reference, say, `a = (6,7,8,9)`, the reference stored in `b` is unchanged:
```
a ──────────🡪 (6,7,8,9)
b ──────────🡪 (5,2,3,4)
```
However, when you assign `b` to another array reference, say, `b = (6,7,8,9)`, because `b` is the owner of the value `(5,2,3,4)`, the value of `(5,2,3,4)` is destroyed before `b` takes the new reference. If `a` refers to the same value `(5,2,3,4)` before, the reference in `a` will be nullified, and `a` won't refer to the new value 
`6,7,8,9)`:
```
Before the assinment:
      a ──────┐
              ├───🡪 (5,2,3,4)
      b ──────┘
After b = (6,7,8,9):
      a ──────────🡪 null
      b ──────────🡪 (6,7,8,9)
```
For more information about the ownership, see [Ownership](Ownership.md).

## Copy Assignment `a @= expr`

A copy assignment uses copy by value only, as a result, the destination gets a distinct shallow copy of the original value. A shallow copy is a copy of a value in which references to element values, if any, are not copied, that is, if the value is composite and contains references to refer to another value then in such cases only the reference is copied and the copied reference does not own the value. Let `b` a reference to a tuple value ("abc", "xyz", "123), and `a` a null reference to a tuple of strings:
```altro
Before the assignment:
a : (string; string; string);
b := ("abc", "xyz", "123);
```
After the assignment `a @= b`, we have:
```
a ───🡪 null
b ───🡪 ( ◯     ◯      ◯ )  ⭠ a tuple value that contains three references
          🡫     🡫      🡫     ⭠ each reference owns the value that they are referring
        "abc"   "xyz"  "123"
```
After the assignment `a @= b`, we get:
```
b ───🡪 ( ◯     ◯      ◯ )
          🡫     🡫      🡫     ⭠ references that owns what they are referring
        "abc"   "xyz"  "123"
          🡩     🡩      🡩     ⭠ references that does not own what they are referring
a ───🡪 ( ◯     ◯      ◯ )
```
Here `a` has a reference to a triple string references that points to the same string values pointed by references referred by `b`. Note that `a` owns the tuple value that contains three references, whereas the references contained in the tuple referred by `a` does not own the string value. If we have `b` set to null, all strings owned by references contained in the tuple value referred by `b` are gone, and `a` still owns the tuple value  but all three references contained in the tuple becomes null:
```
b ───🡪 null 
a ───🡪 ( ◯     ◯      ◯ )
```

## Deep Copy Assignment `a @@= expr`

A deep copy assignment also uses copy by value only, as a result, the destination gets a distinct deep copy of the original value. Deep copy is a process in which the copying process occurs recursively, that is, If the value contains a reference to a value and the reference owns the value, a deep copy of the value is created within its conataining value. How ever, if the contained reference does not own the value, the reference itsef is copied and the copied reference does not own the value either. Let `b` a reference to a tuple value ("abc", "xyz", "123), and `a` a null reference to a tuple of strings:
```altro
a : (string; string; string);
b := ("abc", "xyz", "123);
```
Before the assignment `a @@= b`, we have:
```
a ───🡪 null
b ───🡪 ( ◯     ◯      ◯ )  ⭠ a tuple value that contains three references
          🡫     🡫      🡫     ⭠ each reference owns the value that they are referring
        "abc"   "xyz"  "123"
```
After the assignment `a @@= b`, we get:
```altro
b ───🡪 ( ◯     ◯      ◯ )
          🡫     🡫      🡫     ⭠ references that owns what they are referring
        "abc"   "xyz"  "123"
        
a ───🡪 ( ◯     ◯      ◯ )
          🡫     🡫      🡫     ⭠ references that owns what they are referring
        "abc"   "xyz"  "123"

```

## Declaration Assignment `a := expr`

A declaration assignment `a := expr` is a default assignment applied as an initialization to a new variable name introduced in this declaration assignment statement. It is equivalent to:
```
a : TypeOfExpr = expr;
```
For example, the declaration assignment:
```altro
s := (1,2,3,4);
```
introduces a new variable name `s`. Its type is automatically inferred as `tuple(int;int;int;int)`. This declaration assignment is equivalent to:
```altro
s: tuple(int;int;int;int) = (1,2,3,4);
```
In case the auto deducted type is not the one expected, then an explicit type must be provided in the declaration:
```altro
s: int... = (1,2,3,4);
```
Here `s` is declared as an integer stream, and the tuple value (1,2,3,4) is then converted to an integer stream. However, you can also explicitly denote the type at the right side of the assignment expression:
```altro
s := int...(1,2,3,4);
```

## Tag Assignment `expr -> a`

A tag assignment `expr -> a` is a default assignment of an expression value to a tag name introduced in a branched or conditional statement, and the tag name can be used as an alias for the expression value in the block associated with the condition. Let's say a function `foo` that returns an integer value when it is called, and in the branch control expression of the following switch statement:
```
switch (foo()->a)
{
    case 1: /* do something for the value 1 */;
    case 2: /* do something for the value 2 */;
    default: /* do something for the value assigned to the tag a */
}
```
The value from the function call `foo()` is assigned to a tag `a`, and then the tag `a` can be used in the switch statement to represent the value returned by the function call.

The type of the tag name is automatically inferred. Consider the following match statement:
```
s : int... = (1, 2, 3, 4, 5, 4, 5, 5, 6, 7);
match (s)
{
    on {.:2 -> first, ... -> second, *(4,5):2 -> third, ... -> fourth} -> results:
    {
        /* do something for the tuple value assigned to the tag results */
    }
};
```
where the result of the match pattern expression `{.:2 -> first, ... -> second, *(4,5):2 -> third, ... -> rest}` is assigned to a tag named `results`. In the match pattern, the result of each subpattern is also assigned to a tag. These subpattern tags become the fields of the tuple value of the tag for the enclosing pattern. The type of `results` is then inferred as `tuple (first: (int;int); second: int...; third:(int;int;int;int); rest: int...)`, and for this match, the value is `((1,2),(3),(4,5,4,5), (5,6,7)`.

Tag assignment can also be used in conditional statement for type check. For instance, in the following `if` statement:
```Altro
s : array#(type element_type: numeric) = (2,2); 
if (s[1] -> t is int)
{
    /* do something for the integer value assigned to the tag t */
}
```
the tag assignment is attached with an is-expression to check whether the value of the expression is in the subtype of the given type.

Note that tag assignment follows the [argument passing rule](ArgumentPassing.md). If the instance belongs to a value class, changing the value of the tag will not affect the value of the original source. For instance:
```Altro
s : array#(type element_type: numeric) = (2,2); 
if (s[1] -> t is int)
{
    t = 1;
}
```
Here if we change the value of `t` to 1, the original `s[1]` still hold an integer value 2 in its storage. However, if we pass a reference for instances of a non-value class:
```Altro
s : array#(type element_type: numeric) = (2,2); 
if (s -> t is int[])  // pass reference to 't' if 's' currently refers to an integer array
{
    t[0] = 1;
}
```
assign `t[0]` a new value will actually change the value stored in `s[0]`.

## Selective Assignment  `a <- (expr1, expr2)`

A polymorphic variable can be declared by a type (especially an abstract type) so it can refer to a value in different subtype. For instance, the polymophic variable `y` cab be used to refer to values of any stream types:
```Altro
y : stream;
if (some_condition)
    y = ("abc", "1234", "uvwxyz");  // y referes to a string stream
else
    y = (1,2,3,4);                 // y referes to an integer stream
```
Becuase `y` is polymorphic, the expression of `y[0]` to get the element is also polymorphic. However, you cannot freely assign any type of values to `y[0]` because its type is dependent on th etype of the value that `y` currently holds. You can use tag assignment to check the current type of `y[0]` and then assign the value in the correct type to `y[0]`:
```Altro
if (y -> t is string...)  // pass reference to 't' if 's' currently refers to an integer array
{
    t[0] = "abcd";
}
```
A selective assignment is another way to do this kind of type check and assignment in one step. It assigns the value to a polymorphic dependent argument by selecting a value from a list of expressions that has the suitable type. The value of the first expression is selected when its type is convertible to the type of the value  currently held by the polymorphic argument. Unlike other assignments, a type-case assignment returns a boolean value. It returns true when the polymorphic argument gets assigned. It returns false if none of the expressions matches the type of the  polymorphic argument.
```Altro
s[0] <- ("abcd", 10);
```
If `s` currently refers to a string stream, `s[0]` will take the value "abcd", if `s` currently refers to an integer stream, `s[0]` will take the value 10.

A selective assignment can used for a conditional expression because it returns a boolean value:
```Altro
if (s[0] <- ("abcd", 10))
{
    // s[0] gets a new value
}
else
{
    // s[0] is unchanged
}
```

## Parallel Assignment `(a, b) = expr`

A parallel assignment allows several variables to be assigned in parallel with a tuple value. For example:
```altro
func foo(): (int; string) { (0, "hello") }
a : int;
b : string;
(a, b) = foo();
```
Here the function `foo` return a tuple value in the type of `(int; string)`, and the assignment simultaneously assigns the first element of the tuple value to `a` and the second element of the tuple value to `b`. This feature acts like [unpacking](https://www.w3schools.com/PYTHON/python_tuples_unpack.asp) in some of the other programming languages.

## Parallel Declaration Assignment `a, b := expr`

A parallel declaration assignment allows several new variable names to be declared in parallel with a tuple value. For example:
```altro
func foo(): (int; string) { (0, "hello") }
a, b := foo();
```
Here the function `foo` returns a tuple value in the type of `(int; string)`, and the declaration assignment simultaneously declares two names `a` and `b` being initialized with the first and the second element of the tuple value respectively.

Note that the tuple value is unpacked only when the right side of the parallel declaration assignment contains one expression in a tuple value. If the right side contains a list of expressions, the tuple value will not be unpacked, and the names declared at the left side will be initialized respectively with the expression in the expression list:
```altro
func foo(): (int; string) { (0, "hello") }
a, b := foo(), 3;
```
This is equivalent to:
```altro
func foo(): (int; string) { (0, "hello") }
a := foo();
b := 3;
```
And the following is ill-formed:
```altro
func foo(): (int; string) { (0, "hello") }
a, b, c := foo(), 3;  // Error: Type cannot be inferred for the name: c
```
If an expression in the initialization list has a `void` type, for example, an expression of calling a function without output, the expression will be executed but ignored in parallel declaration assignment. For example,
```altro
func foo() {};
a, b := "hello", foo(), 3;
```
Here `a` gets `"Hello"` and `b`b gets `3`, `foo()` is executed, but ignored.

## Augmented Assignment `a += expr`

Augmented assignment combines assignment with an operator name used as a member function. It is generally used to replace a member function call that applies an input value to its owner's value and then assigned the modified value back to the owner. For instance:
```altro
a := 0;
a = a + 3;
```
Here `+` is a member function of `int`. The member function call `a + 3` adds the input `3` to the owner value contained in `a` and the assignment `a = a + 3` assigns the modified value back to `a`. The augmented assignment combines the two steps together:
```altro
a := 0;
a += 3;
```
The difference is that the owner expression `a` will only be evaluated once. If the owner expression has a side effect that may be evaluated to a different value the second time, then, `owner_expr = owner_expr + input_expr` may have a different result than `owner_expr += input_expr`.

In general, a member function with an operator name that takes one input value and returns the same type value of the owner can have a corresponding augmented assignment defined. Examples are arithmetic operators, bitshift operators, and bitwise operators:

| Operator    | Description                 | Example   |
|:------------|:--------------------------- |:--------- |
| +=          | Add Assign                  | x += y    |
| -=          | Subtacrt Assignn            | x -= y    |
| \*=         | Multiply Assign             | x \*= y   |
| /=          | Divide Assign               | x /= y    |
| ÷=, or /#=  | Integer Divide Assignn      | x ÷= y    |
| %=          | Modulus Assign              | x %= y    |
| ^=          | Power Assign                | x ^= y    |
| \=  or ⊻=   | Bitwise Exclusive Or Assign | x \= y    |
| \|= or ∪=   | Bitwise Inclusive Or Assign | x \|= y   |
| &= or ∩=    | Bitwise And Assig           | x &= y    |
| >>=         | Left Bit Shift Assign       | x >>= n   |
| <<=         | Right Bit Shift Assign      | x <<= n   |

Like an assignment, the owner expression at the left side of an augmented assignment operator must be a non-constant name expression. Unlike an assignment, an augmented assignment cannot be used in the class body for initialization. It can only be used in a code block.
```altro
class test
{
    const a:= 4;
    b:= 4;
    b += 5;      // Error: Name initialization or declartion expected in class body
    ctor()
    {
        a += 4;  // Error: Cannot alter the value of a constant name
        b += 5;  // okay
    }
}
```

## Chained Assignment `a = b = expr`

Chained assignments are used as a shortcut when you want to bind several variables to get a value together.
```altro
a = b = expr;
```
which is the same as this:
```altro
b = expr;
a = b;
```
Note that the preceding statements may not be equivalent to:
```altro
b = expr;
a = expr;
```
if `expr` is a function call that may return a different result each time it is invoked, or the assignment `b assignment_operator expr` returns a different value as in a selective assignment `b <- a` which returns a boolean value.

Chained assignments are supported only when assignments are expressions, and have values. In this case chain assignment can have a right-associative assignment, and assignments happen right-to-left. For example, `b = expr` is an expression that returns a value of the type of `b`; and `a = b = expr` is equivalent to `a` = value returned from the expression `b = expr`.

A selective assignment  `a <- (expr1, expr2)` returns a boolean value, a chained assignment with a selective assignment will have a different result shown as following:
```altro
a := b <- (expr1, expr2);
```
Here `a` is a boolean value which is a different value than the value held by `b`.

A declaratiopn assignment is not an expression and it does not have a return value. Therefore, you cannot chain a declaration assignment:
```altro
a := b:= 4;  // Error:  Name undefined: a -- Invalid declaration in class/object body
```

## See also:
* [Argument Passing](ArgumentPassing.md)
* [Value Class](ValueClass.md)
* [Ownership](Ownership.md)
* [Name Declaration](NameDeclaration.md)
