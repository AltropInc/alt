# Character

Characters are smallest symbol units in a writing system that roughly correspond to grapheme-like symbols such as alphabets, syllabaries, numerical digits, punctuation marks, white spaces, icons, emojis, as well as control units that do not correspond to visible symbols but rather to text processing instructions. Altro has an abstract character type `character` for representing a single character. The type `character` is derived from the interface [`comparable`](TypeComparable.md):
```altro
interface class character is comparable
{
    virtual func code(): uint;      // return UTF-32 code in numeric value
    virtual func is_valid(): bool;  // return if internal representation is a valid character
}
```
Altro has two built-in subtypes derived from `character`, `char` and `uft8`.

## Type `char`

The type `char` is a 32-bit primitive type based on the [UTF-32 encoding](https://en.wikipedia.org/wiki/UTF-32). Although the type `char` provides some arithmetic-like operations to obtain another character in distance to this character, unlike C or Java, the type `char` is not a numeric type and a character value can not be automatically converted to a numeric value used in all arithmetic operations. Therefore, the type `char` is not a subtype of `numeric`, and the operations such as `+` and `-` do not override the methods defined in the `numeric` interface. To convert a `char` value to an unsigned integer that represents the mapped internal numerical value (the [code point](https://en.wikipedia.org/wiki/Code_point)), use the method `code` provided in the type `char`. The type `char` is defined as below:
```altro
sealed value class char is character
{
    final func <=> (other: char): int;  // implements deferred methods in the comparable interface
    final func code(): uint;            // returns UTF-32 code in numeric value
    final func is_valid(): bool;        // returns if UTF-32 code is a valid character
    func + (offset: uint): char;        // returns a character whose code point is greater by the given offset
    func - (offset: uint): char;        // returns a character whose code point is smaller by the given offset
    func += (offset: uint): char;       // augmented assignment that is equivalent to { self = self + offset; self }
    func -= (offset: uint): char;       // augmented assignment that is equivalent to { self = self - offset; self }
    func ++ (): char;                   // postfix increment that is equivalent to { self += 1; self }
    func -- (): char;                   // postfix decrement that is equivalent to { self -= 1; self }
    prefix func ++ (): char;            // prefix increment that is equivalent to { temp:=self; self += 1; temp }
    prefix func -- (): char;            // prefix increment that is equivalent to { temp:=self; self -= 1; temp }
    ctor(code: uint);                   // constructor from UTF-32 code in numeric value
}
```

## Type `utf8`

The primitive type `utf8` is a variable-length character type based on the [UTF-8 encoding](https://en.wikipedia.org/wiki/UTF-8) that uses one to four bytes (8-bit). The type `utf8` is primarily used as the element type of [string](String.md). When it is used in declaration of a variable, the maximum storage (four bytes) is always allocated.
```altro
sealed value class char is character
{
    final func <=> (other: char): int;  // implements deferred methods in the comparable interface
    final func code(): uint;            // returns UTF-8 code in numeric value
    final func code_units(): uint;      // returns number of bytes required for this character
    final func is_valid(): bool;        // returns if UTF-8 code is a valid character
}
```
The character type `char` and `utf8` can be converted to each other automatically:
```altro
c1: utf8 = 'A';
c2: char = c1;
```
Because char and utf8 are interchangeable, a string and a stream of char are also interchangeable:
```altro
ss: char... = "Hello 世界";
s: string = ss;
```

## Character Literal

A character literal contains a character in single quotation mark symbols, for example 'c', '1'，'字'，'😀', and etc. A character literal can also be an escape sequence enclosed in single quotation mark symbols. An escape sequence starts with a backslash (\) with a number of ordinary characters for an alternative interpretation of a single special character (an escape character). The escape sequence is used as an escape character for the following cases:
```altscript
\'                single quote
\"                double quote
\\                backslash
\n                new line
\r                carriage return
\t                tab
\b                backspace
\f                form feed
\v                vertical tab (If cross-browser compatibility is a concern, use \x0B instead of \v.)
\0                null character (only if the next character is not an octal digit; Otherwise it is an octal escape sequence
\o...             one character byte represented by at least one octal digits (validity is unchecked, but overflow is checked)
\xhh              one unicode character in UTF-8 encoding by a pair of hexadecimal digits (validity is checked)
\xhh\xhh          one unicode character in UTF-8 encoding by 2 pairs of hexadecimal digits (validity is checked)
\xhh\xhh\xhh      one unicode character in UTF-8 encoding by 3 pairs of hexadecimal digits (validity is checked)
\xhh\xhh\xhh\xhh  one unicode character in UTF-8 encoding by 4 pairs of hexadecimal digits (validity is checked)
\uhhhh            one unicode character in UTF-16 encoding represented by 4 hexadecimal digits(validity is checked)
\U00hhhhhh        one unicode character in UTF-32 encoding represented by 6 hexadecimal digits (validity is checked)
\#d...            one unicode character represented by decimal digits (Validness is checked)
```
Example code:
```altscript
'\0'              // a null character
'\#126'           // a null character
'\u2705'          // character '✅' in UTF-16 encoding
'\U0001F600'      // character '😀' in UTF-32 encoding
'\xE4\xB8\x96'    // character '世' in UTF-8 encoding
'\#19990;'        // character '世' in UTF-32 decimal encoding
```
Note that the octal-digit sequence `\o...` may generate invalid unicode characters because the validity of the unicode code point is not checked. It is here just for the compatibility to [C-like escape sequence](https://en.wikipedia.org/wiki/Escape_sequences_in_C), and it is at your own risk to use it. Unlike the C escape sequence, you can use more than 3 octal digits in the escape sequence as long as the number is within the boundary of a 32-bit unsigned integer. The digits '8' and '9' cannot be used in the octal-digit sequence.

The escape sequence '\uhhhh' cannot represent a unicode with code point greater than '\uFFFF'. Use the escape sequence `\U00hhhhhh` for those unicodes beyond the limit of '\uhhhh'

Unlike the escape sequence in a (string)(TypeString.md), the escape sequence '\#d...' does not require a semicolon at the end because only one character is expected here.

Except for the octal-digit sequence, all other escape sequences will report errors for an invalid unicode. For examples:
```altscript
'\xE4\xB8\x96'        // character '世' in UTF-8 encoding
'\xE4\xB8\x96\x32'    // Error: Too many bytes in UTF-8 character encoding
'\xE4\xB8'            // Error:  Missing byte(s) in UTF-8 character encoding
'\U0011FFFF'          // Error: Invalid unicode
```
The type of a character literal is automatically inferred as `char`.


