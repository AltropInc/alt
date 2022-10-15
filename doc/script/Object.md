# Object Class

Objects are fundamental components for Altro applications. An Altro application is an root object. All other objects crated in the application are offsprings of the root object, and this forms an object tree in the application. An object is created as an instance of an object class. An object class is a class derived from the class `Object`, which is a pre-defined class (See [Kinds of Classes](KindsOfClasses.md)).
```altro
class Object
{
    func name(): string;
    func type_name(): string;
    func parent(): Object;
    func child_num(): int;
    func child(name: string): Object;
    func offspring(name: string...): Object;
}
```
* `func name(): string` -- returns the name object object, or an empty string of the object is anonymouus.
* `func type_name(): string` -- returns the name of the type from which this object is created.
* `func parent(): Object` -- returns the parent object or null if this is a root object.
* `func child_num(): int` -- returns number of children.
* `func child(): Object` -- returns the child object in the given `name`, or null if child object with the given name does not exist.
* `func offspring(path: string...): Object` -- searchs down from this object with a path of descendant names and finds the offspring; returns the offspring found or null if the offspring in the search path is not found. 
