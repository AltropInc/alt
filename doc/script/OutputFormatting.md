# Output Formatting

The `print` function provided in the `sys` library prints text to the screen or any other output device or media. The input to the `print` function can be values or instances of any type. For example,
```altro
print(3.14);  // print a real number
print('π');   // print a character
print("the ratio of circumference to diameter"); // print a string
```
The output on the printed target is a text of one or multiple characters:
```altro
3.14
π
the ratio of circumference to diameter
```
You can provide any nummber of input values to a print function. For example,
```altro
print("π = ", 3.14); // print a string and a real number
```
The output is,
```altro
π = 3.14
```
However, we often wish we can have alternative ways to present the output for the input values. For instance, we wish we can print colored text if the media supports colors, we wish we can print text with specific align arrangement, we wish we can print a real number in percentage, or we wish we can print the date and time in a particular language and region's preference. This is what **output Formatting** does.

## Formatter

A **formatter** is the specification of a printing format. It provides support for layout arrangement and alignment, common formats for numeric, string, and date/time data, locale-specific output, and various text styles such as colors, underline, and boldness. A formatter is created by a formatter type `fmt` which has a set of members for the format for the following:

* Formatters for strings
* Formatters for integers
* Formatters for floating-point numbers

A formatter can be expressed by a formatter expression with a set of formatter values listed in a pair of bracket `[]`. Each format value is typically given by the format member name defined by the `fmt` type and followed by an equal sign with a format value.  For instance,
```altro
[width=40, align=1, fill='─', bold=1]
```
The above expression specifies a formatter for printing right-aligned contents in bold font with minimum column width 40 and dash filled spaces. Using this format to print a string:
```alreo
print([width=40, align=1, fill='─', bold=1], "this is bold text")
```
generates the following screen output:

**───────────────────────this is bold text**

In addition to the typical form of `fromat_name=fromat_value`, a number of shorthands are also provided for readability and convenience. For instance, instead of using integer for color code in the form `color=1`, you can also use color name such as `red`, and `green`. Format values can also be packed in a specific string similar to Python output format. The following expressions give the same set of formatter values:
```altro
[width=40, align=1, fill='─', color=1, bold=1]
[width=40, align=right, fill='─', red, bold]
[width=40, right, fill='─', red, b]
[:s─>40, red, b]
```
Formatter shorthands will be discussed in the following sections.

### Formatters for Strings

Formatters for strings provides specifications of text layout arrangement, alignment and appearance. They are applied to strings converted from all types of values. For example,
```altro
print([italic=1], "pi = ", 3.14);
```
The formatter `[italic=1]` are applied to all arguments following this formatter, including the string `"pi = "` and the floating-point number `3.14`:

*pi = 3.14*

Formatters for strings include **Alignment Formatter** and **Apperance Formatter**:

#### Alignment Formatter

The following members are used for string aligments:
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| start     |  char   | starting character in alignment            |
| end       |  char   | ending character in alignment            |
| fill      |  char   | filling character in alignment             |
| width     |  int    | width (number of screen columns) of alignment            |
| align     |  0~2    | alignment: left(0), right(1), center(2)          |

The *start* is an optional character put at the very beginning of the aligned and filled text, and the *end* is another optional character put at the end. The start and the end characters are not counted in *width*, and they are not affected by other format values such as align, color, and font.

The *width* is an integer for the minimum number of column width required for the output in this format. If the content exceeds the width, nothing is trimmed. Note that the width is not necessarily the number of characters because some characters may occupy two column places in the printed media.

The *fill* is an optional character to fill the space required for the minimum width. If the fill character is not given, space is assumed. If the fill character occupies two column spaces and the number of column spaces is an odd number, a space will be used between the filling and the text.

The *align* value gives the alignment to use. It can be value from 0 to 2. The meaning of these is as follows:
* 0 – The text is left-justified in the field width. This is the default alignment.
* 1 – The text is right-justified in the field width.
* 2 – The text is centered in the field width. Any filling will be distributed evenly on the left and right sides of the value. If an odd number of padding characters is needed, the extra one will always be on the right.

The following shorthands are provided for text alignment:
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| left       | align=0 (left)   |
| right      | align=1 (right)   |
| center     | align=2 (center)   |

The following packed string, started with `:s`, can also be used for string alignment:

:s\[\[\[*start*]*fill*]*align*]\[*width*]\[*end*]

`:s` intruduces a packed string for string alignment formats. If the first character following `:s` is immediately followed by one of the alignment characters, that first character is treated as the fill character to use. The fill and align values only make sense if you also specify a width value, although it is not an error to specify them without width, but without width, the alignment will not take effect. The `align` character can have any of the values <, >, or ^. The meaning of these is as follows:

* \< – left-justified, which is equivalent to `align=0`, and this is the default.
* \> – right-justified, which is equivalent to `align=1`.
* ^ – centered, which is equivalent to `align=2`.

Here are example of using different fill-and-align format to print a number 1.23:
```
format     output
─────────────────────────────────
:s*<10      1.23******
:s*>10      ******1.23
:s*^10      ***1.23***
:s|<10|     |1.23      |
:s|>10|     |      1.23|
:s|^10|     |   1.23   |
```
The start and the end characters are not counted in width, and are not affected by other formats. For instance,
```altro
print (underline, [:s【^10】], 1.23)
```
In the output:<br>
【<ins>&nbsp;&nbsp;&nbsp;1.23&nbsp;&nbsp;&nbsp;</ins>】<br>
the underline effect is not applied to the starting and the ending characters.

#### Apperance Formatter

The following members are used for text apperance:

| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| color     |  0~255  | text color, see [ANSI escape color code assignment](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) |
| bcolor    |  0~255  | background color, see [ANSI escape color code assignment](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) |
| font      |  0~9    | 0 for primary (default) font, 1~9 for alternative fonts        |
| intense   |  0~3    | normal(0)  bold(1), faint(2), gothic(3)                        |
| italic    |  0~1    | no italic(0)  italic(1)                      |
| underline |  0~2    | no underline(0)  underline(1), double underline(2)             |
| overline  |  0~1    | no overline(0)  overline(1)                      |
| strike    |  0~1    | no strike through(0)  strike through(1)                      |
| reverse   |  0~1    | reverse text and background colors: no reverse(0)  reverse(1)      |
| conceal   |  0~1    | hidden text: no conceal(0)  conceal(1)      |
| blink     |  0~2    | no blink(0)  blink(1), fast blink(2)             |

The following shorthands are provided for text apperance:

* **Shorthands for common color names:**
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| /color     | cancel text color, using default text color   |
| /bcolor    | cancel background color, using default background color   |
| black      | color=0 or bcolor=0    |
| blue       | color=4 or bcolor=4 (blue)   |
| bblue      | color=12 or bcolor=12 (bright blue)   |
| bcyan      | color=14 or bcolor=14 (bright cyan)   |
| bgreen     | color=10 or bcolor=10 (bright green)   |
| bmagenta   | color=13 or bcolor=13 (bright magenta)   |
| bred       | color=9 or bcolor=9 (bright red)   |
| bwhite     | color=15 or bcolor=15 (bright white)   |
| byellow    | color=11 or bcolor=11 (bright yellow)   |
| cyan       | color=6 or bcolor=6 (cyan)   |
| gray       | color=8 or bcolor=8 (gray)   |
| green      | color=2 or bcolor=2 (green)   |
| magenta    | color=5 or bcolor=5 (magenta)   |
| red        | color=1 or bcolor=1 (red)   |
| white      | color=7 or bcolor=7 (white)   |
| yellow     | color=3 or bcolor=3 (yellow)   |

* **Shorthands for common text attributes:**
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| b          | intense=1   |
| /b         | intense=0   |
| blink      | blink=1    |
| /blink     | blink=0    |
| bold       | intense=1   |
| /bold      | intense=0   |
| i          | italic=1  |
| /i         | italic=0  |
| italic     | italic=1  |
| /italic    | italic=0  |
| u          | underline=1  |
| /u         | underline=0  |
| underline  | underline=1  |
| /underline | underline=0  |
| uu         | underline=2 (double underline)  |
| strike     | strike=1  |
| /strike    | strike=0  |
| overline   | overline=1  |
| /overline  | overline=0  |
| conceal    | conceal=1  |
| /conceal   | conceal=0  |
| reverse    | reverse=1  |
| /reverse   | reverse=0  |

If you use a common color name as a shorthand to specify the text color or the background color, the first occurence of the color name stands for the text color, and the second occurence of the color name stands for the background color. For example: `[red, white]` stands for red text with white background, which is eqvalent to `[color=red, bcolor=white]`

Using shorthands `/color` or `/bcolor` to reset text color or background color to the default color used by the printing media.

### Formatters for Integers

The following members are used for integers:
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| ibase     |  0~3    | integer base: decimal(0), hexadecimal(1), octal(2), binay(3)             |
| isign     |  0~2    | show negative sign only(0)  show positive and negative sign(1), show positive sign as a space(2)|
| isep      |  0~2    | no thousand separator (0)  locale awareness separator (1) comma as a separator (2)  |
| iwidth    |  int    | minimum number of characters for the output, excluding base indicator           |
| ipad      |  0~1    | using zeros for padding (0), using spaces for padding(1)            |
| showbase  |  0~1    | hide number base(0), show number base (1)            |
| iupper    |  0~1    | using lowercase(0)  using uppercase(1)                 |

The `ibase` value indicates the integer base used to print the number. It can have one of the following values:

* 0 – Decimal format (base 10). This is the default if no presentation of `ibase`.
* 1 – Hexadecimal format (base 16). The digits above 9 are printed as A, B, C, D, E, F, or a, b, c, d, e, f, and the case is determined by the value of `uppercase`
* 2 – Octal format (base 8). For alternate form, the value is prefixed with 0 as long as it is non-zero. For example, 7 outputs as 07, but 0 outputs as 0.
* 3 – Binary format (base 2). For alternate form, the value is prefixed with 0b for b, and 0B for B.

The following shorthands can be for the `ibase` in theinteger format:
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| dec        | ibase=0 (decimal)      |
| hex        | ibase=1 (hexadecimal)  |
| oct        | ibase=2 (octal)        |
| bin        | ibase=3 (binay)        |

Here are examples of integer being printed in different base:
```altro
print(10, ' ', -1, " ⭠ Decimal, this is default\n");
print([hex], 10, ' ', -1, " ⭠ Hexadecimal format\n");
print([oct], 10, ' ', -1, " ⭠ Octal format\n");
print([bin], 10, ' ', -1, " ⭠ Binary format\n");
_______________________________________________________
output:
10 -1 ⭠ Decimal, this is default
a ffffffff ⭠ Hexadecimal format
12 37777777777 ⭠ Octal format
0b1010 0b11111111111111111111111111111111 ⭠ Binary format
```
When using base other than decimal, the integer is always treated as unsigned in order to reflect the values of the underlying bits of the integer.

The `isign` value specifies how the sign for an integer value is to be printed. It can take the following options:

* 0 – A sign should only be printed for negative values. This is the default.
* 1 – A sign should always be printed for both negative and non-negative values.
* 2 – A sign should be printed for negative values, and a space for non-negative values.

Because non-decimal format ignores signs, and the *sign* option has no effect on non-decimal format. Here are examples of using different *sign* options
```altro
print(-10, ' ', 0, ' ', 10, '\n');               // This is default
print([isign=1], -10, ' ', 0, ' ', 10, '\n');
print([isign=2], -10, ' ', 0, ' ', 10, '\n');
_______________________________________________________
output:
-10 0 10
-10 +0 +10
-10  0  10
```
For non-decimal integer format, the `iwidth` value, when present, gives minmum characters for the output. If the number of digits is smaller than the required width, leading zeros will be padded if the `ipad` value is 1, or leading spaces will be padded if the `ipad` value is 0 or is not given. If the value needs more characters than the specified width, it will be displayed in full, not truncated to the width. Here are examples of using `iwidth` and `ipad` to print integers in decimal:
```altro
print([iwidth=8]], 10, ' ', 0, ' ', -10, '\n');                    // width 8, pad with leading spaces
print([iwidth=8, ipad=1]", 10, ' ', 0, ' ', -10, '\n');            // width 8, pad with leading zeros
print([iwidth=8, ipad=1, isign=1]", 10, ' ', 0, ' ', -10, '\n');   // width 8, pad with leading zeros, show positive sign
print([iwidth=8, ipad=1, isign=1]", 1000000000, '\n');             // digits are not truncated to the width
_______________________________________________________
output:
      10        0      -10
00000010 00000000 -0000010
+0000010 +0000000 -0000010
+1000000000
```
For non-decimal integer formats, if `iwidth` is not given or is zero, and `ipad` value is 1, leading zeros will be padded to show all bits of the given integer value. For instance, if the integer value is a 32-bit integer and hecadecimal format is used, leading zeros will be padded to generate a sequence of 8 hecadecimal digits to show all bytes of the value; and if binary format is used, leading zeros will be padded to generate a sequence of 32 binary digits to show all bits of the value. If  `iwidth` is not given, leading zeros (when `ipad` value is 1) or spaces (when otherwise) will be padded to the given width (the base indicator characters, if any, are not counted in the given width). 
```altro
print([ibase=hex], 192, "        ⭠ default hexadecimal format\n");
print([hex, ipad=1], 192, " ⭠ padding with leading 0s to show all bytes of a 32-bit int (8 hexadecimal digits)\n");
print([bin, ipad=1], 192, " ⭠ padding with leading 0s to show all bits of a 32-bit int (32 binary digits)\n");
print([hex, iwidth=4, ipad=1], 192, "     ⭠ padding with leading 0s to the given width\n");
print([hex, iwidth=4], 192, "     ⭠ padding with leading spaces to the given width\n");
print([hex, iwidth=4, ipad=1, showbase=1], 192, "   ⭠ The base indicator `0x` is not counted in the width\n");
_______________________________________________________
output:
c0       ⭠ default hexadecimal format
000000c0 ⭠ padding with leading 0s to show all bytes of a 32-bit int (8 hexadecimal digits)
0b00000000000000000000000011000000 ⭠ padding with leading 0s to show all bits of a 32-bit int (32 binary digits)
00c0     ⭠ padding with leading 0s to the given width
  c0     ⭠ padding with leading spaces to the given width
0x00c0   ⭠ The base indicator `0x` is not counted in the width
```
The 'isep' value, when present, determines if the value needs to be printed with separators. In a decimal format, digits are separated by thousands (every 3 digits); in a hecadecimal format, digits are separated every two digits; in an octal format, digits are separated every three digits; and in a binary format, digits are separated every eight digits. The 'isep' value can take the following options:

* 0 – No thousand separator. This is the default.
* 1 – Locale-aware thousand separator. This takes effect only for decimal format. See [Locale](Locale.md) for more information.
* 2 – Using comma as a thousand separator for decimal format, and use a space as a separator for other base formats.
```altro
print(1000000000, "    ⭠ This is default\n");
print("Now using ", setlocale(LC_NUMERIC, "German", "Germany"), " for numeric output\n");
print([isep=1], 1000000000, " ⭠ Thounsand separator using de_DE locale\n");
print([isep=2], 1000000000, " ⭠ Thounsand separator using comma\n");
print([ibase=1, isep=2, ipad=1], 1000000000, "   ⭠ Using space as separator in hexadecimal format\n");
_______________________________________________________
output:
1000000000    ⭠ This is default
Now using de_DE for numeric output
1.000.000.000 ⭠ Thounsand separator using de_DE locale
1,000,000,000 ⭠ Thounsand separator using comma
3b 9a ca 00   ⭠ Using space as separator in hexadecimal format
```
The `showbase` value determines if the base will be shown in the output for non-decimal format. It this value is 1, the number output will be prefixed by `0X` or `0x` for hecadecimal format, `0B` or `0b` for binary format, and `0` for none-zero numbers in octal format. The case used in the prefix is determined by the value of `iupper`.
```altro
print([ibase=1, showbase=1], 1000000000, "  ⭠ show base in hexadecimal format\n");
print([ibase=1, showbase=1, iupper=1], 1000000000, "  ⭠ show base using uppercase in hexadecimal format\n");
print([ibase=2], 1000000000,  "  ⭠ octal format without showing base\n");
print([ibase=2, showbase=1], 1000000000, " ⭠ octal format with base shown\n");
print([ibase=3, showbase=1], 1000000000, " ⭠ binary format with base shown\n");
_______________________________________________________
output:
0x3b9aca00  ⭠ show base in hexadecimal format
0X3B9ACA00  ⭠ show base using uppercase in hexadecimal format
7346545000  ⭠ octal format without showing base
07346545000 ⭠ octal format with base shown
0b111011100110101100101000000000 ⭠ binary format with base shown
```

**Integer Format with Packed Format String**

The following packed string can also be used for integer output format:<br>
:*base*\[*sign*]*#*]\[*0*]\[*width*]\[*sep*]

The *base* can be one of the following characters:
| base       | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| d          | ibase=0 (decimal)      |
| x          | ibase=1 (hexadecimal)  |
| X          | ibase=1, iupper=1 (hexadecimal)  |
| o          | ibase=2 (octal)        |
| b          | ibase=3 (binary)       |
| B          | ibase=3, iupper=1 (binary)       |
The base character is mandatory.

The *sign* can be one of the following characters:
| sign       | equivalents |
|:---------- |:------------|
| -          | isign=0 (show negative sign only)    |
| +          | ibase=1 (show sign always)    |
| space      | ibase=2 (show positive sign as a space)    |

The `#` character, if present, is eqivalent to `showbase=1`, and causes base to be printed for non-decimal formats.

The `0` character, if present, is eqivalent to `ipad=1`.  The `width` field is an integer, which is eqivalent to `iwdith=<the filed value>`, and used to give the minimum width for a numeric value.  In decimal format, if both `0` and `width` present, leading zeros will be padded after any *sign* character and/or base indicator to meet the specified width. If only `width` present, leading spaces will be padded. In non-decimal formats, the `iwidth` value, leading zeros will be padded if the `0` presents, otherwise leading spaces will be padded.

The *sep* can be one of the following characters:
| shorthand  | equivalents |
|:---------- |:------------|
| L          | isep=1 (locale awareness separator)    |
| '          | isep=2 (comma separator for decimals and space for other formats)    |

Examples of using packed string for integer formats:
```altro
print("Using ", setlocale(LC_NUMERIC, "de_DE"), " for numeric output:\n");
print([:dL], 1000000000, " ⭠ ibase=dec, isep=1(German)\n");
print([:d+L], 1000000000, " ⭠ ibase=dec, isign=1, isep=1(German)\n");
print([:d+24'], 1000000000, " ⭠ ibase=dec, iwidth=24, isign=1, isep=2(comma)\n");
print([:d+024L], 1000000000, " ⭠ ibase=dec, iwidth=24, isign=1, isep=1(German), ipad=1\n");
print([:X08'], 1000000000, " ⭠ ibase=hex, iwidth=8, isep=2(space), iupper=1\n");
print([:x#08], 1000000000, " ⭠ ibase=hex, iwidth=8, showbase=1\n");
_______________________________________________________
output:
Using de_DE for numeric output:
1.000.000.000 ⭠ ibase=dec, isep=1
+1.000.000.000 ⭠ ibase=dec, isign=1, isep=1(German)
+          1.000.000.000 ⭠ ibase=dec, iwidth=24, isign=1, isep=2(comma)
+00000000001.000.000.000 ⭠ ibase=dec, iwidth=24, isign=1, isep=1(German), ipad=1
3B 9A CA 00 ⭠ ibase=hex, iwidth=8, isep=2(space), iupper=1
0x3b9aca00 ⭠ ibase=hex, iwidth=8, showbase=1
```

### Formatters for Floating-Point Numbers

The following members are used for floating-point numbers:
| name       | values  | Description                                                         |
|:---------- |:------- |:-------------------------------------------------------------- |
| fbase      |  0~4    | floating number base: general(0), fixed point(1), scientific(2), percentage(3), hexadecimal(4)             |
| fsign      |  0~2    | show negative sign only(0)  show positive and negative sign(1), show positive sign as a space(2)|
| fsep       |  0~1    | no thousand separator (0)  locale awareness separator and decimal point (1) comma as a separator (2)                    |
| fpad       |  0~1    | using zeros for padding (0), using spaces for padding(1)             |
| fwidth     |  int    | minimum number of characters for the output              |
| fupper     |  0~1    | no using uppercase(0)  using uppercase(1)                 |
| showpoint  |  0~1    | show decimal point only as needed(0)  show decimal point always(1)                 |
| prec       |  int    | precision                 |

The `fbase` value indicates the format used to print the number. It can have one of the following values:

* 0 – Prints the value in a general way. It uses an optimized fixed format that has no trailing zeros and shows decimal points only as needed. However, if the number is too large or too small based on the given precision (default 6), the scientific format will be used. 
* 1 – Prints the value in fixed-point notation with a fixed number of digits after decimal point (default 6). If digits after decimal point are fewer than the number, trailing zeros are added. 
* 2 – Prints the value in scientific notation, the default precision after decimal point is 6.
* 3 – Prints the value in percentage format with a number of digits after decimal point up tp the precision (by default 6)
* 4 – Prints the value using scientific notation, but with the number represented in hexadecimal. Because `e` is a valid hex digit, the exponent is indicated with a `p` character.

The following shorthands can be for the `fbase` in the floating-point number format:
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| fgen       | fbase=0 (general format for floating point number)      |
| fixed      | fbase=1 (fixed point format)  |
| sci        | fbase=2 (scientific)        |
| %          | fbase=3 (percentage)        |
| hsci       | fbase=4 (hexadecimal scientific)  

Here are examples of integer being printed in different base:
```altro
small, integer, large, huge := 0.1234567, 34., 1e10+12.345, 1e20;
print(0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ fbase=0(default)\n");
print([fixed], 0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ fbase=1(fixed)\n");
print([sci], 0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ fbase=2(scientific)\n");
print([%], 0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ fbase=3(percentage)\n");
print([hsci], 0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ fbase=4(hexadecimal scientific)\n");
_______________________________________________________
output:
0 0.123457 34 1e+10 1e+20 ⭠ fbase=0(default)
0.000000 0.123457 34.000000 10000000012.344999 100000000000000000000.000000 ⭠ fbase=1(fixed)
0.000000e+00 1.234567e-01 3.400000e+01 1.000000e+10 1.000000e+20 ⭠ fbase=2(scientific)
0% 12.34567% 3400% 1000000001234.499878% 10000000000000000000000% ⭠ fbase=3(percentage)
0x0p+0 0x1.f9adbb8f8da72p-4 0x1.1p+5 0x1.2a05f2062c28fp+33 0x1.5af1d78b58c4p+66 ⭠ fbase=4(hexadecimal scientific)
```
The `fsign` value specifies how the sign for a value is to be printed. It can take the following options:

* 0 – A sign should only be printed for negative values. This is the default.
* 1 – A sign should always be printed for both negative and non-negative values.
* 2 – A sign should be printed for negative values, and a space for non-negative values.

```altro
print(10.12, ' ', 0.0, ' ', -10.12, "   ⭠ fsign=0(show negative sign only)\n");
print([fsign=1], 10.12, ' ', 0.0, ' ', -10.12, " ⭠ fsign=1(show positive sign)\n");
print([fsign=2], 10.12, ' ', 0.0, ' ', -10.12, " ⭠ fsign=2(show positive sign as a space)\n");
_______________________________________________________
output:
10.12 0 -10.12   ⭠ fsign=0(show negative sign only)
+10.12 +0 -10.12 ⭠ fsign=1(show positive sign)
 10.12  0 -10.12 ⭠ fsign=2(show positive sign as a space)
```
If the `showpoint` value is 1, it causes a decimal point character to always be output, even if there are no digits after it. This does not apply to infinity and NaN values.
```altro
small, integer, large, huge := 0.1234567, 34., 1e10+12.345, 1e20;
print(0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, "     ⭠ show decimal point as needed (default)\n");
print(showpoint=1], 0.0, ' ', small, ' ', integer, ' ', large, ' ', huge, " ⭠ show decimal point always\n");
_______________________________________________________
output:
0 0.123457 34 1e+10 1e+20     ⭠ show decimal point as needed (default)
0. 0.123457 34. 1.e+10 1.e+20 ⭠ show decimal point always
```
If the `fupper` value is 1, uppercase will be used for `e` and `p` in scientific notion, and uppercase hexadecimal digits will be used.
```altro
print([fupper=1], 1e10, " ⭠ using uppercase for `e`\n");
print([hsci, fupper=1], 1e10, " ⭠ using uppercase for `p` and hex digits\n");
_______________________________________________________
output:
1E+10 ⭠ using uppercase for `e`
0X1.2A05F2P+33  ⭠ using uppercase for `p` and hex digits
```
If `prec` is present, the output will have the precision as specified. If `prec` is not present, the default precision is 6. The precision has different meanings in different formats. In fixed format, the precision determines the number of digits after the decimal point:
```altro
print([fixed], 0.123456789, "       ⭠ default precision in fixed\n");
print([fixed, prec=12], 0.123456789, " ⭠ precision 12 in fixed\n");
_______________________________________________________
output:
0.123457       ⭠ default precision in fixed
0.123456789000 ⭠ precision 12 in fixed
```
In the general (default) format, the precision determines how large the number can be before the fixed format is switched to the scientific format. This is the same as the `g` or `G` format used in other programming languages such as C++ and Python. The detail description of the rule that governs choice between scientific format and the fixed format with a given precision can be found in [Python G format](https://python-reference.readthedocs.io/en/latest/docs/functions/format.html#type). Here is the example how precision affect the choice of the optimized fixed format and the scientific format when the general format is used:
```altro
print(1234567.0, " ⭠ using default precision 6 and the scientific format is used\n");
print([prec=8], 1234567.0, "     ⭠ using customized precision 8, and the optimized fixed format is used\n");
_______________________________________________________
output:
1.23457e+06 ⭠ using default precision 6 and the scientific format is chosen
1234567     ⭠ using customized precision 8, and the optimized fixed format is chosen
```
The 'fsep' value, when present, determines if the value needs to be printed with separators in fixed format, and how the decimal point is printed. The 'fsep' value can take the following options:

* 0 – No thousand separator. This is the default.
* 1 – Locale-aware thousand separator and decimal point. Thousand separator takes effect only for fixed format.
* 2 – Using comma as a thousand separator in fixed format.

```altro
print(1000000000, "    ⭠ This is default\n");
print("Now using ", setlocale(LC_NUMERIC, "German", "Germany"), " for numeric output\n");
print([isep=1], 1000000000, " ⭠ Thounsand separator using de_DE locale\n");
print([isep=2], 1000000000, " ⭠ Thounsand separator using comma\n");
print([ibase=1, isep=2, ipad=1], 1000000000, "   ⭠ Using space as separator in hexadecimal format\n");
_______________________________________________________
output:
1000000000    ⭠ This is default
Now using de_DE for numeric output
1.000.000.000 ⭠ Thounsand separator using de_DE locale
1,000,000,000 ⭠ Thounsand separator using comma
3b 9a ca 00   ⭠ Using space as separator in hexadecimal format
```









You can also use the following format to specift a numeric format:<br>
:\[*sign*]*#*]\[*0*]\[*width*]\[*prec*]\[*L*]\[*type*]

The sign value specifies how the sign for an numeric value is to be printed. It can take the following options:

* `+` – A sign should always be printed for both negative and non-negative values.
* `-` – A sign should only be printed for negative values. This is the default.
* (space) – A sign should be printed for negative values, and a space for non-negative values.

The `#` character, if used, causes a decimal point character to always be printed, even if there is no fractional part. This does not apply to infinity and NaN values.

The `0` character is only valid when also specifying a width value. If present it pads the field with 0 characters after any *sign* character and/or base indicator. The *width* value is used to give the minimum width for a numeric value. If the value needs more characters than the specified width, it will be displayed in full, not truncated to the width.

The *prec* value is formed by a decimal point followed by an integer to indicate the precision. The precision value is only valid for floating-point. It is used to determine how many digits after decimals should be printed.

The 'L' character, when present, indicates that the value should be printed in a locale-aware format, for instance, using comma as decimal point and using space for thousand separator in German locale. See [Locale](Locale.md) for more information.

The *type* character gives differnt floating-point presentation as given below.

* `e` – Prints the value in scientific notation. If no *prec* value is given, it defaults to 6.
* `f` – Prints the value in fixed-point notation. If no *prec* value is given, it defaults to 6.
* `g` – Prints the value in a general way, which picks between `e` and `f` form depending on which form is the shortest.
* `a` – Prints the value using scientific notation, but with the number represented in hexadecimal. Because `e` is a valid hex digit, the exponent is indicated with a `p` character.

**Shorthands for Integer Values**
| fgen       | fbase=0 (general format for floating point number)      |
| fixed      | fbase=1 (fixed point format)  |
| sci        | fbase=2 (scientific)        |
| %          | fbase=3 (percentage)        |
| hsci       | fbase=4 (hexadecimal scientific)  

