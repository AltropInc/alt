# Loop Statements

Loop statement is a feature that facilitates the execution of a piece of code repeatedly while some condition evaluates to true. Altro provides four types of loop statements:

* While loop
* Repeat loop
* For loop
* Foreach loop

## The `while` Loop

The `while` loop is a pre-tested loop. It is used in situations where the number of iterations is unknown at first. The block of statements is executed in the while loop until the condition specified in the while loop become unsatisfied, and thereafter, the loop ends and the execution flow moves to the next statement after the loop. The while loop ahs the following sytax:

**while** (*expression*) *statement*

The expression is the condition expression and must be evaluated to a boolean value. The condition expression is evaluated first (pre-tested). The statement is the while-statement body and is executed if the condition expression evaluates to true. The condition expression is re-tested and the statement is repeated as long as the condition expression is re-evaluated to true.

Exmaple:
```altro
count := 0;
nums : int...;
while (count < 10)
{
    count += 1;
    nums += count;
}
```
After the `while` loop, the integer stream `nums` contains (1,2,3,4,5,6,7,8,9,10).

## The `repeat` Loop

The `repeat` loop is a post-tested loop, that is, the statement for the loop body is executed first, and then the condition expression is tested. The `repeat` loop  has two forms:

**repeat** *statement* **until** (*expression*)

or

**repeat** *statement* **while** (*expression*)

When the condition expression is an `until` case, the statement will be repeated if the condition expression is evaluated to false. When the condition expression is a `while` case, the statement will be repeated if the condition expression is evaluated to true.

Exmaple:
```altro
count := 0;
nums : int...;
repeat
{
    count += 1;
    nums += count;
}
until (count==10)
```

## The `for` Loop

The general form of the for statement can be expressed as follows:

**for** (*initialization*; *termination*; *modification*) *statement*

* The initialization part has a list of expressions to initializes the loop, and they are executed once, as the loop begins.
* The termination expression is a boolean condition and when it evaluates to false, the loop terminates. If it evaluates to true, the statement (the for-loop body) is executed.
* The modification part has a list of expressions to be invoked after the for-loop body is executed; The modification part is normally used to increment or decrement loop indices.
Exmaple:
```altro
result : int...;
for (i,j:=0,1; i+j < 20; i+=2, j+=3)
{
    result += i+j;
}
```
After the `for` loop, the integer stream `nums` contains (1,6,11,16).

## The `foreach` Loop

The foreach loop statement is for traversing items in an instance of an [iterable](TypeIterable.md) type. Unlike other for loop statements, a `foreach` loop has no explicit control expression to tell when the loop is out, rather, it essentially tells "do this to everything in this collection". An iterator is implicitly used as the means of traversal. The syntax is roughly as follows:

**foreach** **backward**<sub>opt</sub> (*element* *filter*<sub>opt</sub> **in** *iterable*) *statement*

The statement starts with the keyword `foreach`. If `backward` present, the traversal will start from the last element toward the first. Instead of declaring and initializing a loop control expression, you declare a variable that represents an element, possibly by a `filer` type, and followed by the keyword `in`, which is then followed by an expression of an iterable instance. The variable is actually an [iterator](TypeIterable.md) that can be used to access an element in the iterable instance in each loop traversal.

The following `foreach` loop multiplies each number in an integer stream by 10:
```altro
int_stream : int... = (1,2,3,4);
foreach (num in int_stream)
{
    num *= 10;
}
```
However, if the collection is constant, you cannot use the iterator to alter the value in the collection:
```altro
const int_stream : int... = (1,2,3,4);
foreach (num in int_stream)
{
    num *= 10; // Error: cannot alter the value of a constant name
}
```
If a filter type is given, elements that are not subtype of the filter type will be skipped. For example:
```altro
a_tuple := (0, 3.2, "xyz", 2);
sum_of_integers:=0;
foreach (ele: int in a_tuple)
{
    sum_of_integers += ele;
}
```
Here only integer elements (the first and the foruth) are added to `sum_of_integers`.

## Break and Continue in Loop

Break-statement and continue-statement can be used in a loop statement body to change the execution flow. The break statement is used to terminate the loop immediately when it is encountered. The continue statement is used to skip the current iteration of the loop and the control flow of the program goes to the next iteration. When you have more than one loop nested, you can use labels with break and continue statements to break or continue a specific outer loop by providing labels. See [Jump Statements](StatementsJump.md) for detail.
