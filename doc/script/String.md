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
Note, in triple double quoted string, all consecutive double quotes after the leading triple double quotes and all consecutive double quotes
before the ending triple double quotes are considered part of the string:
```altscript
""""Hello World" or "Καλημέρα κόσμε" or "こんにちは 世界""""
```
A long string can be broken up by string literal concatenation:
```altscript
"Hello World or "
"Καλημέρα κόσμε or "
"こんにちは 世界"
```
The above is equivalent to
```altscript
"Hello World or Καλημέρα κόσμε or こんにちは 世界"
```
New lines can be contained in a string literal:
```altscript
"Hello World or 
Καλημέρα κόσμε or 
こんにちは 世界"
```
This is equivalent to
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
```altro
"\u4E16\u754C"                // UTF-16 Encoding
"\U00004E16\U0000754C"        // UTF-32 Encoding
"\xE4\xB8\x96\xE7\x95\x8C"    // UTF-8 Encoding, the actual byte values stored in the string
"\#19990;\#30028;"            // UTF-32 decimal Encoding
```
All of the above strings represents the same string:
```altscript
"世界"
```

## Raw String

You can create a raw string by prefixing a string literal with r. A raw string treats the backslash character (\) as a literal character, not an escape sequence start, except for the case `\"` where the double quotation mark `"` can still be escaped with a single backslash character `\`. Raw string is useful when a string needs to contain a backslash, such as for a regular expression where you don’t want it to be treated as an escape character. Take note that in a regular string format where backslash `\` is used for escape sequences, e.g., `\n` for newline, `\t` for tab, and you also need to write `\\` for `\`. Consequently, to write regex pattern `\\` (which matches one `\`) in a regular string format, you need to write `\\\\` (two levels of escape). Similarly, you need to write `\\d` for regex metacharacter `\d`. This is cumbersome and error-prone. And here the raw string format will help to resolve this mess.

In addition, because the double quotation mark `"` can still be escaped with a single backslash character `\`,  you can’t create a raw string that ends with a single backslash character, so `r"abcd\"` is considered an invalid string without the ending quotation mark. Examples of raw strings:
```altro
r"[a-z]+\.txt"
r"quoted \"word\" here"
r"C:\Windows\System32"
```

## Locale-Sensitive String

Locale-sensitive strings are strings to be displayed in different text according to a specific language and region. Locale-sensitive string are started with an underscore, for example,
```
_"Fri"
```
is a short form Friday and it is displayed as `Fri` in English, `周五`  in Chinese China, and 週五` in Chinese Taiwan. When we print the string, it will be translated to the text in the current locale setting. For example:
```altro
setlanguage("中文", "台灣");
print("In Taiwan, the short form of Friday is ",  _"Fri", ", and the long form is ",  _"Friday", ".\n");
```
The output of the print is:
```
In Taiwan, the short form of Friday is 週五, and the long form is 星期五.
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
In this case, the type of the name `char_pair` is deducted to a tuple `(char; char)`. Since the name `greeting` is declared to be in `string` type, when the tuple value of `char_pair` is assigned to `greeting`, a new string object is created for the name `greeting` by converting the value of string pair.

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
Here `ch` is a string iterator and it is automatically converted to the value of `char` whenever it is needed. Since strings are immutable,
you cannot use string iterator to change the value in the string.

To loop over characters in a string through a byte index, you need to make sure that the index is at the start position of a character
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

## String Methods

In addition to methods with common comparison operatiors < > >= <= == != ~=, string has the floowing methods:

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
* `func [(index: int)]: char`
* `func char_at (index: int): char` --
    returns the character at the given byte index position. The byte index can be in any position within a encoded byte sequence of a character.
    If the given index is invalid, it returns a null character. 
```altscript
"世界"[3];     // returns the character '界'. Note, both '世' and '界' have 3 bytes in the string
"世界"[4];     // returns the character '界'. Note, the byte at index 4 is at the second byte of '界'
"世界".char_at(2);  // returns the character '世'.
```
* `func byte_at (index: int): utiny` --
    returns the byte value (in unsigned tiny integer) at the given index position.
```altscript
"世界".byte_at(4);     // returns 0x95 which is the second byte in the encoding of '界' (\xE7\x95\x8C)
```
* `func [(start: int) .. (end: int)]: string`
* `func substring (start:int; end:int=-1): string` --
    returns a substring indexed from start to end. The character at the end position is not included. If the end position is given to -1,
    the end position is at the end of the string. Note, if the start or end position is in the middle of a character, it will be adjusted to the beginning
    character position before the substring is sliced.
```altscript
"世界"[0..5];   // returns "世". Note the end index 5 is in the middle of "界" and is adjusted to 4 pointing the first byte of '界'.
"世界".substring(3);   // returns "界".
```
* `func length (): int` --
    returns byte length (number of bytes) used to store the string
```altscript
"Hello, 世界!".length();     // returns 14
```
* `func is_basic (): bool` --
    returns true if the string does not contain any characters with multiple code units
```altscript
"Hello, World!".is_basic();    // returns true
"Hello, 世界!".is_basic();     // returns false
```
* `func count (): int` --
    returns number of characters contained in the string
* `func count (start: int; end: int =-1): int` --
    returns number of characters between the start and the end indices. If end is not given, the defualt is -1, meaning to the end of the string
```altscript
"Hello, 世界!".count();     // returns 10
"Hello, 世界!".count(7);    // returns 3 for 3 characters "世界!", counts from the 7th character
```

* `func width (): int` --
    returns number of colum positions required to display the string in a fixed width font
* `func width (start: int; end: int =-1): int` --
    returns number of colum positions required to display the string between the start and the end indices in a fixed width font . If end is not given, the defualt is -1, meaning to the end of the string
```altscript
"Hello, 世界!".width();     // returns 12, one Chinese character requires two colum positions
"Hello, 世界!".width(7);    // returns 5 for 3 characters "世界!"
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
* `func char_units_at (index: int): char` --
    returns the number of [code units](https://en.wikipedia.org/wiki/Character_encoding#Terminology) of the character at the byte index position. If the character at the given index is not a valid character, it returns 0. 
```altscript
"世界".char_units_at(3);     // returns 3. The character '界' have 3 code units (bytes) in the string
"世界".char_units_at(4);     // returns 0 because it is not a valid leading byte of a character at index 4
```
* `func next_char_pos (index: int; nth: int=1): int` --
    returns the index of the next nth character starting from the current position given by `index`. If there is no valid character after the given position,
    it returns the length of the string (an index that points to the end of the string). The given current index is not necessarily at the valid
    character position.
```altscript
"世界".next_char_pos(0)     // returns 3, which is the positon of '界'
"世界".next_char_pos(1)     // returns 3, which is the positon of '界'.
"Hello, 世界".next_char_pos(0,8)   // return 10 for the 8th characte,r which is '界' from the start position 0
```
* `func prev_char_pos (index: int; nth: int=1): int` --
    returns the index of the nth previous character starting from the current position given by `index`. If there is no valid character prior to the given position,
    it returns -1. The given current index is not necessarily at the valid character position.
```altscript
"世界".prev_char_pos(3);     // returns 0, which is the positon of '世'
"世界".prev_char_pos("世界".length());     // returns 3, which is the positon of '界'
"Hello, 世界".prev_char_pos("Hello, 世界".length(), 2)   // returns 7 for 2nd character from the end of the string, which is '世'
```
* `func front (): char` --
    returns the first character of the string.
* `func back (): char` --
    returns the last character of the string.
```altscript
"世界".back();     // returns '界'
"世界".front();    // returns '世'
```
* `func occurs (str: string; start: int=0): int` --
    returns the index position where this string occurs first time as a substring from the start index in the given string `str`. Returns -1 if it does not occur.
* `func find (str: string; start: int=0): int` --
    returns the index position where the given string `str` is found first time from the start index in this string. Returns -1 if it is not found.
```altscript
"Hello, 世界".find("世界");      // returns 7
"世界".occurs("Hello, 世界");    // returns 7
```
* `func rfind (str: string; end: int=-1): int` --
    returns the index position where given string `str` is found first time backwards from the end index. Returns -1 if it is not found. If the end index is not given or is given to -1, the search starts from the end of the string
```altscript
"Hello, Hello".rfind("Hello");         // returns 7
"Hello, Hello".rfind("Hello", 7);      // returns 0
```
* `func to_stream (): char...` --
    convert this string in a char stream
```altscript
"Hello, 世界".to_stream();     // returns a stream contains ('H', 'e', 'l', 'l', 'o', 'w', ',', ' ', '世', '界')
```
* `func starts_with (str: string): bool` --
    returns true if this string starts with the given string `str`.
* `func ends_with (str: string): bool` --
    returns true if this string ends with the given string `str`.
```altscript
"Hello, 世界".starts_with("Hello");   // returns true
"Hello, 世界".ends_with("世界");      // returns true
```
* `func compare (str:string; ignore_case:bool=false; number_as_whole:bool=false): int` --
    returns 0 if this string is equal to `str`, a negative value if this string is smaller than `str`, and a positive value if this string is greater that `str`.
    If `ignore_case` is true, do the caseless comparison. This does not support the case of comparsion involving multiple characters vs. single character
    such as between "ss" and “ß” in German, and in these cases they are considered not equal. If `number_as_whole` is true and a number starts at the same
    position of two strings, the compare will use the number as whole instead of comparing digits individually. For instance, "item(12)" is greater than "item(3)"
```altscript
"item(12)".compare("Item(3)", fasle, true);   // returns a positive integer, because "12" is greater than "3"
"item(12)".compare("Item(3)", fasle, fasle);  // returns a negative integer, because '1' is smaller than '3'
```
* `func split (separator:char=','; terminator:char='\0'; skip_leading_sp:bool=true; skip_trailing_sp:bool=true): string...` --
    returns a splitted string stream. `separator` is the character to divide substrings. `terminator` is character at which the split process stops.
    `skip_leading_sp` indicates whether the leading spaces of the substring should be skipped. and `skip_trailing_sp` indicates whether the trailing
    spaces of the substring should be skipped.
```altscript
"item(1), item(2), item(3)".split();   // returns a string stream ("item(1)", "item(2)", "item(3)")
```
* `func join (strings: string...; start:int=0; end:int=-1; separator:string=null; tail:string=null): string` --
    returns a merged string from the given string stream in the ranged of \[strat, end). 
    `separator` is the optional string to be inserted between two strings in the stream and `tail` is the optional string to be appended at the end of the
    output.
```altscript
"【".join(strings=("Hello", "World"), separator=", ", tail="】"); // returns "【Hello, World】"
```

## Regular Expression

Regular expression (regex) is used to perform pattern matching within strings. A regular expression can be represented by a string conatining
both special and ordinary characters. See [Regular expression](https://en.wikipedia.org/wiki/Regular_expression) for the format of regex.

The operator ~= or ≅ can be used to check if a string matches to a regular expression:
```altscript
"foo.txt" ≅ r"([a-z]+)\.([a-z]+)"    // the result is true.
```
This operator simply returns true or false, indicating whether a match for the given regex in a string. However, we want to know not just
whether a string is matched, but also how it is matched. To capture this information about a match, we need to create a 'regex' object. The
'regex' object has a constructor that takes a string with the regular expression (the pattern):
```altscript
regex(str: string; opts: regex_opt=(); grammar: regex_grammar=regex_grammar::ECMAScript);
```
`opts` contains a number of options used to interpret how the regular expression is executed in performing match:
```altscript
enumset regex_opts (
    IgnoreCase,         // Regular expressions match without regard to case
    Optimize,           // Matching efficiency is preferred over efficiency constructing regex objects
    Collate             // Character ranges, like "[a-b]", are affected by locale.
);
```
`grammar` determines the syntax format used in the regular expression string, and the default is ECMAScript:
```altscript
enum regex_grammar (
    ECMAScript,   // ECMAScript grammar
    Basic,        // Basic POSIX grammar
    Extended,     // Extended POSIX grammar
    Awk,          // Awk POSIX grammar
    Grep,         // Grep POSIX grammar
    Egrep         // Egrep POSIX grammar
);
```
Here are examples of regex object creation:
```altscript
re1 := rregex("([a-z]+)\.([a-z]+)");
re2 := regex(r"\bd\w+", regex_opts(IgnoreCase), regex_grammar::ECMAScript);
```
A regex object can also be obtained by converting from a string directly:
```altscript
re : regex = r"([a-z]+)\.([a-z]+)";
```
The regex object povides the follinwg methods to perform pattern matching within strings:

* `func match (str:string, flags:regex_flags=()) string...` --
   performs the match to the entire target of the given string. If it matches, it returns a string stream in which the first element is
   the same string given in the input, and the rest elements give substrings that match all subpatterns given in the regular expression.
   If itdoes not match, an empty string stream is reurned. `flags` is an enumset of flags for match/replacement options, which are defined as:
```altscript
enumset regex_flag (
    NotBOL,             // The first character will be ignored. (i.e. ^ will not match the first)
    NotEOL,             // The last character will be ignored. (i.e. $ will not match the first)
    NotBOW,             // The first character will not match word boundary (\b)
    NotEOW,             // The last character will not match word boundary (\b)
    NotNull,            // Do not match empty sequences
    MatchAny,           // If more than one match is possible, then any match is an acceptable result
    AtStartOnly,        // find the match at the start position only
    FirstMatchOnly,     // replace-only flag - replace the first match only
    CopyMatchedOnly     // replace-only flag - do not copy un-matched strings into to the result
);
```
Example of using regex match:
```altscript
re : regex = r"([a-z]+)\.([a-z]+)";
re.match("foo.txt");  // returns a string stream ("foo.txt", "foo", "txt")
```
* `func search (str:string, flags:regex_flags=()): string...` --
   performs the match to some subsequence in the given string. If it finds a match, it returns a string stream in which the first element is
   the unsearched part of the given input, the second element is the substring that mactchs the enire regular expression, and the rest elements
   give substrings that match all subpatterns given in the regular expression. If it does not find any macth, an empty string stream is reurned.
```altscript
re : regex = r"([a-z]+)\.([a-z]+)";
str := "file_names: foo.txt bar.txt";
ma := re.search(str);          // returns a string stream (" bar.txt", "foo.txt", "foo", "txt")
while (ma.length() > 0)
{
    ma = re.search(ma[0]);   // returns a string stream ("", "bar.txt", "bar", "txt")
}
```
* `func findall (str:string, flags:regex_flags=()): string......` --
   performs the match to all subsequence in the given string. If it finds any match, it returns a stream of string stream, in which each
   string stream gives the result of a macth to the subsequence in the given string. 
```altscript
re : regex = r"([a-z]+)\.([a-z]+)";
str := "foo.txt bar.txt";
ma := re.findall(str);          // returns a string stream stream (("foo.txt", "foo", "txt"), ("bar.txt", "bar", "txt"))
```
* `func split (str:string, trim:bool=false, flags:regex_flags=()): string...` --
   performs the match to all seperators in the given string. If any separators are find by the match, the string will be separated into a set of strings based on the position of the separators found. When `trim is true, the lead and trailing spaces in the split strings will be removed.
```altscript
re : regex = "[.,/]+";
str := "12.abc//56, 1237 ... 321";
print(re.split(str, true));          // the output is (12,abc,56,1237,321)
```
* `func replace (str:string, substitute:string, flags:regex_flags=()): string` --
   performs the match to all subsequence in the given string. If it finds a match, it replaces the subsequence with the given substitute and
   returns the substituted string. If it does not find a match, it returns a null string.
```altscript
re : regex = r"([a-z]+)\.([a-z]+)";
str := "foo.txt bar.txt";
re.replace(str, "[$&]");                                                 // returns a string "[foo.txt] [bar.txt]"
re.replace(str, "[$&]", regex_flags(FirstMatchOnly));                    // returns a string "[foo.txt] bar.txt"
re.replace(str, "[$&]", regex_flags(FirstMatchOnly,CopyMatchedOnly));    // returns a string "[foo.txt]"
```

## Use Match Pattern on String

Because strings are streams, like other type of streams and arrays, they can be used in [match statement](StatementMatch.md) to be compared with match patterns:
```altscript
s := "abcd4545xyz";
match (s)
{
    on {.:2 -> first, ... -> second, *('4','5'):2 -> third, ... -> fourth} -> r:
    {
        // do something on r, which has the value (('a', 'b'), "cd", ('4', '5', '4', '5'), "xyz")
        // and r is in the type of  (first: (char; char); second: string; third: (char; char; char; char); fourth: string) 
        print(r);   // the output if the print is ((a,b),cd,(4,5,4,5),xyz)
    }
};
```
Instead of giving a string stream in match result, match pattern gives a tuple structure and each element in the tuple can be optionally named
for convenient access to segment values. For a complete discussion anout match pattern, see [Match Statement](StatementMatch.md).

## String to Value Conversion Using String Parser

We often come across a situation where the data is presented in a string. To extract these data chunks in the string, a string parser is used to  divide the string into a collection of data for the desired information. The class `parser` is defined in the [`sys`](Sys.md) class. Here are some example of using parser:
```
dp := parser("04/24/2003");
month, day, year := (dp.getint(), dp.skipc(), dp.getint(), dp.skipc(), dp.getint());
print (year, '年', month, '月', day, '日');
pp := parser("""John Smith, 36, "124 Pave Lane, Harbor, FL 23045"""");
person : (name: string; age:int; address: string) = (pp.scan(','), pp.getint(), pp.skipc(), pp.getstr());
print ([:C1], person);  // print the person record using the format :C1 (print tuple name and one element per line)
```
The output of the above:
```
2003年4月24日
name = John Smith
age = 36
address = 124 Pave Lane, Harbor, FL 23045
```
For more information about using string parser, see [String Parser](StringParser.md).

## Value to String Conversion Using `Str` Function

The function `str` or `_str` provided in the [`sys`](Sys.md) class is used to convert any type of values into string. `_str` is the same as `str` except that `_str` makes the converted string locale sensitive, i.e. the converted string, when printed, will be translated into the text in the current application language setting.
```
enum DayOfWeek(Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday);
enum Month(Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec);
date: (year:int; month:Month; day: int; day_of_week: DayOfWeek);
date = (2003, Month::Mar, 16, DayOfWeek::Thursday);
date_str := str(date);
print (date_str, EOL);
date_str2 := str(date.month) + ' ' + str(date.day) + ", " + str(date.year)  + ", " +  str(date.day_of_week);
print (date_str2, EOL);
setlanguage("中文", "台灣");
print (date.year, '年', _str(date.month), date.day, "日，", _str(date.day_of_week), EOL);
date_str3 := str([:C1], date);
print (date_str3);
date_str4 := str([:d02], date.month.order()+1) + '-' + str(date.day) + "-" + str(date.year)  + ", " +  str(date.day_of_week);
print (date_str4, EOL);
```
The output of the above:
```
(2003,Mar,16,Thursday)
Mar 16, 2003, Thursday
2003年3月16日，星期四
year = 2003
month = Mar
day = 16
day_of_week = Thursday
03-16-2003, Thursday
```
The string conversion function supports the output formatter. You can use different formatter values to convert the value into the string in a different format. For instance the `date_str3` is converted using the container formatter `:C1`, and the month string in the `date_str4` is converted using the integer formatter `:d02`. For more information about the output formatter, see [Output Formatting](OutputFormatting.md). Note that the text appearance formatters can only be used for the output media that support text appearance such as bold, underline and text color, and they cannot be used for string conversions
