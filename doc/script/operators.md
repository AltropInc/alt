# Builtin Operators

Altscript provides a complete collection of built-in operators across all of its primitive
types in numeric, enum, enum set and character.

## Operators

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

