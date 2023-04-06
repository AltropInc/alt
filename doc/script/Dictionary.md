# Dictionary

Dictionaries are used to store data values in key:value pairs. Values contained in a dictionary are accessed via their associated names. Dictionaries are Altro’s implementation of a data structure that is more generally known as a named tree node. A dictionary consists of a collection of subtree nodes, each key-value pair maps the node nama to its associated value. Therefore, a dictionary can be nested, that is, a dictionary can contain another dictionary. The dictionary class is defined as a subclass of `namednode`:
```altro
class dic is namednode
{
    ctor();            // constructor to create an empty dictionary
    ctor(string);      // constructor to create a dictionary from a json file
    ctor(tuple);       // constructor to create a dictionary from a tuple value
    func put(name:string; val: any);    // insert a value
    func get(name:string): any;         // get a value by the given name
    func get(names:string...): any;     // get a nested value by a set of names
    func geti(name:string; defa: int=0): int;           // get an integer value by the given name
    func geti(names:string...; defa: int=0): int;       // get a nested integer value by a set of names
    func getis(name:string): int...;                    // get an integer stream value by the given name
    func getis(names:string...): int...;                // get a nested integer stream value by a set of names
    func getn(name:string; defa: double=0): double;     // get a number value by the given name
    func getn(names:string...; defa: double=0): double; // get a nested number value by a set of names
    func getns(name:string): double...;                 // get an number stream value by the given name
    func getns(names:string...): double...;             // get a nested number stream value by a set of names
    func gets(name:string): string;                     // get a string value by the given name
    func gets(names:string...): string;                 // get a nested string value by a set of names
    func getss(name:string): string...;                 // get an string stream value by the given name
    func getss(names:string...): string...;             // get a nested string stream value by a set of names
}
```

## Create a Dictionary with Constructor

A dictionary can be created from one of the three constructors. The default constructor `ctor()` to is used to create an empty dictionary, and then you can call the put function add items into the dictionary: 
```
d := dic();
d.put("firstName", "John");
d.put("lastName", "Smith");
d.put("age", 27);
```
Or, you can use the constructor `ctor(string)` to load a [JSON](https://en.wikipedia.org/wiki/JSON) file, where the input string the path to the JSON file:
```
d := dic("person.json");
```
you can use the constructor `ctor(tuple)` to convert a tuple value into the contents in the dictionary. The conversion will assume a value element to follow an string element as the value name:
```
d := dic("firstName", "John", "lastName", "Smith", "age", 27);
```
or,
```
d := dic(("firstName", "John"), ("lastName", "Smith"), ("age", 27));
```

## Create a Dictionary with Embedded JSON Format

A dictionary can be created by placing an embedded [JSON](https://en.wikipedia.org/wiki/JSON) format, which is a sequence of elements within curly {} braces and separated by ‘comma’. For example:
```altro
d: dic = {
  "firstName": "John",
  "lastName": "Smith",
  "age": 27,
   "isAlive": true,
  "address": {
    "streetAddress": "21 2nd Street",
    "city": "New York",
    "state": "NY",
    "postalCode": "10021-3100"
  },
  "phoneNumbers": [
    {
      "type": "home",
      "number": "212 555-1234"
    },
    {
      "type": "office",
      "number": "646 555-4567"
    }
  ]
};
```
If the type of `dic` must be inferred, the keyword `json` need to be prefixed to indicate that the embedded string within curly {} braces is in JSON format:
```altro
d := json { "firstName": "John", ... };
```
In the above declaration of `d`, the keyword `json` is required because the string within curly {} braces is considered a block for an [lambda function or anonymous function](FreeFunctor.md).

Note that a dictionary can be created from a JSON format, but a dictionary is a more flexible format and its contents cannot always be converted to a JSON format.

## Dictionary Methods

In addition to methods defined in the class `namednode, the class `dic` has the following methods:

* **func put(name:string; val: any);** It inserts a named value into the dictionary.<br><pre>
d := dic();
d.put("name", "John");
d.put("age", 27);
d.put("address", dic(("street", "21 2nd Street"), ("city", "New York"), ("zip", 10021)));
</pre>

* **func get(name:string): any;** It returns the value of associated to the given name.<br><pre>
print(d.get("name"));     // this prints John
print(d.get("age"));      // this prints 27
</pre>

* **func get(names:string...): any;** It returns a nested value by a list of names from the root to the node of the value; For example,<br><pre>
print(d.get("address", "city");   // this prints New York
</pre>

* **func geti(name:string; defa: int=0): int;** It returns the integer value associated with the given name. If there is no integer value associated to the given name, the default value `defa` will be returned.<br><pre>
d.geti("age");      // this returns 27
d.geti("zip", -1);  // this returns -1
</pre>

* **func geti(names:string...; defa: int=0): int;** It returns the nested integer value by a list of names from the root to the node of the value. If there is no integer value associated to the path, the default value `defa` will be returned.<br><pre>
d.geti("address", "zip");  // this returns 10021
</pre>

* **func getis(name:string): int...;** It returns the integer stream value associated with the given name. If there is no such value associated with the name, a null stream value is returned.

* **func getis(names:string...): int...;** It returns the integer stream value associated with the given name. If there is no such value associated with the name, a null stream value is returned.

* **func getn(name:string; defa: double=0): double;** It returns the number value associated with the given name. If there is no number value associated with the given name, the default value `defa` will be returned.

* **func getn(names:string...; defa: double=0): double;** It returns the nested number value by a list of names from the root to the node of the value. If there is no number value associated with the path, the default value `defa` will be returned.

* **func getns(name:string): double...;** It returns the number stream value associated with the given name. If there is no such value associated with the name, a null stream value is returned

* **func getns(names:string...): double...;** It returns the number stream value by a list of names from the root to the node of the value. If there is no such value associated with the name, a null stream value is returned.

* **func gets(name:string): string;** It returns the string value associated with the given name. If there is no number value associated with the given name, the default value `defa` will be returned.

* **func gets(names:string...): string;** It returns the nested string value by a list of names from the root to the node of the value. If there is no string value associated with the path, the default value `defa` will be returned.

* **func getss(name:string): string...;** It returns the string stream value associated with the given name. If there is no such value associated with the name, a null stream value is returned.

* **func getss(names:string...): string...;** It returns the string stream value by a list of names from the root to the node of the value. If there is no such value associated with the name, a null stream value is returned.

## Iteration through a Dictionary

Dictionaries are iterable. You can use `foreach` to iterate all elements within a dictionary value:
```altro
d: dic = {
  "firstName": "John",
  "lastName": "Smith",
  "age": 27,
  "address": {
    "streetAddress": "21 2nd Street",
    "city": "New York",
    "state": "NY",
    "postalCode": 10021
  }
};
foreach (n in d) println(n);
```
The above `foreach` iteration will prints
```
name: "John"
lastName: "Smith"
age: 27
address: {
    "street": "21 2nd Street",
    "city": "New York",
    "postalCode": 10021}
```
Because the dictionary class is a subclass of `namednode` which is in a tree data structure, you can also use the methods provided in the class `namednode` to iterate through the child nodes:
```
for (n := d.firstchild(); n.isvalid(); n=n.next())
{
    println(n);
}
```
This iterations gives the same output. The iterator used in this iteration is a named tree node. If you want to print just the name of the node, you can do:
```
for (n := d.firstchild(); n.isvalid(); n=n.next())
{
    println(n.name());
}
```


