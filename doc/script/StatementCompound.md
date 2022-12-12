# Compound Statement

A compound statement typically appears as the body of a routine or as a component statement with another statement. A compound statement is a list of [statements](Statements.md) and [name declarations](Declarations.md) within a pair of curly brackets. In a particular case, a compound statement can be empty, which is just a pair of curly brackets. Statements and declarations appearing in a compound statement are separated by semicolons. Therefore, semicolon is not required for the last statement or declaration in a compound statement. An example of compound statement:
```altro
{
   a[i]++;
   b[i]--;
   c[i] = a[i] + b[i]
}
```

A compound statement can be used as a single statement wherever a statement is called for. A compound statement introduces a new nested block scope in which lives all names that are declared within the compound statement. Any name declared in the compound statement is not visible outside the statement, and names in the statement can [shadow names](https://en.wikipedia.org/wiki/Variable_shadowing) from outside the statement. The lifetime of any name declared in the compound statement is confined to that statement. All values live in the block scope are destroyed when execution leaves the compound statement.

A compound statement is most often used as the bodies of another compound statement, a conditional statement, a branch statement or a loop statement. A compound statement can also be used as the implementation block of a [routine interface](RoutineInterface.md). If the routine interface is a function interface that has an output specification, a return value must be provided in the compound statement (see [return statement](StatementsJump.md) for more information).

A compound statement can also be used to provide a [concrete free functor](FreeFunctor.md), or a lambda function, for an argument specified in the type of an abstract free functor (or a lambda function interface). For example:
```altro
{
   func fn_sum (nums: double...;  f: fn(x: double): double): double
   {
       sum:=0.0;
       foreach (n in nums) sum += f(n);
       sum
   }
   cubic_sum := fn_sum((1,2,3,4), {x³});
}
```
Here the input parameter `f` in function `fn_sum` expects a free functor with the interface `(x:double):double`, and in calling the function `fn_sum`, the actual parameter provided for `f` is a compound statement `{x³}`, which is actually the body of the free functor (lambda function) `fn(x: double): double {x³}`.

