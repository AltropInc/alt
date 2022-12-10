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

## Branch Fall Through and Break

Only the matching block is executed. That is, the execution of the switch statement is completed after the statement of the matched case is executed. this behavior is different that the switch staement in other languages derived from C.

However, if a [gonext statement](JumpStatement.md) is encountered, the execution flow will fall through to the statement of the next case clause. For example,
```altro
switch (options)
{
    case option_a:
    {
        // do something for option A
        gonext // execution will not complete from this switch but will go to the next case.
    }
    case option_a1:
    {
        // do something for option A1
    }
}
```

However, if a [gonext statement](JumpStatement.md) is encountered, the execution 
