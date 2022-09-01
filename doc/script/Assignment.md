# Assignment

An assignment sets or re-sets the value stored or referred in the storage location denoted by a variable name, or in other words, it copies a value or reference into the storage location of the variable.

When you assign a variable, if it is the value copied into the variable, this behavior is called **copy by value**. If the value is an instance of a [value class](ValueClass.md) (examples are integers, characters, and enumeration values), the default assignment behavior is copy by value. For example, let `a` and `b` be two variables to store integer values and each has an initial value, 1 and 2 respectively:
```altro
a := 1; 
b := 2;
```
After assignement `a = b`, the value stored in `b`, the integer `2`, is copied into `a`. As result, the value stored in `a` is now `2`. Although both `a` and `b` holds value `2`, they are indepedent values, that is, assigning `a` to another different value will have no effect on the value stored in `b`.

When you assign a variable, if it is the reference to a value not the value itself, copied into the variable, this behavior is called **copy by reference**. If the value is an instance of a non value class (examples are arrays, streams, strings, tuples, and objects), the default assignment behavior is copy by reference. For example, let `p` and `q` be two variables to store references to integer arrays. `a` has a null reference and `b` has a reference to an array value `(1,2,3,4)`:
```altro
a : int[4]; 
b : int[4] = (1,2,3,4);
```
After assignement `a = b`, the reference stored in `b` is copied into `a`. As result, the referece stored in `a` refers to the same array value referred by `b`:
```altro
a ──────┐
        ├───🡪 (1,2,3,4)
b ──────┘
```
When we change array value through `a`:
```altro
a[0] = 5;
```
the value refered by `b` is also changed because it is the same value:
```altro
a ──────┐
        ├───🡪 (5,2,3,4)
b ──────┘
```
When you assign `a` to another array reference, say, `a = (6,7,8,9)`, the reference stored in `b` is unchanged:
```altro
a ──────────🡪 (6,7,8,9)
b ──────────🡪 (5,2,3,4)
```
However, when you assign `b` to another array reference, say, `b = (6,7,8,9)`, because `b` is the owner of the value `(5,2,3,4)`, the value of `(5,2,3,4)` is destroyed before `b` takes the new reference. If `a` referes to the same value `(5,2,3,4)` before, the reference in `a` will be nullified, and `a` won't refer to the new value 
`6,7,8,9)`:
```altro
Before the assinment:
      a ──────┐
              ├───🡪 (5,2,3,4)
      b ──────┘
After b = (6,7,8,9):
      a ──────────🡪 null
      b ──────────🡪 (6,7,8,9)
```

* Common Assignment (notation: `x = expr`)
