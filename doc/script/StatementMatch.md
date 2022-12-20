# Match Statement

A `match` statement provides pattern matching via match cases, which are simular [switch cases](StatementSwitch.md). Match cases are more powerful and allows for more complicated pattern matching. 

The match statement syntax:
```altscript
match (control_expression => tag)  // => tag is used to hold the result of the control_expression and is optional
{
    on value:    // code to execute when the control_expression evaluates to the given value;
    on pattern:  // code to execute when the control_expression evaluates to a value match the given pattern;
    default:     // code to execute if none of the listed patterns are matched;
}
```
Unlike switch, the on value can be variable, and it can be any value type as well as array, stream, tuple, and string.

Pattern match tests are performed top to bottom, if a pattern is matched, the first corresponding block will be executed and the
control flow is done if there is no continue to fallthrough. In the fallthrough, the pattern followed will be tested and the block
will be executed only when the test is sucessful.

Three kinds of matches are provided: exact value match, regular expression match, and pattern match.

### Exact Value Match

Exact value match can be used for any type. It simply checks if the control_expression given in the match statement evaluates to
an value equal to the value of the expression given in the on clause.

```altscript
i : int;
j : int;
...
match (i)
{
    on 1: // code to execute when the value of i is equal to 1
    on j: // code to execute when the value of i is equal to the value of j
}
```

### Regular Expression Match

Regular expression match is be used for string type.

```altscript
file_name : string;
...
match (file_name)
{
    on "[a-z]+\.txt" => results: foreach (result in results) print(result);
}
```
Regular expression match checkes if the string given in the control_expression matches the regular expression given in the
on clause. The matched result, which is in the type of stream of string, can be saved a in variable by "=> results", where
results is the variable name.

### Pattern Match

Pattern match can be used for any data type including array, stream, tuple, and string.

```altscript
s : int...;   // an integer stream
...
match (s)
{
   on {* => first, [1..9, 20, 100..109] => second, ...=> rest} => result:
   {
       s1 := result.first;    // s1 is in the type of int, it is the first element of the s and can be any value
       s2 := result.second;   // s2 is in the type of int, it is the first element of the s and will be an integer
                              // in the ranged of 1..9, or 100..109, or the value of 20
       s3 := result.rest;     // s3 is a string stream and contains the rest of the elements of s
   }
}
```
A pattern is represented in a pair of curly brackets. The result of the match can be optionally saved in a variable by
"=> <variable_name>", and the type of the result is a tuple if the control_expression is in composite type such as
array, stream, tuple, and string. In the above example, the type of the result is

```altscript
(first:int; second:int; rest:int...)
```
Each element in the pattern represents an element in the tuple type. The tuple elements can be named by the expression
"=> <name>".

The type of the result is the same type of control_expression if the type of the control_expression is a non-composite
type. For instance:
 
```altscript
s : int;
...
match (s)
{
   on {..3} => result:
   {
       s1 := result;    // s1 is in the type of int, it is has the value less or equal to 3
   }
}
```
The following pattern elements are provided:
 
```altscript
on {1, 2, 3}            // for exact value match.
on {*}                  // for wildcard match
on {0..9}               // for a range match
on {..9}                // for a range match without floor, <=9
on {9..}                // for a range match without ceiling, >=9
on {[8, 9, 11..20]}     // match one of the sub patterns given in the list
on {...}                // match any number of elements until next match satisfies or to the end
```
 
A pattern element (except for the ... pattern element) can have a repeating suffix to indicate how any times of the this pattern
can be repeatively used:
 
```altscript
on {6...}          // match any number of elements that has the value 6, including the case of no match
on {6...2}         // match any up to 2 elements that has the value 6, including the case of no match
on {6:2}           // match exacly 2 elements that has the value 6
``` 
A match pattern can be nested for nested composite data structure. For example:
 
```altscript 
s : string... ...;   // a stream of stream string
t :string...;        // a stream of string
match (s)
{
   on {* => first, {*, "mnf"} => second, t:2 => third, ... => rest} => results:
   {
       x := results.first;            // x is the first string stream in s
       y := results.second;           // y is the second string stream in s where the second string equal to "mnf"
       z := results.third;            // z is a tuple of two string streams, the first is the third string stream
                                      // and the second is the the fourth string stream and both has the same value of t.
       foreach (str in results.rest)
       {
            e := str;                 // e is the each string in the rest string stream of s
       }
   }
}
```  
 
The notation ... for any number of match as well as the notion ... for any number of repeating match can only be used
for string, stream and array, i.e. the container has the same element type. It can not be used for tuple. This is
because the result from the match with notation ... is a stream that requires the same element type.
 
Either regular expressions or match patterns can be used for a string. For instance:
```altscript
s: string;
match (s)
{
   on {*:2 => first, ['A'..'F', 'a'..'f', '0'..'9']...2 => second} => result:
   {
       s1 := result.first;   // s is the tuple of (char, char) that contains the first two characters of s
       s2 := result.second;  // s2 is the string that contains zero or up to 2 characters from the third characters in s
                             // each character is a valid hexical digit
   }
}
``` 

