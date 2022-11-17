# Operator

An operator is a special [function](FunctionClass.md) name composed of operator characters. The following characters are operator characters:
```altro
! @ $ % & * - + = / \ ? > < ~ | ^ # . [ ] ÷ 
```
Operator characters also include:
* Mathematical operators '\u2000' ~ '\u22FF', except for '\u221E' (∞)
* Modifiers '\u02B0' ~ '\u02FF'
* General punctuations '\u2013' ~ '\u204A'
* Arrows, mathematical operators, and miscellaneous technical  '\u2190' ~ '\u23FF'
* Number superscripts '\u00A1' ~'\u00BF'

A operator character will discontinue an identifier sequence on the lexical grammar of Altro. For example, in
```altro
width*height
```
the operator character '\*' terminates the identifier sequence  "width". In
```altro
distance²
```
the operator character '²' terminates the identifier sequence  "distance".

On the other hand, a non-operator character will discontinue an operator sequence. For example, in
```altro
width>=height
```
the starting character 'h' in identifier "height" terminates the operator sequence ">=".

The operator characters  '\[' and '\]' are separators. A separator will terminate both identifier and operator sequences. The operator character '.' is a also a separator when it is used for function name, but it is not an operator character when it is used as a decimal point or it is used in the combination `..` for range token or `...` for [stream](Stream.md) token.

The following combinations of operator characters are not allowed, and these combination will automatically terminate the operator sequence:
```altro
:<   =<   ><
=+  =-  =*  =/  =%  =|  =&  =~  =!  =^  =? 
```
For instance:
```altro
pi=-3.14
```
Here the operator sequence starts with the character '=' and terminates  with the character '-' because operator name "=-" is not allowed. As result, we get lexical elements `pi`, '=`, `-`, and `3.14`.

An operator can be used as a function name just like an [identifier](Identifier.md]. For examples:
```altro
func + (x, y: int) : int;                   // the operator name + is used for this function 
func ++ (): ownerclass;                     // the operator name ++ is used for this function 
func ∑ (int_stream: int...): int;           // the operator name ∑ is used for this function 
prefix func |()|: ownerclass;               // the operator || is a prefix operator where the first character is put at front
func [(start: int) .. (end: int)]: string   // the operator name [..] that is separated by multiple input sections 
```
We can use the above operator names to make the following function calls:
```altro
a+b
a++
∑S
|a|
s[2..3]
```
Note that ∑ is an operator name and `∑S` are two lexical elements: `∑` (the operator) and `S` (the operand).

In Altro, you can combine any number of operator characters to form an operator name (with a few restrictions we have described before). This requires you to use a white space to separate two operator names. For example, in expression,
```altro
x+++--y
```
`+++--` is treated as one operator in Altro and it is wrong if you acually mean the following expression:
```altro
x++ + --y
```

## Built-in Operators

Altro provides a complete collection of built-in operators across all of its primitive types in numeric, enum, enumset, character and etc.

The following builtin operators are provided with certain properties regarding to precedence and associativity:

| Op     | unicode  | Name             | category      | precedence    | associativity    |
|:------ |:-------- |:---------------- |:------------- |:------------- |:--------------   |
| ::     |          | scope            | access        | scope(0)      | left             |
| []     |          | selection        | access        | selection(1)  | left             |
| .      |          | selection        | access        | selection(1)  | left             |
| *      |          | decomposition    | access        | postfix(2)    | right            |
| ==     |          | equal            | equality      | relation(8)   | left             |
| !=     | ≠        | inequal          | equality      | relation(8)   | left             |
| >      |          | greater          | comparible    | relation(8)   | left             |
| <      |          | less             | comparible    | relation(8)   | left             |
| >=     | ≥        | greater or equal | comparible    | relation(8)   | left             |
| <=     | ≤        | less or equal    | comparible    | relation(8)   | left             |
| ..     |          | between          | comparible    | addition(6)   | left             |
| +      |          | add              | addable       | addition(6)   | left             |
| -      |          | substract        | addable       | addition(6)   | left             |
| *      |          | multiply         | scalable      | scaling(5)    | left             |
| /      |          | divide           | scalable      | scaling(5)    | left             |
| %      |          | modulo           | scalable      | scaling(5)    | left             |
| ^      |          | exponent         | scalable      | exponent(4)   | right            |
| /#     | ÷        | int divide       | scalable      | scaling(5)    | left             |
| +      |          | positive         | addable       | postfix(2)    | right            |
| -      |          | negative         | addable       | postfix(2)    | right            |
|        | ²        | exponent 2       | scalable      | Prefix(3)     | right            |
|        | ³        | exponent 3       | scalable      | Prefix(3)     | right            |
|        | √        | square root      | scalable      | postfix(2)    | right            |
|        | ∛        | cubic root       | scalable      | postfix(2)    | right            |
| ++     |          | increment        | Incrementable | Prefix(3)     | left             |
| --     |          | decrement        | Incrementable | Prefix(3)     | left             |
| ++     |          | post increment   | Incrementable | postfix(2)    | right            |
| --     |          | post decrement   | Incrementable | postfix(2)    | right            |
| ~      | ≅, ∈    | regex equal      | reg-equality  | relation(8)   | left             |
| !~     | ≇ ∉     | regex inequal    | reg-equality  | relation(8)   | left             |
| !      |          | logical not      | logical       | pistfix(2)    | right            |
| ||     | ⋁       | logical or        | logical       | logic-or(10)  | left             |
| &&     | ⋀       | logical and       | logical       | logic-and(9)  | left             |
| \      | ⊻       | exclusive or      | clusivity     | addtion(6)    | left             |
| |      | ∪       | inclusive or      | clusivity     | addtion(6)    | left             |
| |      | ∩       | conjunction       | clusivity     | scaling(5)    | left             |
| ~      |          | flip             | clusivity     | postfix(2)    | right            |
| <<     |          | left shift       | shiftable     | shift(7)      | left             |
| >>     |          | right shift      | shiftable     | shift(7)      | left             |
| .?     |          | contain          | containment   | condition(11) | left             |
| ?:     |          | conditional      | access        | condition(11) | right            |
| =      |          | assign           | access        | assignment(12)| right            |
| +=     |          | add assign       | addable       | assignment(12)| right            |
| -=     |          | sub assign       | addable       | assignment(12)| right            |
| \*=     |          | mul assign       | scalable      | assignment(12)| right            |
| /=     |          | div assign       | scalable      | assignment(12)| right            |
| %=     |          | mod assign       | scalable      | assignment(12)| right            |
| /#=    | ÷=       | int div assign   | scalable      | assignment(12)| right            |
| ^=     |          | exp assign       | scalable      | assignment(12)| right            |
| \=     | ⊻=       | excl-or assign   | clusivity     | assignment(12)| right            |
| |=     | ∪=       | incl-or assign   | clusivity     | assignment(12)| right            |
| &=     | ∩=       | conjunct assign  | clusivity     | assignment(12)| right            |
| >>=    |          | L-hift assign    | shiftable     | assignment(12)| right            |
| <<=    |          | R-hift assign    | shiftable     | assignment(12)| right            |
| ,      |          | seperator        | access        | sequence(13)  | right            |

## Operator Precedence

Operator precedence, also referred as [order of operations](https://en.wikipedia.org/wiki/Order_of_operations), determines which operations to perform first in order to evaluate a given expression. Operators with higher precedence become the operands of operators with lower precedence. For example, multiplication is granted a higher precedence than addition. Thus, the expression 3 + 4 * 2 is interpreted as 3 + (4 × 2), not (3 + 4) * 2. Each built-in operator is assigned with a precedence number. The smaller number indicates the higher precedence.

| Precedence Group     | Order  |
|:-------------------- |:------ |
| Scope                | 0      |
| Selection            | 1      |
| Prefix               | 2      |
| Postfix              | 3      |
| Exponent             | 4      |
| Scaling              | 5      |
| Addition             | 6      |
| Shift                | 7      |
| Relation             | 8      |
| Logical And          | 9      |
| Logical Or           | 10     |
| Condition            | 11     |
| User Defined         | 12     |
| Assignment           | 13     |
| Sequencing           | 14     |

All prefix operators have the `Prefix` precedence (2). Therefore, when we apply a prefix operator to an expression that contains operators in lower precedence, we have to use parentheses. For instance, the root symbol √ is traditionally prolongated by a bar (vinculum) over the expression, but we cannot make this notation in Altro lexical. If we use the root symbol √ to define a prefix operator, we have to use parentheses in order to get the desired result:
```altro
√(x+y)
```
Even the prefix operator defined has a closing operator such as in |x| for getting absolute value, we still need to use parentheses for expression containing operators in lower precedence:
```altro
|(x+y)|
```
Otherwise, the expression
```altro
|x+y|
```
will ne interpreted as `(|x) + (y|)`, which is ill-formed.

All user defined operators has the  precedence number 12.

## Operator Associativity

[Operator associativity](https://en.wikipedia.org/wiki/Operator_associativity) is a property that determines how operators of the same precedence are grouped when they appear in a row in an expression. Left associative means we bind operands to an operator in the expression from left to right, while right associative means we  bind operands to an operator in the expression from from right to left. Associativity is required when the operators in an expression have the same precedence in the absence of parentheses. For instance, `+` and `-` have the same precedence and have the left associative property. Consider the expression:
```altro
a - b + c
```
The result is (a - b) + c, not a - (b + c), becuase operands in the expression must be grouped from left to right. Consider another expression using operator with right associative property:
```altro
a = b += c
```
The result is a = (b += c), not (a = b) += c, becuase operands in the expression must be grouped from right to left.

All prefix operators and assignment operators have the right associativity. All user-defined non-prefix operators have the left associativity.
