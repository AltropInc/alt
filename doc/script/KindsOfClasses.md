# Kinds of Classes

Everything in Altro is associated with [classes](Class.md) and and their instances. A class acts like a constructor or blueprint for creating their instances.

Altro has two kinds of class instances: data and objects. Data are pieces of information represented in a form that can be stored, passed, and processed. Objects contain data and reside as a node in an object tree of an Altro application. Both data and objects can be structured and can have a set of behaviors that can be applied. However, objects are building blocks of an Altro application which is a root object. All other objects are created as offsprings of the application root object. An object has references to its parent and children. Data as pieces of information stored in memory do not reside in the application object tree. Unlink objects, though data can possibly be structured as in composite data, component data do not necessarily have references to its enclosing data, and the enclosing data do not necessarily have explicit reference to its component data.

Therefore， Altro has two kinds of classes: **data class** and **object class**. Obviously, instances of a data class are data, and instances of an object class are objects. The class `any` is the root class from which all other classes are derived. All object classes are derived from the class `Object`. All other classes derived from the class `any` are data classes. This is illustrated as below:
```
                        any (the root class)
                  ┌──────────────┴────────────┐
           (data classes)               Object (object classes)
```
