# Altro Nodes

Altro nodes are used to represent a hierarchical [tree structure](https://en.wikipedia.org/wiki/Tree_(data_structure)). Each node in the tree can be connected to multiple child nodes, but must be connected to exactly one parent (owner) node, except for the root node, which has no parent. Each child can be treated like the root node of its own subtree. A node can either be anonymous or named. Named child nodes can be searched through their parents by their names. Altro nodes are basic components used to implement [objects](Object.md), [dictionary](Dictionary.md), Altro syntactic entities such expressions and declarations, and any data structure that requires a hierarchical tree structure.

## The Class Node

The class `node` is the abstraction of Altro nodes.
```altro
class node
{
    func name(): string;
    func typename(): string;
    func isanonymous(): bool;
    func isvalid(): bool;
    func parent(): node;
    func ancestor(type t: node): node;
    func child(name: string): node;
    func member(name: string): node;
    func offspring(names: string...): node;
    func firstchild(): node;
    func firstchild(type t: node): node;
    func next(): node;
    func next(type t: node): node;
    func lastchild(): node;
    func lastchild(type t: node): node;
    func prev(): node;
    func prev(type t: node): node;
    func childnum(): int;
    func childnum(type t: node): int;
    func children(): node...;
    func children(type t: node): node...;
    func children(filter: fn(n:node):bool): node...;
    func callable(f: string): fcall;
    func callable(f: string; input: tuple): fcall;
    func call(f: string): any;
    func call(f: string; input: tuple): any;
    func call(fcall): any;
    func evaluate(n: node): any;
    func getvalue(n: string): any;
    func load(name: string; file: string): node;
    func load(name: string; file: string; input: tuple): node;
    func preload(name: string; file: string): bool;
    func preload(name: string; file: string; input: tuple): bool;
    func unload(name: string): bool;
    func unload(chd: node): bool;
    func unloadall(): bool;

}
```
* **func name(): string;** It returns the name of the node or a null string if the node is anonymous.

* **func typename(): string;** It returns the name of the node type.

* **func isanonymous(): bool;** It returns true if it is an anonymous node.

* **func isvalid(): bool;** It returns true if the node is valid.

* **func parent(): node;** It returns the parent node.

* **func ancestor(type t: node): node;** It returns the ancestor node in the given type `t`. Example: <br><pre>
object popup: Window
{
&nbsp;   object toplevel: Frame
&nbsp;   {
&nbsp;       object left_panel: Frame
&nbsp;       {
&nbsp;           ctor()
&nbsp;           { 
&nbsp;               outer_window := ancestor(Window); // get ancestor that is an object of Window, which is 'popup'
&nbsp;           }
&nbsp;       }
&nbsp;   }
}</pre>
* **func child(name: string): node;** It returns the child node in the given name.<br><pre>
object popup: Window
{
&nbsp;   object toplevel: Frame { }
&nbsp;   ctor()
&nbsp;   {
&nbsp;       toplevel_frame := child("toplevel");  // toplevel_frame refers to popup's child named "toplevel"
&nbsp;   }
}</pre>
* **func member(name: string): node;** It returns the member node in the given name. Note that members are not limited to child nodes, but can also be declared names, including those inherited from the base class. For example:<br><pre>
class base  { x:= 1; }
class derived: base { y:= 2; }
object derived_obj: derived {}
x_member := derived_obj.member("x"); 
// x_member refers to the expression node for the declared name "x"</pre>
* **func offspring(names: string...): node;** It returns the offspring node in the path of the given name list.<br><pre>
object popup: Window
{
&nbsp;   object toplevel: Frame
&nbsp;   {
&nbsp;       object left_panel: Frame { }
&nbsp;   }
}
panel := popup.offspring("toplevel", "left_panel");
// panel refers to the offspring left_panel</pre>
* **func firstchild(): node;** It returns the reference to the first child node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
}
first_frame := popup.firstchild();
// first_frame refers  to topframe</pre> 
* **func firstchild(type t: node): node;** It returns the reference to the first child node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
first_button := popup.firstchild(Button);
// first_button refers  to leftbutton</pre> 
* **func next(): node;**  It returns the reference to the next node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
next_frame := popup.topframe.next();
// next_frame refers  to bottomframe</pre> 
* **func next(type t: node): node;** It returns the reference to the next node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
next_button := popup.topframe.next(Button);
// next_button refers to leftbutton</pre>
* **func lastchild(): node;**  It returns the reference to the last child node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
}
last_frame := popup.lastchild();
// last_frame refers to bottomframe</pre>
* **func lastchild(type t: node): node;** It returns the reference to the last child node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
last_frame := popup.lastchild(Frame);
// last_frame refers to bottomframe</pre>
* **func prev(): node;**  It returns the reference to the previous node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
prev_node := popup.rightbutton.prev();
// prev_node refers to leftbutton</pre>
* **func prev(type t: node): node;**  It returns the reference to the previous node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
prev_frame := popup.rightbutton.prev(Frame);
// prev_frame refers to bottomframe</pre>
* **func childnum(): int;**  It returns number of child nodes.<br><pre>
class base  { object x {} }
class derived: base { object y {} }
object derived_obj: derived {}
n := derived_obj.childnum(); // n gets 2</pre>
* **func childnum(type t: node): int;** It returns number of child nodes in the given type.<br><pre>
class base  { object x {} }
class derived: base { object y: Drawable{} }
object derived_obj: derived {}
n := derived_obj.childnum(Drawable); // n gets 1</pre>
* **func children(): node...;** It returns a list of all child nodes.<br><pre>
class base  { object x {} }
class derived: base { object y {} }
object derived_obj: derived { object z: Drawable{} }
n := derived_obj.children(); // n gets a node list (z, x, y)</pre>
* **func children(type t: node): node...;**  It returns a list of all child nodes in the given type.<br><pre>
class base  { object x {} }
class derived: base { object y: Drawable{} }
object derived_obj: derived { object z: Drawable{} }
n := derived_obj.children(Drawable); // n gets a node list (z,y)</pre>
* **func children(filter: fn(n:node):bool): node...;**  It returns a list of all child nodes that satisfy the condition given by the functor filter.<br><pre>
class base  { object ch1 {} }
class derived: base { object ch2 {} }
object derived_obj: derived { object z {} }
n := derived_obj.children({ n.name().starts_with("ch") }); // n gets a node list (ch1, ch2)</pre>
* **func callable(f: string): fcall;** It returns a fcall expression if the member of the given name is callable. The fcall expression is a tuple pair of a routine and an input expression:<br><pre>
type fcall : tuple (rt: type; input: expr);</pre>
The fcall expression can be executed by calling the routine `rt` with the expression `input`. here is an example to use `callable`:<br><pre>
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf(): int { i }
};
print(ob1.callable("mf"));  // prints a fcall expression: (mf(): int, null)</pre>
* **func callable(f: string; input: tuple): fcall;** It returns a fcall expression if the member of the given name is callable with the given input. If the callable has no matched interface for the given input, an empty fcall expression will be returned.<br><pre>
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf(x:int): int { x + i }
};
print(ob1.callable("mf", 10)); // prints a fcall expression: (mf(x: int): int, (10))</pre>
* **func call(f: string): any;** It finds a callable in the given name, calls it, and returns the value that the callable returns after called.<br><pre>
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf(): int { i }
};
print(ob1.call("mf"));  // prints an integer: 5</pre>
* **func call(f: string; input: tuple): any;** It finds a callable in the given name and with an interface that matches the given input, calls it with the input, and returns the value that the callable returns after called..<br><pre>
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf(x:int): int { x + i }
};
print(ob1.call("mf", 10));  // prints an integer: 15</pre>
If the callable in the given name is not found or does not have an interface that matches the given input, this function returns a value of type `none`. If the callable has no output when called, this function returns void:<br><pre>
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf() {}
&nbsp;   func mf(x:int): int { x + i }
};
print(ob1.call("mf");                  // prints an empty string for void
print(ob1.call("mf", "test string"));  // prints a string "none" for matched callable unfounded</pre>
* **func call(fcall): any;** It calls the callable with the input stored in the fcall tuple.<br><pre>
object ob1 { func mf(x:int): int { x*x } };
fc := ob1.callable("mf", 10);
if (not fc.empty())
{
&nbsp;   println (ob1.call(fc));   // this prints an integer 100
}</pre>
* **func evaluate(n: node): any; ** It returns the value of a member node given by `n`.<br><pre>
class member_class { y: int = 2; }
object member1: member_class { }
y_member := member1.member("y");
if (y_member!=null)
{
&nbsp;   y_val := member1.evaluate(x_member); // y_val gets integer 2
}</pre>
* **func getvalue(n: string): any;** It returns the value of a member declaration in the given name.<br><pre>
class member_class { y: int = 2; }
object member1: member_class { }
y_val := member1.getvalue("y");    // y_val gets integer 2</pre>
If the member of the given  does not exist, a value of `none` is returned. This function is a combination of `member` and `evaluate`.
* **func load(name: string; file: string): node;** It creates a child in the given mame from an Altro script file.<br><pre>
using sys;
object test: Object
{
&nbsp;   loaded := load("mychild", "MyClass.alt");  // When succeeded, the load makes the 'loaded' to refer to an child object of MyClass
}</pre>
Note that this function can only succeed if `MyClass` has a [default constructor](Constructor.md). If `MyClass` has a constructor that requires input, an input expression mu be provided using the following function.
* **func load(name: string; file: string; input: tuple): node;** It creates a child in the given mame from an Altro script file with an input expression required by the constructor of the class specified in the file. For instance, if we class file `Person.alt` with the following contents:<br><pre>
class Person
{
&nbsp;   name: string;
&nbsp;   age:  int;
&nbsp;   ctor (n: string; a: int)
&nbsp;   {
&nbsp;       name = n;
&nbsp;       age = a;
&nbsp;   }
}</pre>
We can use this function to create a child with an string input:<br><pre>
using sys;
object test
{
&nbsp;   loaded := load("person", "Person.alt", ("John", 17)); 
&nbsp;   // When succeeded, the load makes the 'loaded' to refer to an child object of Person with name "John" and age 17
}</pre>
* **func preload(name: string; file: string): bool;** It creates a child in the given name from an Altro script file. The loading process is in a separate thread. After the loading process is finished, the loading process will notify the caller thread, the thread that calls this function, typically, the main thread. The caller thread will transfer the loaded object into the child list of the given parent and calls the parent `onload`.<br><pre>
using sys;
object test
{
&nbsp;   func onload(loaded: node)
&nbsp;   {
&nbsp;       println("a child with name ", loaded, " is loaded!");
&nbsp;   }
&nbsp;   ctor()
&nbsp;   {
&nbsp;       preload("mychild", "TestClass.alt");
&nbsp;   }
}</pre>
* **func preload(name: string; file: string; input: tuple): bool;** It creates a child in the given name from an Altro script file with an input expression required by the constructor of the class specified in the file and the loading process is in a separate thread.<br><pre>
using sys;
object test
{
&nbsp;   func onload(loaded: node)
&nbsp;   {
&nbsp;       println("a person ", loaded.getvalue("name"), " with age=", loaded.getvalue("age"), " is loaded!");
&nbsp;   }
&nbsp;   ctor()
&nbsp;   {
&nbsp;       preload("person", "Person.alt", ("John", 17));
&nbsp;   }
}</pre>
* **func unload(name: string): bool;**  It removes a child in the given name. The child to be removed must be created by calling the load or preload function.<br><pre>
using sys;
object test
{
&nbsp;   func loadnext(person_file: string; name: string; age: int)
&nbsp;   {
&nbsp;       unload("person");
&nbsp;       loaded = load("person", person_file, (name, age));
&nbsp;   }
}</pre>
* **func unload(chd: node): bool;** It removes a child given by `chd`. The child to be removed must be created by calling the load or preload function.<br><pre>
using sys;
object test
{
&nbsp;   loaded: node;
&nbsp;   func loadnext(person_file: string; name: string; age: int)
&nbsp;   {
&nbsp;       if (loaded!=null) unload(loaded);
&nbsp;       loaded = load(name, person_file, (name, age));
&nbsp;   }
}</pre>
* **func unloadall(): bool;** It removes all children created by calling the load or preload function.<br><pre>
using sys;
object test
{
&nbsp;   func loadnext(person_file: string; name: string; age: int)
&nbsp;   {
&nbsp;       unloadall();
&nbsp;       loaded = load(name, person_file, (name, age));
&nbsp;   }
}</pre>
