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

Expressions are divided into different levels according to the precedence of the operators they are using. Primary expressions are in the inmost level, which is level 0. Primary expressions with operators of the highest precedence (postfix operators) constitute compound expressions at level 1. Compound expressions at level 1 with operators of the operators of the second highest precedence (prefix operators) compound expressions at level 2. And this build-up of expressions is done until the level of the compound expressions using the operators at the lowest precedence. 

## Primary Expressions (Level 0)

Primary expressions are the building blocks of more complex expressions. They may be literals, names, names qualified by the scope-resolution operator (::), an expression enclosed in a pair of parentheses, or a routine call (excluding calls to functions named in special operator strings)

*primary-expression*:<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*literal*](Literals.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**self**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**owner**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**selfclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**ownerclass**](SelfAndOwner.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**root**](Object.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[**super**](Constructor.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*name*](Names.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;::[*name*](Names.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;(expression)
   
A literal is a constant primary expression. Its type depends on the form of its presentation. For complete information about specifying literals, see [Literals](Literals.md).

The keyword `self` appearing in a class body refers to the current instance created by this class or by a derived class of this class, and `selfclass` refers to the type of `self`. The keyword `owner` appearing in a member class body refers to the enclosing instance of the current instancecreated by the member class or by a derived class of the member class, and `ownerclass` refers to the type of `owner`. For complete information about `self` and `owner`, see [self, selfclass, owner, and ownerclass](SelfAndOwner.md). The keyword `self` appearing in a class body also refers to this class when it is used in [constructor delegation](Constructor.md) and
the keyword `super` appearing in a class body refers to the super class of this class, which is ised in [constructor chaining](Constructor.md).

The keyword `root` refers to the Altro application root under which all Altro objects are created. See [Object and Object Class](Object.md) for the description of application root.

The scope-resolution operator (::) followed by a name refers to a name at global scope, not a member name.

An expression enclosed in parentheses is a primary expression.

## Level 1 Compound Expressions (Postfix Expressions)

Postfix expressions consist of primary expressions and possibly followed by postfix routine calls. A postfix routine call can be one of the following:

* a call to member selection operator, as in `p.member`
* a constructor or function call with parentheses, such as in `foo()` and `bar(1,2)`, or
* a call to a function that uses a [postfix operator](Operators.md) as its name, such as `a[2]`,  `n++`, `x²`

Here is a list of postfix operators:
| Op     | unicode  | Name             | category      | precedence    | associativity    | is lvaue operator | returns lvalue  |
|:------ |:-------- |:---------------- |:------------- |:------------- |:--------------   | ----------------- | --------------|
| []     |          | subscript        | access        | Postfix    | left             | false             | true          |
| ++     |          | post increment   | Incrementable | Postfix    | left            | true             | false         |
| --     |          | post decrement   | Incrementable | Postfix    | left            | true             | false         |
|        | ²        | exponent 2       | scalable      | Postfix    | left            | false             | false         |
|        | ³        | exponent 3       | scalable      | Postfix    | left            | false             | false         |

*postfix-expression*:<br>
&nbsp;&nbsp;&nbsp;&nbsp;*primary-expression*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*postfix-expression*.*name*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*postfix-expression*(*expression-list*)<br>
&nbsp;&nbsp;&nbsp;&nbsp;*postfix-expression* *postfix-operator*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*postfix-expression*\[*expression*\]<br>

The postfix expression with operator `[expression]` is a subscript expression which is typically used to specify an element of a container in terms of the index expression inside the brackets`[]`. 

A constructor call or a function call with a parenthesized expression list is a postfix expression. The expression list, possible empty) is the actual parameters supplied to the call. See [Routines](Routine.md) for the description of routines calls. If the expression of the routine call has just one input expression, the parentheses can be omitted and in this case the expression is a binary expression, not not a postfix expression.

## Level 2 Compound Expressions (Prefix Expressions)

Prefix expressions are postfix expressions possibly following a prefix operator. Here is a list of prefix operators:


| Op     | unicode  | Name             | category      | precedence    | associativity    | is lvaue operator | returns lvale |
|:------ |:-------- |:---------------- |:------------- |:------------- |:--------------   | ----------------- | --------------|
| +      |          | positive         | addable       | Prefix     | right            | false             | false         |
| -      |          | negative         | addable       | Prefix     | right            | false             | false         |
| \|()\|   |          | absolute         | Negatable       | Prefix     | right            | false             | false         |
| ⌈()⌉      |          | ceiling         | addable       | Prefix     | right            | false             | false         |
| ⌊()⌋      |          | floor         | addable       | Prefix     | right            | false             | false         |
|        | √        | square root      | scalable      | Prefix     | right            | false             | false         |
|        | ∛        | cubic root       | scalable      | Prefix     | right            | false             | false         |
| ++     |          | increment        | Incrementable | Prefix     | right           | true             | true         |
| --     |          | decrement        | Incrementable | Prefix     | right           | true             | true         |
| !      |          | logical not      | logical       | Prefix      | right            | false             | false         |
| ~      |          | flip             | clusivity     | Prefix      | right            | false             | false         |

A prefix operator is the operator placed before the operand (the owner). Users can define any prefix operator with the keyword `prefix`. See [Operators](Operators.md) for a complete description of the prefix operator. These operators have right-to-left associativity.

*prefix-expression*:<br>
&nbsp;&nbsp;&nbsp;&nbsp;*postfix-expression*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*prefix-operator* *prefix-expression*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*prefix-operator-opening* *prefix-expression* *prefix-operator-closing* <br>
&nbsp;&nbsp;&nbsp;&nbsp;*prefix-operator-opening* *expression* *mirrored-prefix-operator-opening* 

If the prefix operator is a multipart including an opening part and a closing part and the closing part string is a horizontally-mirrored string to the opening part,  such as in `⌈x⌉`, the expression between the opening part and a closing part can be any expression. In this case, the opening part and the closing part serve as a pair of parentheses, for instance, `⌈x+y⌉`.

## Compound Expressions from Level 3 ~ 13

All other compound expressions at level N are made of operators at precedence level N and expressions at the levels below:

*compound-expression-level-n*:<br>
&nbsp;&nbsp;&nbsp;&nbsp;*compound-expression-level-n-1*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*compound-expression-level-n* *operator-at-level-n* *compound-expression-level-n-1*<br>
&nbsp;&nbsp;&nbsp;&nbsp;*compound-expression-level-n* *operator-at-level-n-suffix* *compound-expression-level-n-1* <br>

Altro has the following precedence level:

| Precedence Group     | Level  | Operators      |
|:-------------------- |:------ | :---------------------------------------- |
| Postfix              | 1      | [()], ., ², ³, --, ++,                    |
| Prefix               | 2      | +, -, √, ∛, \|()\|, ⌈()⌉, ⌊()⌋, ++, --, !, ~  | 
| Exponent             | 3      | ^                                         |
| Scaling              | 4      | \*, /, %, &                              |
| Addition             | 5      | +, -, .., \\, \|                              |
| Shift                | 6      | <<, >>, 
| Relation             | 7      | ==, !=, <, >, >=, <=, \~, !\~,                         |
| Logical And          | 8      | &&                              |
| Logical Or           | 9      | \|\|,                              |
| Condition            | 10     | .?, ?:                              |
| User Defined         | 11     |                              |
| Assignment           | 12     | =, +=, -=, \*=, /=, %=, /#=, ^=, \\=, \|=, &=, >>=, <<=, <-, @=, @@=, -> |
| Sequencing           | 13     | ,


The smaller the order of level, the higher the precedence in grouping expressions.

Here is the example of compound expression at `Addition` level (level 5) is made from the compound expression at `Scaling` level (level 4):
```altro
x*y + z/2
```
Expressions `x*y` and `z/2` are level-4 compound expressions and they constitute a compound expression at level 5 using the operator `+` at level 5.

Any user defined member function with one input parameter can be used as `User Defined` operator at level 11. For example:
```altro
object integer_container
{
    func ∋ (element: int): bool { /* returns true if this contains the given element */ }
}
```
Here the member function `∋` can be used as a user-defined operator at level 11 and the expression call to the function can use a compound expression at level 11 instead of using a parenthesized postfix expression:
```altro
integer_container ∋ x * y + 1
```
Here the expression check if the object integer_container contains the value `x * y + 1`.

An operator can be multiparted where its name is separated in multiple sections as in:<br>
*operator-name* *operator-suffix1* *operator-suffix2* ...<br>
In this case, a compound expression at level n will be made up by appending a compound expression at level n-1 to each suffix of the operator. For example,

```altro
object integer_container
{
    func ∋ (element: int) from (start_index: int) to (end_index: int) : bool
    { /* returns true if this contains the given element in a given range*/ }
}
```
Here the member function `∋()from()to()` is a multipart operator with an operator name `∋` and suffixes `from` and `to`. Here is an example of a compound expression to call this member function: 
```altro
integer_container ∋ x * y + 1 from start_index to start_index + 7
```

Note that if any section of a multipart operator contains more than one input parameters, parentheses must be used for this section. For example,
```altro
object integer_container
{
    func ∋ (element: int) between (start_index: int; end_index: int) : bool
    { /* returns true if this contains the given element in a given range*/ }
}
```
And the expression to call this member function will be:
```altro
integer_container ∋ x * y + 1 between(start_index, start_index + 7)
```

Note that if an operator suffix uses an operator string, it cannot choose a built-in operator name that has a higher precedence than the precedence of the first part of the operator name. For instance
```altro
object integer_container
{
    func ∋ (element: int) | (start_index: int) .. (end_index: int) : bool
    // Error: Precedence of an operator suffix cannot be higher than the given operator
}
```
The above declaration is ill-formed because the operator name `|` is a built-in operator name that has a precedence of `Addtion`, which is higher than the precedence of `User Defined`.

