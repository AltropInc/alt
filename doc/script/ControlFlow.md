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
to decide witch portions of the code to be executed.

The Altscript switch statement is used for this purpose. Syntax:

```altscript
switch (control_expression => tag)  // => tag is used to hold the result of the control_expression and is optional
{
    case value1:  // code to execute when the value is equal to value1;
    case value2, value3:  // code to execute when the value is equal to value2 or value3;
    when conditional_expression1: // code to execute when conditional_expression1 is evaluated to true;
    when conditional_expression2: // code to execute when conditional_expression2 is evaluated to true;
    match pattern1: // code to execute when pattern1 is matched;
    match pattern2: // code to execute when pattern2 is matched;
    is type1: // code to execute when the type of value is type1;
    is type2: // code to execute when the type of value is type2;
    default: // code to execute if none of the listed cases or conditions satisfy;
}
```
The "case" test is the equality test. The case value must be literal or constant. It doesn't allow variables and must be unique.
Using a case test requires the control expression be of type of integral, enum, enumset, string, and type.

The "when" condition test is the execution of the following conditional expression that typically containes the value of the
control expression, optionally represented by the tag. There is no limitation of the type of control expression if "when" condition
test is used.

The "match" pattern test is to check the value of the control expression and see if it matches the given pattern.

The "is type" test is to check the type of the value evaluated from the control expression.

All tests are performed top to bottom, however, a case test must appear before any other kind of tests. If a case is found, condition is true,
or a pattern is matched, the first corresponding block will be executed and the control flow is done if there is no continue to fallthrough.
In the fallthrough, the case, the condition or the pattern whill be tested again and the block will be executed only when the test is
sucessful. Therefore, the "case" test for equality in fallthrough will be ignored.

The "match pattern" test works for string, array, stream and tuple. For string, the match pattern is a regular expresion. For example

```altscript
switch (file_name)
{
    match r"[a-z]+\.txt" => results: foreach (result in results) print(result);
}
```
For array, stream and tuple, the match pattern is the composite pattern that contains a list of comma seperated sub patterns.

```altscript
switch (x):
    match (): print("Empty container matched");
    match (1, 3, "test", null): print("a specific tuple value matched")
    match (start, *, "end"]: print("First element is ", start, ", the middle is any, and the last is \"end\"")
    match (start, ..., "end"]: print("First element is ", start, ", multiple middles ignored, and the last is \"end\"")
    match (42, ...): print("First element is 42 and the rest are ignored")
    match ((*, 32), ...): print("A nested patttern. The first has two element and the second is 32 and the rest are ignored")
}
```


        
