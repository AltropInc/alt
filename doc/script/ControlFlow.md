# Control Flow

Altscript provides follwing control flow constructs:
  * Compound
  * Conditional
  * Repeated
  * Branched
  
## Conditional

Conditional evaluation allows portions of code to be evaluated or not evaluated depending on the value of a conditional expression.

A conditional expression is an expresion that evalutes to a value convertable to a boolean value true or false.
The boolean value false, zero value, null value, any empty container are convereted to boolean false and everything else
is convereted to true.

A conditional expression can also be a type assumption expression in the format of "expresion => tag is type". For example:

```altscript
x := (3, "text", 3.14);
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

