# Referential

The Referential represents references used to access objects or values. The Referential is parameterized by the type of the object or value referred by
the reference (ReferredType).

| name         | scope  | meaning                                                                |
|:------------ |:------ |:---------------------------------------------------------------------- |
| ReferredType | class  | The type of the object or value referred by the reference              |
| get          | object | function to get the referred. This is also called by operator . and *  |

## Built-in Referential ref

The class "ref" is a built-in referential class which defines how objects and values are accessed
by all names declared in the code. Primitive values such as integers and characters are stored in
the reference (name) directly. Composite values and objects are stored in a shared pool and the
reference does not provide the storage but acts as a vitual link points to the location in the
shared pool. One, and only one references is the owner of the value or object, and the owner will
remove the value or object from the pool when it goes away.
