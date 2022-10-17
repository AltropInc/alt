# Singleton

If a class can have only one instance, the class is a singleton class, and the unique instance is called singleton. The instance is defined together with its class. The declaration of a singleton is similar to the declaration of a [class declaration](Class.md] with the following differences:
* A singleton declaration starts with the keyword `object` instead of the keyword `class`.
* A singleton can only inherit from an object class (not from a data class). If the inheritance clause is missing, the default class is [`Object`](object.md).
* The constructor of a singleton should have no input because a singleton is created in-place and its constructor cannot be called.
* A singleton declaration cannot introduce its own class parameters.
* A singleton cannot introduce more virtual function interfaces.
* A singleton cannot introduce any deferred function interfaces.
* A singleton must implement all deferred function interfaces in base classes and interface classes.
* The class scope and the object scope in a singleton is the same scope. Therefore, meta members are not allowed in a singleton. All members declared in a singleton belongs to the singleton object scope.

Singletons are commonly used for unique member objects contained in parent objects, and these member objects are permanently attached to its parent. Let's take an example of a pseudo code for table that contains a top surface and four legs:
```altro
class Table
{
    length := 64.0;
    width := 22.0;
    height := 30.0;
    leg_thickness := 4;
    top_thickness := 1;
    leg_height := height-top_thickness;
    object TableTop is RectangularCuboid   // TableTop is a singleton, a member object of the table
    {
        material = wood;
        size = (length, height, top_thickness);
        position = (0, 0, height-top_thickness/2);
    }
    class Leg is RectangularCuboid   // Leg is a member class used as the base for its four legs
    {
        material = wood;
        size = (4, 4, height-top_thickness/2);
    }
    object FrontLeftLeg is Leg   // A singleton for the front left leg of the table
    {
        position = ((leg_thickness-length)/2, (width-leg_thickness)/2, leg_height/2);
    }
    object FrontRightLeg is Leg   // A singleton for the front right leg of the table
    {
        position = ((length-leg_thickness)/2, (width-leg_thickness)/2, leg_height/2);
    }
    object BackLeftLeg is Leg   // A singleton for the back left leg of the table
    {
        position = ((leg_thickness-length)/2, (leg_thickness-width)/2, leg_height/2);
    }
    object BackRightLeg is Leg   // A singleton for the back right leg of the table
    {
        position = ((length-leg_thickness)/2, (leg_thickness-width)/2, leg_height/2);
    }   
}
```
When we create an object of `Table`, the table top and four legs will automatically be created as child objects of the table. When we remove the table, its tabletop and four legs are also removed.

A singleton can also be written in a separate file, and then be loaded dynamically into an object. For example, the contents to be loaded in a viewer can be contained in a singleton in a separate file. These dynamically loaded objects are not permanently attached to its parent - the viewer can remove its current contents before loading new contents contained in another singleton.

Because a singleton unifies the class scope and the object scope, meta members inside a singleton can access names that are not declared as meta. Let's assume `Table` itself is a singleton:
```altro
object Table   // Table is a singleton here, which inherits the default base 'Object'
{
    length := 64.0;
    leg_thickness := 4;
    class Leg is RectangularCuboid   // Leg is a member class used as the base for its four legs
    {
        material = wood;
        size = (4, 4, height-top_thickness/2);
        meta func leg_offset_x (): double
        {
            return (length-leg_thickness)/2;
        }
    }
}
```
Here the meta function `leg_offset` gets the leg distance in x-dimension from the center and it accesses the names `length` and `leg_thickness` defined in the singleton `Table`. This is not allowed if `Table` is not a singleton, because different instances of a table class may have different `length` and `leg_thickness` values and the function `leg_offset_x` cannot be defined as meta.

