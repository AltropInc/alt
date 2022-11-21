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
&nbsp;&nbsp;&nbsp;&nbsp;[**self**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**owner**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;;[**selfclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**ownerclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*name*](Names.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;::[*name*](Names.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;(expression)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[routine-call](Routine.md)<br>
   
A literal is a constant primary expression. Its type depends on the form of its presentation. For complete information about specifying literals, see [Literals](Literals.md).

The keyword `self` appearing in a class body refers to the current instance created by this class or by a derived class of this class, and `selfclass` refers to the type of `self`. The keyword `owner` appearing in a member class body refers to the enclosing instance of the current instancecreated by the member class or by a derived class of the member class, and `ownerclass` refers to the type of `owner`. For complete information about `self` and `owner`, see [self, selfclass, owner, and ownerclass](SelfAndOwner.md).

The scope-resolution operator (::) followed by a name refers to a name at global scope, not a member name.

An expression enclosed in parentheses is a primary expression.

A call to a routine in the forma of `function_name(optional-input-list)`, except for calls to functions with pecial operator strings, is a primary expression. See [Routine](Routine.md) for complete information about routine calls.







