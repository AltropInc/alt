# String

A string is finite sequence of characters in [UTF-8](https://en.wikipedia.org/wiki/UTF-8) encoding. Because UTF-8 is a variable-width
character encoding, a string is a partial indexed character container where the byte index does not always map to a valid character.
The value of a single character is a 32-bit primitive value type  which can be converted to a numeric value representing a
[Unicode code point](https://en.wikipedia.org/wiki/Code_point). Therefore, the representation of single character is different from
the character representation within a string.

All strings are instances (objects) of the `string` type.

Strings are immutable, that is, the value of a string cannot be changed once it is created. The string type has a number of methods
that appear to modify strings, but what these methods really do is create and return a new string as the result of the
operation.

For mutable sequence of characters, use stream of characters, i.e., char..., which is a variable-length character sequence that can be
edited. However, the element of the stream is a fixed-width character using [UTF-32](https://en.wikipedia.org/wiki/UTF-32#:~:text=UTF-32) encoding.

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
Καλημέρα κόσμε or 
こんにちは 世界"
```
is equivalent to
```altscript
"Hello World or \nΚαλημέρα κόσμε or \nこんにちは 世界"
```
where \n is an escape character that stands for a new line (see below).

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
\0          null character (only if the next character is not a decimal digit; else it is an octal escape sequence)
\xFF        one character byte represented by the hexadecimal byte "FF" (Validness is unchecked. If this is a concern, use \uFFFF instead of \xFF)
\uFFFF      one unicode character in UTF-16 Encoding represented by the hexadecimal "FFFF" (Validness is checked)
\UFFFFFFFF  one unicode character in UTF-32 Encoding represented by the hexadecimal "FFFFFFFF" (Validness is checked)
\#00000;    one unicode character represented by the decimal "00000" (Validness is checked)
```
Example code:
```altscript
"\u4E16\u754C"                // UTF-16 Encoding
"\U00004E16\U0000754C"        // UTF-32 Encoding
"\xE4\xB8\x96\xE7\x95\x8C"    // UTF-8 Encoding, the actual byte values stored in the string
"\#19990;\#30028;"            // UTF-32 decimal Encoding
```
All of the above strings represents the same string:
```altscript
"世界"
```

## Creating Strings

A direct way to create a string is to write the string in string literal and assign the literal to a name (variable) so that it can be accessed later:
```altscript
greeting := "Hello World!"
```
In this case, "Hello world!" is a string literal. Whenever it encounters a string literal in your code, a string object is created with its value — in this case, `Hello world!`. The name `greeting` is used to refer to the string object `Hello world!`. Whenever the name `greeting` presents, it stands for the value
`Hello world!`.

A string object can also be created by an function output or by automatic conversion from an object in a compatable data structure:
```altscript
char_pair := ('世', '界');
greeting :string = char_pair;
```
In this case, the type `char_pair` is deducted to a tuple `(char; char)`. Since the name `greeting` is declared to be in `string` type, when the tuple value
of `char_pair` is assigned to `greeting`, a new string object is created for the name `greeting`.

## Common String Operations

### Comparison

Strings are compariable objects and standard comparison operators (=, ≠, <, ≤, >, ≥) can be applied to them. For instance
```altscript
"abstract" ≥ "concrete"    // the result is false. Note, operator ≥ is same as >=
```
In addtion, the `approximately-equal-to` operator ≅ (or ~) can be used for regular expression match:
```altscript
"foo.txt" ≅ "([a-z]+)\\.([a-z]+)"    // the result is true. We will discuss string regular expression later
```

### Concatenation

String concatenation means adding strings together. The string concatenation operator is `+`:
```altscript
greet := "Hello";
whom := "world";
greet_whom := greet + ", " + whom + "!.\n";
```
The result in `greet_whom` is "Hello, world!".

### Iteraction

You can iterate or loop over all the characters of string in forward, backward direction using `foreach`:
```altscript
greet_世界 := "Hello, 世界!";
foreach (ch in greet_世界) { /* do something on each character stored in ch */ }
foreach backward (ch in greet_世界) { /* do something backwards on each character stored in ch */ } 
```
Since strings are not fully indexed containers, you cannot simply loop over characters in a string through a byte index. Instead, you use
string iterator:
```altscript
for (ch:=greet_世界.begin(); ch.is_valid(); ch.next())
{
    print(ch);
}
```
Or use `rbegin` to loop over backwards:
```altscript
for (ch:=greet_世界.rbegin(); ch.is_valid(); ch.next())
{
    print(ch);
}
```
Here `ch` is a string iterator and it is automatically covereted to the value of `char` whenever it is needed. Since strings are immutable,
you cannot use string iterator to change the value in the string.

To loop over characters in a string through a byte index, you need to make sure that the index is at the starting position of a character
in the string:
```altscript
greet_世界 := "Hello, 世界!";
for ( i:=0;
      i < greet_世界.length();
      i = greet_世界.next_char_pos(i))
{
    print(greet_世界.char_at(i));
}
```
`char_at` returns a null character if the position at i is not the start of a valid character.

### String Methods

Here is the list of all string methods:

* `func + (ch: char; repeat: int=1): string` --
    creates a new string by concatenation of this string with a number of given characters. `ch` is the given character and 'repeat'
    is the repeat number with default set to 1.
```altscript
greet_world := "Hello, world" + '!';     // "Hello, world!"
greet_world2 := greet_world + ('!', 2);  // "Hello, world!!!"
```
* `func + (str: string): string` --
    creates a new string by concatenation of this string with another string
```altscript
greet_world := "Hello" + ',' +  "world";     // "Hello, world!"
```
* `func length (): int` --
    returns byte length (number of bytes) used to store the string
```altscript
"Hello, 世界!".length();     // returns 14
```
* `func begin (): iterator` --
    returns the iterator points to the first character of the string. If the string is empty, it returns an invalid iterator.
```altscript
"Hello, 世界".begin();     // returns an iterator that points to the first character 'H'
```
* `func rbegin (): iterator` --
    returns the iterator points to the last character of the string. If the string is empty, it returns an invalid iterator.
```altscript
"Hello, 世界".rbegin();     // returns a backwards iterator that points to the last character '界'
```
* `func char_at (index: int): char` --
    returns the character at the byte index position. If the character at the given index is not a valid character, it returns a null character. 
```altscript
"世界".char_at(3);     // returns the character '界'. Note, both '世' and '界' have 3 bytes in the string
"世界".char_at(4);     // returns null character, becuase the byte at index 4 is not a valid leading byte of a character
```
* `func next_char_pos (index: int): int` --
    returns the index of the next character starting from the current position given by `index`. If there is no valid character after the given position,
    it returns the length of the string (an index that points to the end of the string). The given current index is not necessarily at the valid
    character position.
```altscript
"世界".next_char_pos(0);     // returns 3, which is the positon of '界'
"世界".next_char_pos(1);     // returns 3, which is the positon of '界'
```
* `func prev_char_pos (index: int): int` --
    returns the index of the previous character starting from the current position given by `index`. If there is no valid character prior to the given position,
    it returns -1. The given current index is not necessarily at the valid character position.
```altscript
"世界".prev_char_pos(3);     // returns 0, which is the positon of '世'
"世界".prev_char_pos("世界".length());     // returns 3, which is the positon of '界'
```
* `func at (index: int): utiny` --
    returns the byte value (in unsigned tiny integer) at the given index position.
```altscript
"世界".at(4);     // returns 0x95 which is the second byte in the encoding of '界' (\xE7\x95\x8C)
```
* `func front (): utiny` --
    returns the first byte value of the string.
* `func back (): utiny` --
    returns the last byte value of the string.
```altscript
"世界".back();     // returns 0x8C which is the last byte in the encoding of '界' (\xE7\x95\x8C)
"世界".front();    // returns 0xE4 which is the first byte in the encoding of '世' (\xE4\xB8\x96)
```

