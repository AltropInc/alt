# Altro Nodes

Altro nodes are used to represent a hierarchical [tree structure](https://en.wikipedia.org/wiki/Tree_(data_structure)). Each node in the tree can be connected to multiple child nodes, but must be connected to exactly one parent (owner) node, except for the root node, which has no parent. Each child can be treated like the root node of its own subtree. A node can either be anonymous or named. Named child nodes can be serached through its parent by their names. Altro nodes are basic components used to implement [objects](Object.md), [dictionary](Dictionary.md), Altro syntactic entities such expresions and declarations, and any data structure that requires a hierarchical tree structure.

## The Class Node

The class `node` is the abstraction of altro nodes.
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
// first_frame referes to topframe</pre> 
* **func firstchild(type t: node): node;** It returns the reference to the first child node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
first_button := popup.firstchild(Button);
// first_button referes to leftbutton</pre> 
* **func next(): node;**  It returns the reference to the next node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
next_frame := popup.topframe.next();
// next_frame referes to bottomframe</pre> 
* **func next(type t: node): node;** It returns the reference to the next node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
next_button := popup.topframe.next(Button);
// next_button referes to leftbutton</pre>
* **func lastchild(): node;**  It returns the reference to the last child node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
}
last_frame := popup.lastchild();
// last_frame referes to bottomframe</pre>
* **func lastchild(type t: node): node;** It returns the reference to the last child node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
last_frame := popup.lastchild(Frame);
// last_frame referes to bottomframe</pre>
* **func prev(): node;**  It returns the reference to the previous node.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
prev_node := popup.rightbutton.prev();
// prev_node referes to leftbutton</pre>
* **func prev(type t: node): node;**  It returns the reference to the previous node in the given type.<br><pre>
object popup: Window
{
&nbsp;   object topframe: Frame {}
&nbsp;   object bottomframe: Frame {}
&nbsp;   object leftbutton: Button {}
&nbsp;   object rightbutton: Button {}
}
prev_frame := popup.rightbutton.prev(Frame);
// prev_frame referes to bottomframe</pre>
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
The fcall expression can be executed by calling the routine 'rt' with the expression 'input'. here is an example to use `callable`:<br><pre>
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
If the callable in the given name is not found or does not have an interface that matches the given input, this function returns a value of type `none`. If the callable has no output when called, this function returns void.
object ob1
{
&nbsp;   i:=5;
&nbsp;   func mf() {}
&nbsp;   func mf(x:int): int { x + i }
};
print(ob1.call("mf");                  // prints an empty string for void</pre>
print(ob1.call("mf", "test string"));  // prints a string "none" for matched callable unfounded</pre>
* **func call(fcall): any;** It calls the callable with the input stored in the fcall tuple.<br><pre>
object ob1 { func mf(x:int): int { x*x } };
fc := ob1.callable("mf", 10);
if (not fc.empty())
{
    println (ob1.call(fc));   // this prints an integer 100
}</pre>
* **func getvalue(n: string): any;**  .<br><pre>
</pre>
* **func load(name: string; file: string): node;**  .<br><pre>
</pre>
* **func load(name: string; file: string; input: tuple): node;**  .<br><pre>
</pre>
* **func preload(name: string; file: string): bool;**  .<br><pre>
</pre>
* **func preload(name: string; file: string; input: tuple): bool;**  .<br><pre>
</pre>
* **func unload(name: string): bool;**  .<br><pre>
</pre>
* **func unload(chd: node): bool;**  .<br><pre>
</pre>
* **func unloadall(): bool;**  .<br><pre>
</pre>
