# Jump Staements

Jumping statements are control statements that transfer execution control from one point to another point in the program. The following jump statements that are provided in the Altro:

* **return** statement
* **break** statement
* **continue** statement
* **gonext** statement

## Break Statement

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
The break statement with label `set_iteration` will cause the execution to jump to the end of the outer loop statement which is labeled by the name `set_iteration`. Without the label given in the break statement, the execution will jump to the end of the inner loop and the execution will continue for the next iteration of the outer loop and the result will not be the first integer set that contains negative numbers 

