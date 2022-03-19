# String

A string is finite sequence of characters in [UTF-8](https://en.wikipedia.org/wiki/UTF-8) encoding. Because UTF-8 is a variable-width
character encoding, a string is a partial indexed character container where the byte index does not always map to a valid character.
The value of a single character is a 32-bit primitive value type  which can be converted to a numeric value representing a
[Unicode code point](https://en.wikipedia.org/wiki/Code_point). Therefore, the representation of single character is different from
the character representation within a string.

Strings are immutable, that is, the value of a string cannot be changed.

## String Literal

String literals are delimited by double quotes or triple double quotes:
```altscript
"Hello World or Καλημέρα κόσμε or こんにちは 世界"
"""Hello World or Καλημέρα κόσμε or こんにちは 世界"""
```
A long string can be broken up by string literal concatenation:
```altscript
"Hello World or "
"Καλημέρα κόσμε or "
"こんにちは 世界"
```
is equivalent to
```altscript
"Hello World or Καλημέρα κόσμε or こんにちは 世界"
```
New lines can be contained in a string literal:
```altscript
"Hello World or 
Καλημέρα κόσμε or "
こんにちは 世界"
```
is equivalent to
```altscript
"Hello World or \nΚαλημέρα κόσμε or \nこんにちは 世界"
```
## Escape Character

An escape character is a character that invokes an alternative interpretation on the following characters in a string. The \ (backslash)
is used as an escape character for the following cases:
```altscript
\'          single quote
\"          double quote
\\          backslash
\n          new line
\r          carriage return
\t          tab
\b          backspace
\f          form feed
\v          vertical tab (If cross-browser compatibility is a concern, use \x0B instead of \v.)
\0          null character (U+0000 NULL) (only if the next character is not a decimal digit; else it is an octal escape sequence)
\xFF        character represented by the hexadecimal byte "FF"
\xFFFFFFFF  character represented by the hexadecimal byte "FFFFFFFF"
```





