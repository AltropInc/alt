# String Parser

The class `parser` is a string parser used to extract these data chunks from a string or a text file. The class `parser` is defined in the `sys` class as below:
```altro
data class paser
{
    parser (str: string; is_file: bool=false);
    func getc(): char;
    func geti(base: int): long;
    func getn(): double;
    func gets(): string;
    func getw(): string;
    func getl(): string;
    func skipc(num: int=1);
    func skipl(): bool;
    func skipws();
    func peer(): char;
    func peernext(): char;
    func scan(to: char): string;
    func scan(to: string): string;
    func scanto(to: char...): string;
    func pos(): (int; int);
    func done(): bool;
    func is_valid(): bool; 
}
```
The constructor of the parser, `parser (str: string; is_file: bool=false)`, takes a string to be parsed, or a text file name given by the `str` input if `is_file` is set to true. The member functions of `parser` are explained below.
* **func getc(): char;** It returns the character at the current position and advances the position to the next character.
* **func geti(base: int=0): long;** It returns an integer value at the current position and advances the position to the next character after the string that represents the parsed integer value. The `base` indicates the base of the number. It can be one of 0, 2, 8, 10 , and 16. If the `base` is zero, it reads the integer value according to the base characters at the beginning of the integer value, i.e., `0x` for base 16, `0` for base 8, and `0b` for base 2. If the current position is not an integer value, it returns zero and the position is unchanged if the current position is a non-space character, otherwise, the position is advanced to the first non-space character.
* **func getn(): double;** It returns a floating-point number value at the current position and advances the position to the next character after the string that represents the number. If the current position is not an integer value, it returns zero and the position is unchanged if the current position is a non-space character, otherwise, the position is advanced to the first non-space character.
* **func gets(): string;** It returns a string in quotation marks at the current position and advances the position to the next character after the string that represents the quotation. If the current position is not a quotation, it returns a null string and the position is unchanged if the current position is a non-space character, otherwise, the position is advanced to the first non-space character.
* **func getw(): string;** It returns a string of a word at the current position and advances the position to the next character after the string that represents the word. A word is either a quotation or a character sequence terminated by a white space or a [operator character](Operators.md). If the current position is not a word, it returns a null string and the position is unchanged if the current position is a non-space character, otherwise, the position is advanced to the first non-space character.
* **func getl(): string;** It returns a string with characters from the current position to the end of the current line and advances the position to the next line.
* **func skipc(num: int=1);** It skips a number of characters. The `num` parameter gives the number of characters (not number of bytes) to be skipped. The position is advanced to the first character after the skipped character(s).
* **func skipl(): bool;** It skips the current line and advances the position to the next line. It returns false if there is no line to skip.
* **func skipws();** It skips a white spaces. The position is advanced to the next non-space character.
* **func peer(): char;** It returns the character at the current position and the current position is unchanged.
* **func scan(to: char): string** It returns a string that contains characters from the current position to the character before the first occurance of the character given by the parameter `to`. The current position is advanced to the next character after the first occurance of the given character.
* **func scan(to: string): string** It returns a string that contains characters from the current position to the character before the first occurance of the string given by the parameter `to`. The current position is advanced to the next character after the first occurance of the given string.
* **func scanto(to: char...): string** It returns a string that contains characters from the current position to the character before the first occurance of the character listed in the character stream given by the parameter `to`. The current position is advanced to the first occurance of the character listed in the character stream.
* **func pos(): (int; int)** Returns the current position. The first integer in the returned integer pair is the line number, and the second integer is the column position.
* **func done(): bool** Returns true if the parser comes to the end.
* **func is_valid(): bool** Returns true if the parser contains a valid string buffer to parse.

## Examples

When any scan function is called, all leading and trailing white spaces, if any, will be removed, unless you set the parameter `rm_ws` to false.
Also note the difference of the advanced current position in calling `scan` and `scanto`. When `scan` is called, the current position is moved to the next character after the first occurance of the given character. For example:
```
receipt :=
    "Large Eggs       $3.49
     Milk             $3.15
     Cottage Cheese   $2.59
     Chicken Breast   $6.89";
shopping_list: (product: string; price: double)...;
p := parser(receipt);
shopping_list += (p.scan('$'), p.getn());
shopping_list += (p.scan('$'), p.getn());
shopping_list += (p.scan('$'), p.getn());
shopping_list += (p.scan('$'), p.getn());
print(shopping_list);
```
After `p.scan('$')` is called, the position is moved to the beginning of the number after the dollar sign. The output:
```
((Large Eggs,3.49),(Milk,3.15),(Cottage Cheese,2.59),(Chicken Breast,6.89))
```
When `scanto` is called, the current position is moved to the first occurance of the character in the given character list. For example:
```
receipt :=
    "Large Eggs       $3.49
     Milk             £3.15
     Cottage Cheese   €2.59
     Chicken Breast   ¥689";
shopping_list: (product: string; price: double)...;
p := parser(receipt);
shopping_list += (p.scanto(('£','$','€','¥')), p.getc(), p.getn());
shopping_list += (p.scanto(('£','$','€','¥')), p.getc(), p.getn());
shopping_list += (p.scanto(('£','$','€','¥')), p.getc(), p.getn());
shopping_list += (p.scanto(('£','$','€','¥')), p.getc(), p.getn());
print(shopping_list);
```
After `p.scanto(('£','$','€','¥')` is called, the position is moved to the character of the currency sign. The output:
```
((Large Eggs,$,3.49),(Milk,£,3.15),(Cottage Cheese,€,2.59),(Chicken Breast,¥,689))
```

