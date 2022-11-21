# Expressions

An expression is a syntactic entity in Altro that specifies the evaluation of a value by a combination of one or more [literal constants](Literals.md), [names](Names.md), as well as [routine calls](Routine.md) ([constructor calls](Constructor.md) or [function calls](Functor.md)). Some function names use special [operator strings](Operators.md) that have particular rules of precedence and association. Altro interprets calls to these functions in a certain order according to the rules bound to their operator name. Parentheses can be used in an expression to modify the normal order of operations so that the terms present inside a pair of parentheses are evaluated first. For instance, the following expression
```altro
x² * (2 + √y)
```
contains a variable name x, a call to exponent function ², a call to function \*, a pair of parentheses that enclose a sub-expression with a literal 2, a call to function +, and a prefix function √ that is applied to a variable name y. Altro interprets this expression in the following order:
* evaluate x
* evaluate x²
* evaluate 2
* evaluate y
* evaluate √y
* evaluate 2 + √y
* evaluate x² + (2 + √y)

## Primary Expressions

Primary expressions are the building blocks of more complex expressions. They may be literals, names, names qualified by the scope-resolution operator (::), an expression enclosed in a pair of parentheses, or a routine call (excluding calls to functions named in special operator strings)

*primary-expression*<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*literal*](Literals.md)<br>
&nbsp;&nbsp;&nbsp;;&nbsp;[**self**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;;&nbsp;[**owner**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;;[**selfclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;;&nbsp;[**ownerclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;;&nbsp;[*name*](Names.md)<br>
&nbsp;&nbsp;&nbsp;;&nbsp;::[*name*](Names.md) ( expression )
&nbsp;&nbsp;&nbsp;;&nbsp;(expression)
&nbsp;&nbsp;&nbsp;;&nbsp;[routine-call](Routine.md)
   
 


