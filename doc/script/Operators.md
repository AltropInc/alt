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
Here the operator sequence starts with the character '=' and terminates with the character '-' because operator name "=-" is not allowed. As result, we get lexical elements `pi`, `=`, `-`, and `3.14`.

An operator can be used as a function name just like an [identifier](Identifier.md). For examples:
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

| Op     | unicode  | Name             | category      | precedence    | associativity    | is lvaue operator | returns lvalue  |
|:------ |:-------- |:---------------- |:------------- |:------------- |:--------------   | ----------------- | --------------|
| []     |          | subscript        | access        | Postfix    | left             | false             | true          |
| .      |          | selection        | access        | Postfix    | left             | false             | true         |
| ==     |          | equal            | equality      | Relation   | left             | false             | false         |
| !=     | ≠        | inequal          | equality      | Relation   | left             | false             | false         |
| >      |          | greater          | comparible    | Relation   | left             | false             | false         |
| <      |          | less             | comparible    | Relation   | left             | false             | false         |
| >=     | ≥        | greater or equal | comparible    | Relation   | left             | false             | false         |
| <=     | ≤        | less or equal    | comparible    | Relation   | left             | false             | false         |
| ..     | ‥        | between          | comparible    | Addition   | left             | false             | false         |
| +      |          | add              | addable       | Addition   | left             | false             | false         |
| -      |          | substract        | addable       | Addition   | left             | false             | false         |
| *      |          | multiply         | scalable      | Scaling    | left             | false             | false         |
| /      |          | divide           | scalable      | Scaling    | left             | false             | false         |
| %      |          | modulo           | scalable      | Scaling    | left             | false             | false         |
| ^      |          | exponent         | scalable      | Exponent   | left            | false             | false         |
| /#     | ÷        | int divide       | scalable      | Scaling    | left             | false             | false         |
| +      |          | positive         | addable       | Prefix     | right            | false             | false         |
| -      |          | negative         | addable       | Prefix     | right            | false             | false         |
|        | ²        | exponent 2       | scalable      | Postfix    | left            | false             | false         |
|        | ³        | exponent 3       | scalable      | Postfix    | left            | false             | false         |
|        | √        | square root      | scalable      | Prefix     | right            | false             | false         |
|        | ∛        | cubic root       | scalable      | Prefix     | right            | false             | false         |
| ++     |          | increment        | Incrementable | Prefix     | right           | true             | true         |
| --     |          | decrement        | Incrementable | Prefix     | right           | true             | true         |
| ++     |          | post increment   | Incrementable | Postfix    | left            | true             | false         |
| --     |          | post decrement   | Incrementable | Postfix    | left            | true             | false         |
| ~      | ≅, ∈    | regex equal      | reg-equality  | Relation    | left             | false             | false         |
| !~     | ≇ ∉     | regex inequal    | reg-equality  | Relation    | left             | false             | false         |
| !      |          | logical not      | logical       | Prefix      | right            | false             | false         |
| \|\|     | ⋁       | logical or        | logical       | Logic-or  | left             | false             | false         |
| &&     | ⋀       | logical and       | logical       | Logic-and   | left             | false             | false         |
| \\      | ⊻       | exclusive or      | clusivity     | Addtion    | left             | false             | false         |
| \|      | ∪       | inclusive or      | clusivity     | Addtion    | left             | false             | false         |
| &      | ∩       | conjunction       | clusivity     | Scaling     | left             | false             | false         |
| ~      |          | flip             | clusivity     | Prefix      | right            | false             | false         |
| <<     |          | left shift       | shiftable     | Shift       | left             | false             | false         |
| >>     |          | right shift      | shiftable     | Shift       | left             | false             | false         |
| .?     |          | contain          | containment   | Condition   | left             | false             | false         |
| ?:     |          | conditional      | access        | Condition   | right            | false             | false         |
| =      |          | assign           | access        | Assignment  | right            | true             | true         |
| <-      |          | test assign     | access        | Assignment  | right            | true             | true         |
| ->      |          | tag assign     | access        | Assignment  | right            | true             | true         |
| @=      |          | copy assign     | access        | Assignment  | right            | true             | true         |
| @@=     |          | deep copy assign  | access        | Assignment  | right            | true             | true         |
| +=     |          | add assign       | addable       | Assignment  | right            | true             | true         |
| -=     |          | sub assign       | addable       | Assignment  | right            | true             | true         |
| \*=     |          | mul assign       | scalable      | Assignment | right            | true             | true         |
| /=     |          | div assign       | scalable      | Assignment  | right            | true             | true         |
| %=     |          | mod assign       | scalable      | Assignment  | right            | true             | true         |
| /#=    | ÷=       | int div assign   | scalable      | Assignment  | right            | true             | true         |
| ^=     |          | exp assign       | scalable      | Assignment  | right            | true             | true         |
| \\=     | ⊻=       | excl-or assign   | clusivity     | Assignment  | right            | true             | true         |
| \|=     | ∪=       | incl-or assign   | clusivity     | Assignment  | right            | true             | true         |
| &=     | ∩=       | conjunct assign  | clusivity     | Assignment  | right            | true             | true         |
| >>=    |          | L-hift assign    | shiftable     | Assignment  | right            | true             | true         |
| <<=    |          | R-hift assign    | shiftable     | Assignment  | right            | true             | true         |

Note that `.` has special meaning and cannot be used as an operator character in a function name. They are listed here just for lexcial processing purpose on [expressions](Expressions.md).

## Operator Precedence
==, !=, <, >, >=, <=, , !,
Operator precedence, also referred as [order of operations](https://en.wikipedia.org/wiki/Order_of_operations), determines which operations to perform first in order to evaluate a given expression. Operators with higher precedence become the operands of operators with lower precedence. For example, multiplication is granted a higher precedence than addition. Thus, the expression 3 + 4 * 2 is interpreted as 3 + (4 × 2), not (3 + 4) * 2. Each built-in operator is assigned with a precedence number. The smaller number indicates the higher precedence.

| Precedence Group     | Order  |
|:-------------------- |:------ |
| Postfix              | 1      |
| Prefix               | 2      |
| Exponent             | 3      |
| Scaling              | 4      |
| Addition             | 5      |
| Shift                | 6      |
| Relation             | 7      |
| Logical And          | 8      |
| Logical Or           | 9     |
| Condition            | 10     |
| User Defined         | 11     |
| Assignment           | 12     |
| Sequencing           | 13     |

All user defined non-prefix operators has the precedence number 12.

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
value class double
{
    prefix func √();           // Okay
    prefix func sin();         // Okay
    prefix func √(n: double);  // Error: Prefix interface should not have input
}
```
In the expression to call a prefix operator, `operator operand`, the operand is actually the owner instance to execute the operator, whihc is equivalent to a member function call `owner.member_function_name`, where the `operand` is the `owner`, and the `operator` is the `member_function_name`.

All prefix operators have the `Prefix` precedence (2). Therefore, when we apply a prefix operator to an expression that contains operators in lower precedence, we have to use parentheses. For tinstance, the expression `sin x + y` means `sin(x) + y`. If you actually need the `sin` value of the sum, you need to write `sin (x+y)`.
The root symbol √ is traditionally prolongated by a bar (vinculum) over the expression, but we cannot make this notation in Altro lexical. If we use the root symbol √ to define a prefix operator, we have to use parentheses in order to get the desired result:
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
    class foo
    {
        prefix func --> () <-- : foo { owner }  // returns the owner object, i.e. this instance of foo
        prefix func ⇒ () ⇐ { /* do something here */ }
    }
    ctor()
    {
      ⇒-->foo()<--⇐;
    }
}
```
Note that `⇒` and `⇐` are separators, the sequence `⇒-->` contains two operators, one is the operator for function call to `⇒()⇐`, and another is the operator for function call to `-->()<--`. Because the prefix operator is right associative, the inner expression `-->foo()<--` is executed first, which returns an object of `foo`, and then the function `⇒()⇐` is applied on the returned object from the function `-->()<--`. This is illustrated as below:
```altro
⇒(-->foo()<--)⇐
```

## Lvalue Operators

An lvalue operator is an operator that requires a modifiable [lvalue](https://en.wikipedia.org/wiki/Value_(computer_science)#lrvalue) for its operand (its owner or executor if we view the operator as a member funcion). An lvalue, also referred as a locator value, represents an instance that occupies some identifiable location in memory where its value or state can be changed. All assignment operators are lvalue operators. Increment operator `++` and decrement operator `--` are also lvalue operators. Using an lvalue operator on a non-modifiable value is not allowed.

Any of the following expressions can be lvalue expressions:
* A variable name
* A subscript expression with a variable name
* A member-selection expression with a variable name
* A function output that is a reference
* An lvalue in parentheses

Consider the following examples:
```altro
x := ++3;   // Error: Cannot alter the value of a constant value
3 = x;      // Error: Cannot alter the value of a constant value
func bar():int;
bar() = x;  // Error: An lvalue expression is expected for operator: =
x++ ++      // Error: An lvalue expression is expected for operator: ++
++x++;      // Error: An lvalue expression is expected for operator: ++. Note that postfix operator has higher precedence.
(++x)++;    // Okay, because prefix increment returns an lvalue
```
Note that post increment `x++` returns a integer value, not a reference. Therefore, the lvalue operator `++` cannot be applied to the expression `x++`. However, the prefix increment `++x` returns a reference to the value contained in `x`, thus the lvalue operator `++` can be applied to the expression `++x`.



