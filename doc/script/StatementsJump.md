# Jump Staements

Jumping statements are control statements that transfer execution control from one point to another point in the program. The following jump statements are provided in the Altro:

* **break** statement
* **continue** statement
* **fallthrough** statement
* **return** statement

## The `break` Statement

The break statement is used to terminate the execution of a breakable statement. Breakable statements includes:

* [**while** statement](StatementLoop.md)
* [**repeat** statement](StatementLoop.md)
* [**for** statement](StatementLoop.md)
* [**foreach** statement](StatementLoop.md)
* [**switch** statement](StatementSwitch.md)
* [**match** statement](StatementMatch.md)

Here is an example of break statement in a `foreach` loop statement:
```altro
integers: int... = (1,8,5,-1,2,-5);
sum_of_start_positives := 0;
foreach (i in integers)
{
    if (i < 0) break;
    sum_of_start_positives += i;
}
// sum_of_start_positives gets the value of 14
```
A break statement can have a label that identifies an outside breakable statement labeled by a matched name. Here is an example of a break with label:
```altro
int_sets: int... ... = ((2, 102, -5), (1, 306), (22, 17, -7, 65));
first_set_contain_negative : int...;
set_iteration: foreach (int_set in int_sets)
{
    foreach (i in int_set)
    {
        if (i < 0)
        {
            first_set_contain_negative = int_set;
            break set_iteration;
        }
    }
}
```
The break statement with label `set_iteration` will cause the execution to jump to the end of the outer loop statement which is labeled by the name `set_iteration`. Without the label given in the break statement, the execution will jump to the end of the inner loop and the execution will continue for the next iteration of the outer loop and the result will not be the first integer set that contains negative numbers. 

## The `continue` Statement

The continue statement is used to skip the rest of the code in the current iteration in a loop statement and starts the next iteration. The unlabeled form skips to the end of the innermost loop's body. The following loop statements may have a continue statement in their bodies:

* [**while** statement](StatementLoop.md)
* [**repeat** statement](StatementLoop.md)
* [**for** statement](StatementLoop.md)
* [**foreach** statement](StatementLoop.md)

Here is an example of continue statement in a `foreach` loop to sort out an integer set based on whether the set contains zero:
```altro
int_sets: int... ... = ((2, 0, -5), (1, 306), (22, 17, 0, 65), (9));
sets_with_zero : int... ...;
sets_without_zero : int... ...;
foreach (int_set in int_sets)
{
    if (int_set.find(0))
    {
        sets_with_zero += int_set;
        continue;  // end of the current iteration body and start to check next integer set
    }
    sets_without_zero += int_set;
}
```
A continue statement can have a label that identifies an outer loop statement labeled by a matched name. The following code illustrates an labeled continue statement:
```altro
int_sets: int... ... = ((2, 102, -5), (1, 306), (-22, 17, -7, 65), (2));
all_sets_with_negative : int... ...;
set_iteration: foreach (int_set in int_sets)
{
    foreach (i in int_set)
    {
        if (i < 0)
        {
            all_sets_with_negative += int_set;
            continue set_iteration;
        }
    }
}
```
The continue statement with label `set_iteration` will skip the current iteration of the outer loop statement which is labeled by the name `set_iteration`, and start the next iteration to check the next integer set to see if the set contains negative numbers. Without the label given in the continue statement, the execution will skip the inner iteration and start to check if the next integer in the current integer set is negative, and may cause the same integer set to to be added into the result multiple times.

## The `fallthrough` Statement

A branch statement such as switch and match causes the program to begin executing a selected block, or branch code, that matches the case given for that block. Once the branch code is executed, the branch statement is completed. Unlike the switch statement in C-derived languages, the execution does not continue (fallthrough) to the next branch after the matching branch is executed. The **fallthrough** statement is used to implement the C-style fallthrough behavior. It skips the rest of the code in the current branch and continues to execute the code present in the next branch without checking if the case for the next branch is a match. The following branch statements may have a fallthrough statement in their bodies:

* [**switch** statement](StatementSwitch.md)
* [**match** statement](StatementMatch.md)

Here is an example of fallthrough statement in a `switch` statement:
```altro
event: enum (LeftButtonDown, MiddleButtonDown, RightButtonDown);
button := 0;
switch (event)
{
    case LeftButtonDown:
    {
        button = 1;
        fallthrough // execution will not complete from this switch but will go to the next case.
    }
    case MiddleButtonDown:
    {
        ++button;
        fallthrough // execution will not complete from this switch but will go to the next case.
    }
    case RightButtonDown:
    {
        ++button;
        handleButtonDown(button)
    }
}
```

## The `return` Statement

The **return** statement exits the code block of a [routine](Routine.md) and returns to the point where the routine was called. If the routine is an function and the interface of the function has a output specified, the return statement in the associated block must have an [expression](Expressions.md) for the **return value** to be returned. The return statement has the following syntax:

**return** *expression*<sub>opt</sub>

Example:
```altro
func first_matched_position(int_stream: int...; numer_to_match: int): int
{
   for (position:=0; position < int_stream.length(); position++)
      if (numer_to_match == int_stream[position])
         return position;  // exits the function body and returns the index position here
   return -1  // returns -1 here to indicate the number is not found
}
```
If no return statement appears in a routine block, the control automatically returns to the calling point after the last statement of the called routine is executed. In this case, if the interface requires an output, the the last statement must be an expression for the return value:
```altro
func first_matched_position(int_stream: int...; numer_to_match: int): int
{
   for (position:=0; position < int_stream.length(); position++)
      if (numer_to_match == int_stream[position])
         return position;  // exits the function body and returns the index position here
   -1  // the last statement implies the return, and the value of the expression is the return value
}
```
If the function interface has an output type, it's an error if the last statement is not the expression for the value or does not return the value in the specified output type. For example:
```altro
func compare_two(n1, n2: int): int
{
   if (n1 == n2) return 0;
   elif (n1 < n2) return -1;
}  // Error: expect a return value. Not all paths have a return value
```
An error will be caught in the above code because not all programmed paths have a return value. The correct one should be something like below:
```altro
func compare_two(n1, n2: int): int
{
   if (n1 == n2) return 0;
   elif (n1 < n2) return -1;
   else return 1
}
```
It's okay to have no return statement for a function that has no output, or for a [constructor](Constructor.md) or a [destructor](Destructor.md).



