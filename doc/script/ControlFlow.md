# Control Flow

Altscript provides follwing control flow constructs:
  * Compound
  * Conditional
  * Repeated
  * Branched
  
## Conditional

Conditional evaluation allows portions of code to be executed or not executed depending on the value of a conditional expression
(condition).

A condition is an expresion that evalutes to a value convertable to a boolean value true or false.
The boolean value false, zero value, null value, invalid enum value, empty enum set value, and any empty container are converted to boolean false and everything else
is converted to true.

The Altscript if statement tests the condition and it executes the if block if the condition is true and otherwise,
if the else block or elif block is provided, executes the else block or test the elif condition to perform the work
as an if statement is followed. The syntax of if statement:

```altscript
// if statement
if (condition)
{
    // if-block with code to execute if condition is evaluated to true
}

// if statement with else
if (condition)
{
    // if-block with code to execute if condition is evaluated to true
}
else
{
    // else-block with code to execute if condition is evaluated to false
}

// if statement with elif
if (condition1)
{
    // if-block with code to execute if condition1 is evaluated to true
}
elif (condition2)
{
    // elif-block with code to execute if condition1 is evaluated to false and condition2 is evaluated to true
}
// more elif may follow here
else
{
    // optional else-block with code to execute if all conditions are evaluated to false
}

```

A conditional expression can also be a type assumption expression in the format of "expresion => tag is type". For example:

```altscript
x:=(3, "text", 3.14);
if (x[i] => ele is string)
{
    system.out << "the element type is string: " << ele;
}
```

The tag in the type assumption expression is optional. It provides a convenient name for accessing the result of the evaluated expression
and in the block of the true clause of the if statement, the type of ele is statically assumed to be string. If the expression is a varaible name alone,
the tag can be omitted, and the type of the name in the true claise is statically assumed to be the type given in the type assumption expression.
for instance:

```altscript
x : super(int, string, double);
if (x is string)
{
    system.out << "the x type is string: " << x;
}
elif (x is double)
{
    system.out << "the x type is douible: " << x;
}
```

## Branched

A branched control flow executes blocks of code from multiple conditions, like if-else-if ladder statement. In other words, it
evaluates a control expression, and use the evaluated value to test the equality, check a condition, or find a pattern match
to decide witch portions of the code to be executed. Teo statements are used for this purpose: swicth and match

The switch statement syntax:

```altscript
switch (control_expression => tag)  // => tag is used to hold the result of the control_expression and is optional
{
    case value1:          // code to execute when the value is equal to value1;
    case value2, value3:  // code to execute when the value is equal to value2 or value3;
    case value4..value6:  // code to execute when the value is in the range of value4 and value6;
    case value7, value9..value11, value13:
                          // code to execute when the value is value7, value13
                          // or in the range of value9 and value11;
    default:              // code to execute if none of the listed cases or conditions satisfy;
}
```
The case value must be literal or constant. It doesn't allow variables and must be unique.
Using a case test requires the control expression be of type of integral, enum, enumset, string, and type.

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

The type of the result is the same tyoe of control_expression if the type of the control_expression is a non-composite
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
on {6:2}           // match any exacly 2 elements that has the value 6
``` 
A match pattern can be nested for nested composite data structure. For example:
 
```altscript 
s : string... ...;   // a stream of stream string
t :string...;        // a stream of string
match (s)
{
   on {*:2 => first, {*, "mnf"}:2 => second, t:2 => third, ... => rest} => results:
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
