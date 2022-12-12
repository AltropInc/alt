# Switch Statement

The **switch statement** evaluates an expression, **the control expression**, matching the expression's value against a series of **case clauses**, and executes statements after the case clause with a matching value. The default clause of a switch statement will be jumped to if no case matches the expression's value. The switch statement syntax is illustrated as below:
```altro
switch (control_expression -> tag)  // -> tag is used to hold the result of the control_expression and is optional
{
    case value1:          // statement to execute when the value is equal to value1;
    case value2, value3:  // statement to execute when the value is equal to value2 or value3;
    case value4..value6:  // statement to execute when the value is in the range of value4 and value6;
    case value7, value9..value11, value13:
                          // statement to execute when the value is value7, value13
                          // or in the range of value9 and value11;
    default:              // code to execute if none of the listed cases or conditions satisfy;
}
```
The case value must be literal or constant. It doesn't allow variables and must be unique. Using a case test requires the control expression be of type of integral, character, enum, enumset, string, and type. The value range can only be used for integral, character, and enum type. Example:
```altro
func fruit_picker() : string { /* return some fruit name */ }
fruit_price := 0.0;
switch (fruit_picker())
{
  case "Oranges":
    fruit_price = 0.79;
  case "Mangoes", "Papayas":
    fruit_price = 1.79;
  default:
    fruit_price = 0.0;
}
```

## Fall Through or Break

Only the matching block is executed. That is, the execution of the switch statement is completed after the statement of the matched case is executed. This behavior is different from the [switch statement in other languages derived from C](https://en.wikipedia.org/wiki/Switch_statement).

However, if a [fallthrough statement](StatementsJump.md) is encountered, the execution flow will fall through to the statement of the next case clause. For example,
```altro
switch (options)
{
    case option_a:
    {
        // do something for option A
        fallthrough // execution will not complete from this switch but will go to the next case.
    }
    case option_a1:
    {
        // do something for option A1
    }
}
```
Note that the fallthrough will directly execute the statement of the next case clause without checking if the next case is a match.

If a [break statement](StatementsJump.md) is encountered, execution of the switch statement is then completed and then the execution will continue at the end of the switch statement, or if the break statement has a label, at the end of an outside breakable statement that is labeled with the same name.

## Case on Type

The switch statement can be used for type checking if the control expression is a type expression. For example,
```altro
func numeric_generator(): numeric { /* generate some number in any numeric type */ }
switch (typeof numeric_generator() -> result)
{
    case int: // do something when the result is an integer
    case double:  // do something when the result is a double
}
```
If the expression in the type expression is a variable name alone, the tag name can be omitted, and the type of the name in the case-clause is statically assumed to be the type given in the case condition. for instance:
```altro
func numeric_generator(): numeric { /* generate some number in any numeric type */ }
n := numeric_generator(); 
switch (typeof n)
{
    case int:     /* do something with n as an integer */;
    case double:  /* do something with n as a doubler */;
}
```

Note that when using a tag to hold the result of an expression, the tag is a reference if the expression is an [l-value expression](LvalueExpression.md). Changing the value through the tag will affect the original l-value:
```altro
n : numeric = 2; 
switch (typeof n -> tag)
{
    case int:     tag += 1;
    case double:  tag = 3.0;
}
// n holds the integer value 3 here.
```
