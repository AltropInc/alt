# Dictionary

Dictionaries are used to store data values in key:value pairs. Values contained in a dictionary are accessed via their associated names. Dictionaries are Altro’s implementation of a data structure that is more generally known as a named tree node. A dictionary consists of a collection of subtree node, each key-value pair maps the node nama to its associated value. Therefore, a dictionary can be nested, that is, a dictionary can contain another dictionary. The dictionay class is defined as a subclass of `namednode`:
```altro
class dic is namednode
{
    ctor();            // constructor to create an empty dictionary
    ctor(string);      // constructor to create a dictionary from a json file
    ctor(tuple);       // constructor to create a dictionary from a tuple value
    func put(name:string; val: any);   // insert a value
    func get(name:string): any;        // get a value by the given name
    func get(name:string...): any;     // get a nested value by a set of names
    func geti(name:string; defa: int): int;           // get an integer value by the given name
    func geti(names:string...; defa: int): int;       // get a nested integer value by a set of names
    func getis(name:string): int...;                  // get an integer stream value by the given name
    func getis(names:string...): int...;              // get a nested integer stream value by a set of names
    func getn(name:string; defa: double): double;     // get a number value by the given name
    func getn(names:string...; defa: double): double; // get a nested number value by a set of names
    func getns(name:string): double...;               // get an number stream value by the given name
    func getns(names:string...): double...;           // get a nested number stream value by a set of names
    func gets(name:string): string;                   // get a string value by the given name
    func gets(name:string...): string;                // get a nested string value by a set of names
    func getss(name:string): string...;               // get an string stream value by the given name
    func getss(names:string...): string...;           // get a nested string stream value by a set of names
}
```

## Create a Dictionary with Constructor

A dictionary can be created from one of the three constructors. The default constructor `ctor()` to is used to create an empty dictionay, and then you can call the put function add items into the dictionary: 
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
