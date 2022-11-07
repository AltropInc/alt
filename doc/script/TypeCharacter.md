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
