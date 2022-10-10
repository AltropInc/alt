# Class Inheritance Tree

The class inheritance hierarchy in Altro is demonstrated through the following diagram.
```
any (the root class)
    Object
       (user-defined object classes)
    functional
       func
       (user-defined functional classes)
    enum
       bool
       (user-defined concrete enum classes)
    enumset
       (user-defined concrete enumset classes)
    numeric
       integral
           signed
               tiny
               short
               int
               long
               llong
           usigned
               utiny
               ushort
               uint
               ulong
               ullong
       real
           float
           double
           ldouble
       (user-defined numeric classes)
   char
   container
       tuple
           (user-defined tuple classes)
       value
           (user-defined value classes)
       array
           (user-defined array classes)
       stream
           (user-defined stream classes)
       string
   iterator
       (user-defined iterator classes)
   ref
       (user-defined reference classes)
```
