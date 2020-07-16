# Control Flow

Altscript provides follwing control flow constructs:
  * Compound
  * Conditional
  * Repeated
  * Branched
  
## Conditional

Conditional evaluation allows portions of code to be evaluated or not evaluated depending on the value of a conditional expression
(condition).

A condition is an expresion that evalutes to a value convertable to a boolean value true or false.
The boolean value false, zero value, null value, any empty container are converted to boolean false and everything else
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

