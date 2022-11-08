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

The type `char` is a 32-bit primitive type based on the [UTF-32 encoding](https://en.wikipedia.org/wiki/UTF-32). Although the type `char` provides some arithmetic-like operations to obtain another character in distance to this character, unlike C and Java, the type `char` is not a numeric type and a character value cannot be automatically converted to a numeric value used in all arithmetic operations. Therefore, the type `char` is not a subtype of `numeric`, and the operations such as `+` and `-` do not override the methods defined in the `numeric` interface. To convert a `char` value to an unsigned integer that represents the mapped internal numerical value (the [code point](https://en.wikipedia.org/wiki/Code_point)), use the method `code` provided in the type `char`. The type `char` is defined as below:
```altro
sealed value class char is character
{
    final func <=> (other: char): int;  // implements deferred methods in the comparable interface
    final func code(): uint;            // return UTF-32 code in numeric value
    final func is_valid(): bool;        // return if UTF-32 code is a valid character
    func + (offset: uint): char;        // return a character whose code point is greater by the given offset
    func - (offset: uint): char;        // return a character whose code point is smaller by the given offset
    func += (offset: uint): char;       // augmented assignment that is equivalent to { self = self + offset; self }
    func -= (offset: uint): char;       // augmented assignment that is equivalent to { self = self - offset; self }
    func ++ (): char;                   // postfix increment that is equivalent to { self += 1; self }
    func -- (): char;                   // postfix decrement that is equivalent to { self -= 1; self }
    prefix func ++ (): char;            // prefix increment that is equivalent to { temp:=self; self += 1; temp }
    prefix func -- (): char;            // prefix increment that is equivalent to { temp:=self; self -= 1; temp }
    ctor(code: uint);                   // constructor from UTF-32 code in numeric value
}
```

## Character Literal

A character literal contains a character in single quotation mark symbols, for example 'c', '1'，'字'，'✅', and etc. A character literal can also be an escape sequence enclosed in single quotation mark symbols. An escape sequence starts with a blackslash (\) with a number of ordinary characters for an alternative interpretation of a single special character (an escape character). The escape sequence is used as an escape character for the following cases:
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
\0                null character (only if the next character is not a decimal digit; Otherwise it is an octal escape sequence
\xFF              one character byte represented by the hexadecimal byte "FF" (Validness is unchecked. If this is a concern, use \uFFFF instead of \xFF)
\xFF\xFF          two character bytes in UTF-8 Encoding (Validness is checked)
\xFF\xFF\xFF      three character bytes in UTF-8 Encoding (Validness is checked)
\xFF\xFF\xFF\xFF  four character bytes in UTF-8 Encoding (Validness is checked)
\uFFFF            one unicode character in UTF-16 Encoding represented by the hexadecimal "FFFF" (Validness is checked)
\UFFFFFFFF        one unicode character in UTF-32 Encoding represented by the hexadecimal "FFFFFFFF" (Validness is checked)
\#00000;          one unicode character represented by the decimal "00000" (Validness is checked)
\000              one unicode character represented by the octal number "000" (Validness is checked)
```
Example code:
```altscript
'\u2705'                    // character '✅' in UTF-16 encoding
'\U0001F174'                // character '🅴' in UTF-32 encoding
'\xE4\xB8\x96'              // character '世' in UTF-8 encoding
'\#19990;'                  // character '世' in UTF-32 decimal encoding
```


