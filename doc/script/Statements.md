# Statements

A statement is a syntactic unit that expresses some action to be carried out. A block of code for a [routine](Routine.md) body is formed by a sequence of one or more statements, possibly mixed with some name declarations. A statement may contain other statements to form a compound statement.

Altro has following statements:

*expression-statements*<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*expression*](Expressions.md)<br>
[*jump-statements*](StatementsJump.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*break-statement*](StatementsJump.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*continue-statement*](StatementsJump.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*fallthrough-statement*](StatementsJump.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*return-statement*](StatementsJump.md)<br>
[*compound-statements*](StatementCompound.md)<br>
*branch-statements*<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*if-statement*](StatementIf.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*switch-statement*](StatementSwitch.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*match-statement*](StatementMatch.md)<br>
[*loop-statements*](StatementsLoop.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*while-statement*](StatementsLoop.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*repeat-statement*](StatementsLoop.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*for-statement*](StatementsLoop.md)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[*foreach-statement*](StatementsLoop.md)<br>

A semicolon is required to separate two statements if the previous statement is not ended with a right curly bracket.
