# Assignment

An assignment sets or resets the value stored or referred in the storage location denoted by a variable name, or in other words, it copies a value or reference into the storage location of the variable.

There are five kinds assignments:

|                        | notation    | Description                                                                        |
|:---------------------- |:----------- |:---------------------------------------------------------------------------------- |
| assignment             | a = expr    | default assignment using copy by value for value classes or by reference otherwise |
| copy assignment        | a @= expr   | copy assignment using copy by value only and the copy is a shallow copy            |
| deep copy assignment   | a @@= expr  | copy assignment using copy by value only and the copy is a deep copy               |
| declaration assignment | a := expr   | a new name declaration being assigned with an initial value or reference           |
| reference assignment   | a <- expr   | a new reference declaration being assigned with an initial reference to a value    |

## Assignment `a = expr` (Default assignment)

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
## Reference Assignment `a <- expr`

A reference assignment `a <- expr` is to copy the reference of a value evaluated by `expr` to a reference variable `a`. The expression `expr` must be a name expression (also called as [l-value expression](https://en.wikipedia.org/wiki/Value_(computer_science)#lrvalue) in some other programing languages) which evaluates to a value stored in a location that persists beyond this expression. The location of the value is copied as a reference to the reference variable. Example:
```altro
s : int... = (1,2,3,4);      // 's' refers to a integer stream (1,2,3,4)
r <- s[1];                   // 'r' refers to the second element value in 's'
r = 10;                      // copy assign the value 10 into the location referred by 'r'
i := s[1];                   // 'i' get the value of the second element in 's', which is changed to 10
```
Note that a reference assignment is a special declaration assignment which introduce a new reference variable name.
