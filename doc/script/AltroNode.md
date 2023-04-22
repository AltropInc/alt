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
    func children(): int;
    func children(type t: node): int;
    func children(filter: fn(n:node):bool): int;
    func callable(f: string): fcall;
    func callable(f: string; input: tuple): fcall;
    func call(f: string): any;
    func call(f: string; input: tuple): any;
    func call(fcall): any;
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
}
</pre>
* **func child(name: string): node;** It returns the child node in the given name.<br><pre>
object popup: Window
{
&nbsp;   object toplevel: Frame { }
&nbsp;   ctor()
&nbsp;   {
&nbsp;       toplevel_frame := child("toplevel");  // toplevel_frame refers to popup's child named "toplevel"
&nbsp;   }
}
</pre>
* **func member(name: string): node;** It returns the member node in the given name.<br><pre>
object popup: Window
{
&nbsp;   object toplevel: Frame { }
&nbsp;   ctor()
&nbsp;   {
&nbsp;       toplevel_frame := child("toplevel");  // toplevel_frame refers to popup's child named "toplevel"
&nbsp;   }
}
</pre>
