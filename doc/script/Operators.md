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

In Altro, you can combine any number operator characters to form an operator (except for a few restrictions we have described before). This requires you to use a white space to separate two operator names. For example, in expression,
```altro
x+++--y
```
`+++--` is treated as one operator in Altro and it is wrong if you acually mean the following expression:
```altro
x++ + --y
```

## Built-in Operators

Altscript provides a complete collection of built-in operators across all of its primitive types in numeric, enum, enum set and character.

The following builtin operators are provided

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
| *=     |          | mul assign       | scalable      | assignment(12)| right            |
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

