# Singleton

If a class can have only one instance, the class is a singleton class, and the unique instance is called singleton. The instance is defined together with its class. The declaration of a singleton is similar to the declaration of a [class declaration](Class.md] with the following differences:
* A singleton declaration starts with the keyword `object` instead of the keyword `class`
* The constructor of a singleton should have no input because a singleton is created in-place and its constructor cannot be called.
* A singleton declaration cannot introduce its own class parameters.
* A singleton cannot introduce more virtual function interfaces.
* A singleton cannot introduce have any deferred function interfaces.
* A singleton must implements all deferred function interfaces in base classes and interface classes.
* The class scope and the object scope in a singleton is the same scope. Therefore, there is no distinction between meta members and object members.

