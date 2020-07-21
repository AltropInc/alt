#Scope

A decalration introduce a name in a scope. The scope is the region of a program text in which the declaration of the name is valid.
There are three kinds of scope: block, object, and class.

## Block Scope.
A block is a compound statement. A name declared in a block is local to that block and can be used only in this block or blocks
enclosed (nested blocks). It can be used only after the point of the declaration. Names of input arguments for a block are treated
as if they were declared in the outermost block that encloses the block.

##Object Scope.
A name declared in a class body is local to an instance of this class. The name must be used with an instance (the enclosing object)
of this class. It can be used (1) in classes enclosed by this class (nested classes); (2) in classes derived (subclasses) from this class;
and (3) after the “.” operator applied to an instance of this class or of a derived class. When the name is used in cases of (1) and (2),
the enclosing object is self.

## Class Scope. Names declared as class parameters or meta members are local to its class. The name must be used with the (enclosing) class.
It can be used (1) in classes enclosed by this class (nested classes); (2) in classes derived (subclasses) from this class; and (3)
after the “.” operator applied to an this class or a derived class. When the name is used in cases of (1) and (2), the enclosing class
is selfclass.

## Scope Selector

If a name decalred in an outer scope is obscured by the same name in an innder scope, it can be still used when it is qualified by the
name of its enclosing class with a scope select operator ::


