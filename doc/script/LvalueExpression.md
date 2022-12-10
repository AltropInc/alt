# Lvalue Expression

Expressions that refer to values stored in memory locations that can be accessed through another expression later are called **lvalue** expressions. An lvalue implies a value at `left`, indicating that it can appear on the left side of an assignment operator, and can be used by other [lvalue operators](Operators.md).

Expressions referring to modifiable locations are called "modifiable lvalue". A modifiable lvalue cannot have a type with the const attribute. A variable name is a modifiable lvalue if it is not const. Any of the following expressions can be lvalue expressions:

* A variable name
* A subscript expression with a variable name
* A member-selection expression with a variable name
* A function output that is a reference
* An lvalue in parentheses

The term `rvalue` is used for the value of an expression and to distinguish it from an lvalue, which implies a value that can appear on the right side of an assignment operator. Rvalue may not have a memory location, examples are [literals](Literals.md) and a non-reference value returning from a function.  All lvalues are rvalues but not all rvalues are lvalues.
