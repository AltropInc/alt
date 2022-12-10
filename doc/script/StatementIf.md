# If Statement

## If-Statement For Condition Test

If-statement is a conditional statement used to execute different code blocks based on different conditions.  The if-statement tests the condition and it executes the if block if the condition is true and otherwise, if the else block or elif block is provided, executes the else block or test the elif condition to perform the work as an if statement is followed. The syntax of if-statement is illustrated as below:
```altro
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
Example:
```altro
if (time < 10)
  greeting = "Good morning";
elif (time < 20) {
  greeting = "Good day";
else
  greeting = "Good evening";
```
If the time is less than 10, the greeting is "Good morning", if not, but the time is less than 20, the greeting is "Good day", otherwise the greeting is "Good evening".  Note that if the code block has a single statement, the curly brackets for the code block can be omitted.

## If-Statement For Type Checking

An if-statement can be used for type checking when the condition appearing in the statement is a type assumption expression in the format of `expression -> tag is type`. If the actual type of the expression is the subtype of the given type, the expression evaluates to true and the value of the expression is assigned to the tag. For example
```altro
func numeric_generator(): numeric { /* generate some number in any numeric type */ }
if (numeric_generator() -> num is int)
{
    // do something with num which is an integer
}
```
The tag name in the type condition is optional.  In the true-block of the if-statement, the tag name provides a convenient name for accessing the evaluated result of the expression in the type condition. If the expression in the type condition is a variable name alone, the tag name can be omitted, and the type of the name in the true-clause is statically assumed to be the type given in the type condition. for instance:
```altro
func numeric_generator(): numeric { /* generate some number in any numeric type */ }
n := numeric_generator(); 
if (n is int)
{
    // do something with n which is assumed to be an integer in this block
    n = 3.2;  // Error: expect int but a value of double is provided
    n = 3;    // Okay
}
elif (n is double)
{
    // do something with n which is assumed to be an integer in this block
    m1: int = n;       // Error: expect int but a value of double is provided
    m2: double = n;    // Okay
}
```
Note that when using a tag to hold the result of an condition expression, the tag is a reference if the expression is an [l-value expression](LvalueExpression.md). Changing the value through the tag will affect the original value:
```altro
d: int[2]=(1,2);
n: array = d;
if (n[0] -> e is int)
{
   e = 30;
}
// here n[0] holds the integer value 30
```

