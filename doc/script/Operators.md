# Operator

An operator is a special [function](FunctionClass.md) name composed of operator characters.

## Operator Character Sequence

The following characters in the [Basic Latin](https://www.compart.com/en/unicode/block/U+0000) block are operator characters:
```altro
! @ $ % & * - + = / \ ? > < ~ | ^ # [ ] 
```
And the following characters in the [Latin-1 Supplement](https://www.compart.com/en/unicode/block/U+0080) block are operator characters:
```altro
¡ ¦  «  ¬  ±  ²  ³  ·  ¹  »  ¿ ÷ 
```
Operator characters also include the following characters not in the Basic Latin block:
* [Arrows](https://www.compart.com/en/unicode/block/U+2190) '\u2190' ~ '\u21FF'
* [Mathematical Operators](https://www.compart.com/en/unicode/block/U+2200) '\u2200' ~ '\u22FF', except for '\u221E' (∞)
* [Miscellaneous Mathematical Symbols-A](https://www.compart.com/en/unicode/block/U+27C0) '\u27C0' ~ '\u27EF'
* [Supplemental Arrows-A](https://www.compart.com/en/unicode/block/U+27F0) '\u27F0' ~ '\u27FF'
* [Supplemental Arrows-B](https://www.compart.com/en/unicode/block/U+2900) '\u2900' ~ '\u297F'
* [Miscellaneous Mathematical Symbols-B](https://www.compart.com/en/unicode/block/U+2980) '\u2980' ~ '\u29FF'
* [Supplemental Mathematical Operators](https://www.compart.com/en/unicode/block/U+2A00) '\u2A00' ~ '\u2AFF'
* [Miscellaneous Symbols and Arrows](https://www.compart.com/en/unicode/block/U+2B00) '\u2B00' ~ '\u2BFF'
* [Supplemental Punctuation](https://www.compart.com/en/unicode/block/U+2E00) '\u2E00' ~ '\u2E7F'

The double dot `..` is treated as a single token, which is eqivalent to `‥` ('\u2025'), it can be used as a single operator character.

An operator character will discontinue an identifier sequence on the lexical grammar of Altro. For example, in
```altro
width*height
```
the operator character '\*' terminates the identifier sequence  "width", and in the following expression,
```altro
distance²
```
the operator character '²' terminates the identifier sequence  "distance".

On the other hand, a non-operator character will discontinue an operator sequence. For example,
```altro
width>=height
```
Here the starting character 'h' in identifier "height" terminates the operator sequence ">=".

The operator character sequence will also discontinue with an operator character that is also a separator. The following operator characters are separators:
| character | unicode  | Comments                                          |
|:--------- |:-------- |:------------------------------------------------- |
| \[        | \u005B   | Left Square Bracket                               |
| \]        | \u005D   | Right Square Bracket                              |
| ..        | \u005D   | Range (double dots)                               |
| >\u007F   |  >\u007F | All non-basic-latin operator chars are separators |

Therefore, the operator sequence `+=` or `==>` is a single operator, but `[[` or `∛∛` contains two operators. 

A separator will terminate both identifier and operator sequences. The operator character '.' is a also a separator when it is used for function name, but it is not an operator character when it is used as a decimal point or it is used in the combination `..` for range token or `...` for [stream](Stream.md) token.

The following combinations of operator characters are not allowed, and these combination will automatically terminate the operator sequence:
```altro
:<   =<   ><
=+  =-  =*  =/  =%  =|  =&  =~  =!  =^  =? 
```
For instance:
```altro
pi=-3.14
```
Here the operator sequence starts with the character '=' and terminates with the character '-' because operator name "=-" is not allowed. As result, we get lexical elements `pi`, '=`, `-`, and `3.14`.

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

## Multipart Operators

A multipart operator has an operator name separated in multiple sections. For instance, the operator `[]` is a multipart operator (note that the character `[` is a separator) and it has two sections: `[` and `]`, which can be separated by an operand:
```
[x+2]
```
A multipart operator, like a multipart function name, is specified in a [multi-section function interface](RoutineInterface.md). For instance,
```
class string
{
    func [(start: int) .. (end: int)]: string;  // Gets a substring from start index up to end index.
                                                // The character at the end index is excluded
}
x := "abcdefg";
y := x[2..4];   // y gets "cd"
```
Here the function interface uses a multipart operator `[`, `..`, `]`, and the name is separated by operands the `start` and the `end` index. Each section of a multipart operator can only contain one operator sequence. For instance,
```
func [(start: int) >..< (end: int)];  //  Error: Multiple operator sequences are used in the same interface section
```
Because `..` is a separator, `>..<` contains three operator sequences: `>', '..', and '<'.

## Built-in Operators

Altro provides a complete collection of built-in operators across all of its primitive types in numeric, enum, enumset, character, etc.

The following builtin operators are provided with certain properties regarding to precedence and associativity:

| Op     | unicode  | Name             | category      | precedence    | associativity    |
|:------ |:-------- |:---------------- |:------------- |:------------- |:--------------   |
| ::     |          | scope            | access        | Scope(0)      | left             |
| []     |          | selection        | access        | Selection(1)  | left             |
| .      |          | selection        | access        | Selection(1)  | left             |
| *      |          | decomposition    | access        | Prefix(2)     | right            |
| ==     |          | equal            | equality      | Relation(8)   | left             |
| !=     | ≠        | inequal          | equality      | Relation(8)   | left             |
| >      |          | greater          | comparible    | Relation(8)   | left             |
| <      |          | less             | comparible    | Relation(8)   | left             |
| >=     | ≥        | greater or equal | comparible    | Relation(8)   | left             |
| <=     | ≤        | less or equal    | comparible    | Relation(8)   | left             |
| ..     | ‥        | between          | comparible    | Addition(6)   | left             |
| +      |          | add              | addable       | Addition(6)   | left             |
| -      |          | substract        | addable       | Addition(6)   | left             |
| *      |          | multiply         | scalable      | Scaling(5)    | left             |
| /      |          | divide           | scalable      | Scaling(5)    | left             |
| %      |          | modulo           | scalable      | Scaling(5)    | left             |
| ^      |          | exponent         | scalable      | Exponent(4)   | left            |
| /#     | ÷        | int divide       | scalable      | Scaling(5)    | left             |
| +      |          | positive         | addable       | Prefix(2)     | right            |
| -      |          | negative         | addable       | Prefix(2)     | right            |
|        | ²        | exponent 2       | scalable      | Prefix(3)     | right            |
|        | ³        | exponent 3       | scalable      | Prefix(3)     | right            |
|        | √        | square root      | scalable      | Prefix(2)    | right            |
|        | ∛        | cubic root       | scalable      | Prefix(2)    | right            |
| ++     |          | increment        | Incrementable | Prefix(3)     | right           |
| --     |          | decrement        | Incrementable | Prefix(3)     | right           |
| ++     |          | post increment   | Incrementable | Postfix(2)    | left            |
| --     |          | post decrement   | Incrementable | Postfix(2)    | left            |
| ~      | ≅, ∈    | regex equal      | reg-equality  | Relation(8)   | left             |
| !~     | ≇ ∉     | regex inequal    | reg-equality  | Relation(8)   | left             |
| !      |          | logical not      | logical       | Postfix(2)    | right            |
| ||     | ⋁       | logical or        | logical       | Logic-or(10)  | left             |
| &&     | ⋀       | logical and       | logical       | Logic-and(9)  | left             |
| \      | ⊻       | exclusive or      | clusivity     | Addtion(6)    | left             |
| |      | ∪       | inclusive or      | clusivity     | Addtion(6)    | left             |
| |      | ∩       | conjunction       | clusivity     | Scaling(5)    | left             |
| ~      |          | flip             | clusivity     | Prefix(2)     | right            |
| <<     |          | left shift       | shiftable     | Shift(7)      | left             |
| >>     |          | right shift      | shiftable     | Shift(7)      | left             |
| .?     |          | contain          | containment   | Condition(11) | left             |
| ?:     |          | conditional      | access        | Condition(11) | right            |
| =      |          | assign           | access        | Assignment(12)| right            |
| +=     |          | add assign       | addable       | Assignment(12)| right            |
| -=     |          | sub assign       | addable       | Assignment(12)| right            |
| \*=     |          | mul assign       | scalable      | Assignment(12)| right            |
| /=     |          | div assign       | scalable      | Assignment(12)| right            |
| %=     |          | mod assign       | scalable      | Assignment(12)| right            |
| /#=    | ÷=       | int div assign   | scalable      | Assignment(12)| right            |
| ^=     |          | exp assign       | scalable      | Assignment(12)| right            |
| \=     | ⊻=       | excl-or assign   | clusivity     | Assignment(12)| right            |
| |=     | ∪=       | incl-or assign   | clusivity     | Assignment(12)| right            |
| &=     | ∩=       | conjunct assign  | clusivity     | Assignment(12)| right            |
| >>=    |          | L-hift assign    | shiftable     | Assignment(12)| right            |
| <<=    |          | R-hift assign    | shiftable     | Assignment(12)| right            |
| ,      |          | seperator        | access        | Assignment(13)  | right            |

Note that `::` and `.` has special meaning and cannot be used as an operator character in a function name. They are listed here just for lexcial processing purpose.

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

All user defined non-prefix operators has the precedence number 12.

## Prefix Operators

A prefix operator is the operator placed before the operand (the owner), as in prefix increment and decrement operators, positive and negative signs, square root:
```altro
++x  // prefix increment
-x   // negation
√x   // root
!x   // logic not
~x   // bitwise flip
```
A prefix operator must have an empty input interface, and the operand of the prefix operator is the instance of the owner class that contains the prefix operation: 
```altro
class test
{
    prefix func √(n: double);  // Error: Prefix interface should not have input
}
```

All prefix operators have the `Prefix` precedence (2). Therefore, when we apply a prefix operator to an expression that contains operators in lower precedence, we have to use parentheses. For instance, the root symbol √ is traditionally prolongated by a bar (vinculum) over the expression, but we cannot make this notation in Altro lexical. If we use the root symbol √ to define a prefix operator, we have to use parentheses in order to get the desired result:
```altro
√(x+y)
```
However, if the prefix operator is a multipart including an opening part and a closing part, such as in |x|, ⌈x⌉, using parentheses is then unneccessary under certain condition:
```altro
|x+y|   // absolute value of x+y
⌈x+y⌉    //ceiling of x+y
```
Parentheses are only required when the enclosed expression has an operator with the same name of the closing part for different purpose:
```altro
|(x|y)|   // absolute value of x|y
```
The expression placed between the opening part and the closing part can have operator precedence  up to `Assignment` (13), and the  opening part and the closing part of the operator serve as a pair of parentheses. In order to achieve this, the opening part of the operator must be a sequence of any horizontally-mirrorable characters (for a list of such characters, see [List of Mirrored Characters](https://www.compart.com/en/unicode/mirrored)), and the closing part must use the mirrored character sequence. For instances:
```altro
class test
{
    prefix func --> () <--;
    prefix func ⇒ () ⇐;
}
ctor()
{
  t := test();
  --> t <--;
  ⇒ t ⇐;
}
```

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
