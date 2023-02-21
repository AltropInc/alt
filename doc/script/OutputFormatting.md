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
You can provide any number of input values to a print function. For example,
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
* Formatters for elements of containers

A formatter can be expressed by a formatter expression with a set of formatter values listed in a pair of brackets `[]`. Each format value is typically given by the format member name defined by the `fmt` type and followed by an equal sign with a format value.  For instance,
```altro
[width=40, align=1, fill='─', bold=1]
```
The above expression specifies a formatter for printing right-aligned contents in bold font with minimum column width 40 and dash filled spaces. Using this format to print a string:
```altro
print([width=40, align=1, fill='─', bold=1], "this is bold text")
```
generates the following screen output:
<pre>
<b>───────────────────────this is bold text</b>
</pre>

In addition to the typical form of `fromat_name=fromat_value`, a number of shorthands are also provided for readability and convenience. For instance, instead of using integers for color code in the form `color=1`, you can also use color names such as `red`, and `green`. Format values can also be packed in a specific string similar to Python output format. The following expressions give the same set of formatter values:
```altro
[width=40, align=1, fill='─', color=1, bold=1]
[width=40, align=right, fill='─', red, bold]
[width=40, right, fill='─', red, b]
[:s─>40, red, b]
```
Formatter shorthands will be discussed in the following sections.

There are two ways to use formatters in `print` function:

* Provide formatters as input parameters.
* Provide formatters in a format string.

When we use a formatter as an input parameter, the formatter can be inserted in any position in the input list of the `print` function. The formatter value takes effect on all inputs that follow the formatter until the end of the input list or a new formatter value of the same type which overrides the previous one. The effect of the formatter is only active within the current print function. Example:
```altro
print ("output:\n", [hex], 10, ' ', 20, ' ', 30, ' ', [dec], 40, ' ', 50, ' ', 60);
─────────────────────────────────
output:
a 14 1e 40 50 60
```
When we use formatters in a format string, the formatter is applied only to the corresponding input parameter in the `print` function. For example:
```altro
print ([format="output: {[bin]}, {[oct]}, {[dec]}, {[hex]} \n"], 10, 20, 30, 47);
─────────────────────────────────
output:  0b1010, 24, 30, 2f
```

### Formatters for Strings

Formatters for strings provide specifications of text layout arrangement, alignment and appearance. They are applied to strings converted from all types of values. For example,
```altro
print([italic=1], "pi = ", 3.14);
```
The formatter `[italic=1]` are applied to all arguments following this formatter, including the string `"pi = "` and the floating-point number `3.14`:

*pi = 3.14*

Formatters for strings include **Alignment Formatter** and **Appearance Formatter**:

#### Alignment Formatter

The following members are used for string alignments:
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| start     |  char   | starting character in alignment            |
| end       |  char   | ending character in alignment            |
| fill      |  char   | filling character in alignment             |
| width     |  int    | width (number of screen columns) of alignment            |
| align     |  0~2    | alignment: left(0), right(1), center(2)          |
| level     |  int    | alignment element level          |

The *start* is an optional character put at the very beginning of the aligned and filled text, and the *end* is another optional character put at the end. The start and the end characters are not counted in *width*, and they are not affected by other format values such as align, color, and font.

The *width* is an integer for the minimum number of column width required for the output in this format. If the content exceeds the width, nothing is trimmed. Note that the width is not necessarily the number of characters because some characters may occupy two column places in the printed media.

The *fill* is an optional character to fill the space required for the minimum width. If the fill character is not given, space is assumed. If the fill character occupies two column spaces and the number of column spaces is an odd number, a space will be used between the filling and the text.

The *align* value gives the alignment to use. It can be a value from 0 to 2. The meaning of these is as follows:
* 0 – The text is left-justified in the field width. This is the default alignment.
* 1 – The text is right-justified in the field width.
* 2 – The text is centered in the field width. Any filling will be distributed evenly on the left and right sides of the value. If an odd number of padding characters is needed, the extra one will always be on the right.

The *level* is an integer for the level of elements to be aligned. When the level is 0, the alignment is applied to the elements at the same level of the formatter; When the level is 1, the alignment is applied to the elements at a deeper level, that is, the elements of the elements at the same level of the formatter; and so on.

The following shorthands are provided for text alignment:
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| left       | align=0 (left)   |
| right      | align=1 (right)   |
| center     | align=2 (center)   |

The following packed string, started with `{\*}:s`, can also be used for string alignment:

{\*}:s\[\[\[*start*]*fill*]*align*]\[*width*]\[*end*]

`{\*}:s` introduces a packed string for string alignment formats. The optional staring '\*' sequence indicates the alignment level, which is equal to the number of stars in the sequence. If the first character following `:s` is immediately followed by one of the `align` characters (<, >, or ^), that first character is treated as the fill character to use. The fill and align values only make sense if you also specify a width value, although it is not an error to specify them without width, but without width, the alignment will not take effect. The meaning of the `align` character is as follows:

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

Here are example of using different alignment levels to print a composite data `('x', "xyz", (2,3))`:
```altro
print([:s| ^20|], ('x', "xyz", (2,3)), [/align], " ⭠ aligment at level 0\n");
print([*:s| ^20|], ('x', "xyz", (2,3)), " ⭠ aligment at level 1\n");
print([**:s| ^20|], ('x', "xyz", (2,3)), " ⭠ aligment at level 2\n");
```
The first print center-aligned the output `(x,xyz,(2,3))` as a whole. The second print center-aligned each element of the output `(x,xyz,(2,3))`. The third print center-aligned each element of the `(2,3)`, which is the last element of the output `(x,xyz,(2,3))`. The print output is shown as below:
```
|   (x,xyz,(2,3))    | ⭠ aligment at level 0
(|         x          |,|        xyz         |,|       (2,3)        |) ⭠ aligment at level 1
(x,xyz,(|         2          |,|         3          |)) ⭠ aligment at level 2
```

#### Appearance  Formatter

The following members are used for text appearance:

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

The following shorthands are provided for text appearance:

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

If you use a common color name as a shorthand to specify the text color or the background color, the first occurance of the color name stands for the text color, and the second occurance of the color name stands for the background color. For example: `[red, white]` stands for red text with white background, which is equivalent to `[color=red, bcolor=white]`.

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

The following shorthands can be for the `ibase` in the integer format:
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
When using a base other than decimal, the integer is always treated as unsigned in order to reflect the values of the underlying bits of the integer.

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
The `iwidth` value, when present, gives minimum characters for the output. If the number of digits is smaller than the required width, leading zeros will be padded if the `ipad` value is 1, or leading spaces will be padded if the `ipad` value is 0 or is not given. If the value needs more characters than the specified width, it will be displayed in full, not truncated to the width. Here are examples of using `iwidth` and `ipad` to print integers in decimal:
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
For non-decimal integer formats, if `iwidth` is not given or is zero, and `ipad` value is 1, leading zeros will be padded to show all bits of the given integer value. For instance, if the integer value is a 32-bit integer and hexadecimal format is used, leading zeros will be padded to generate a sequence of 8 hexadecimal digits to show all bytes of the value; and if binary format is used, leading zeros will be padded to generate a sequence of 32 binary digits to show all bits of the value. If  `iwidth` is not given, leading zeros (when `ipad` value is 1) or spaces (when otherwise) will be padded to the given width (the base indicator characters, if any, are not counted in the given width).
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
The 'isep' value, when present, determines if the value needs to be printed with separators. In a decimal format, digits are separated by thousands (every 3 digits); in a hexadecimal format, digits are separated every two digits; in an octal format, digits are separated every three digits; and in a binary format, digits are separated every eight digits. The 'isep' value can take the following options:

* 0 – No thousand separators. This is the default.
* 1 – Locale-aware thousand separators. This takes effect only for decimal format. See [Locale](Locale.md) for more information.
* 2 – Using comma as a thousand separator for decimal format, and use a space as a separator for other base formats.
```altro
print(1000000000, "    ⭠ This is default\n");
print("Now using ", setlocale(LC_NUMERIC, "German", "Germany"), " for numeric output\n");
print([isep=1], 1000000000, " ⭠ Thousand separator using de_DE locale\n");
print([isep=2], 1000000000, " ⭠ Thousand separator using comma\n");
print([ibase=1, isep=2, ipad=1], 1000000000, "   ⭠ Using space as separator in hexadecimal format\n");
_______________________________________________________
output:
1000000000    ⭠ This is default
Now using de_DE for numeric output
1.000.000.000 ⭠ Thousand separator using de_DE locale
1,000,000,000 ⭠ Thousand separator using comma
3b 9a ca 00   ⭠ Using space as separator in hexadecimal format
```
The `showbase` value determines if the base will be shown in the output for non-decimal format. It this value is 1, the number output will be prefixed by `0X` or `0x` for hexadecimal format, `0B` or `0b` for binary format, and `0` for non-zero numbers in octal format. The case used in the prefix is determined by the value of `iupper`.
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
:*format*\[*sign*]*#*]\[*0*]\[*width*]\[*sep*]

The *format* can be one of the following characters:
| format     | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| d          | ibase=0 (decimal)      |
| x          | ibase=1 (hexadecimal)  |
| X          | ibase=1, iupper=1 (hexadecimal)  |
| o          | ibase=2 (octal)        |
| b          | ibase=3 (binary)       |
| B          | ibase=3, iupper=1 (binary)       |
The format character is mandatory.

The *sign* can be one of the following characters:
| sign       | equivalents |
|:---------- |:------------|
| -          | isign=0 (show negative sign only)    |
| +          | isign=1 (show sign always)    |
| space      | isign=2 (show positive sign as a space)    |

The `#` character, if present, is equivalent  to `showbase=1`, and causes base to be printed for non-decimal formats.

The `0` character, if present, is equivalent to `ipad=1`.  The `width` field is an integer, which is equivalent to `iwdith=<the filed value>`, and used to give the minimum width for a numeric value.  In decimal format, if both `0` and `width` present, leading zeros will be padded after any *sign* character and/or base indicator to meet the specified width. If only `width` present, leading spaces will be padded. In non-decimal formats, the `iwidth` value, leading zeros will be padded if the `0` presents, otherwise leading spaces will be padded.

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
| fsep       |  0~1    | no thousand separator (0)  locale awareness separator(1) comma as a separator (2)                    |
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
The 'fsep' value, when present, determines if the value needs to be printed with thousand separators in general or fixed format. If the thousand separator is used, the general format will never use scientific format, and use the optimized fixed format instead. The 'fsep' value can take the following options:

* 0 – No thousand separator. This is the default.
* 1 – Locale-aware thousand separator. Thousand separator takes effect only for general and fixed format.
* 2 – Using commas as a thousand separator in fixed format.

Some numeric locales use commas as a decimal point, and this causes conflict when choosing commas as thousand separators. In this case, the decimal point will be switched back to the default form `.`.  
```altro
print("Now using ", setlocale(LC_NUMERIC, "de_DE"), " for numeric output\n");
print([fsep=1], 2000000000.25, "     ⭠ general format, locale-aware separators\n");
print([fsep=2], 2000000000.25, "     ⭠ general format, commas as separators\n");
print([fixed, fsep=1], 2000000000.25, " ⭠ fixed format, locale-aware separators\n");
print([fixed, fsep=2], 2000000000.25, " ⭠ fixed format, commas as separators\n");
_______________________________________________________
output:
Now using de_DE for numeric output
2.000.000.000,25     ⭠ general format, locale-aware separators
2,000,000,000.25     ⭠ general format, commas as separators
2.000.000.000,250000 ⭠ fixed format, locale-aware separators
2,000,000,000.250000 ⭠ fixed format, commas as separators
```
The `fwidth` value, when present in positive value, gives minimum characters for the output. If the number of output characters is smaller than the required width, leading zeros will be padded if the `fpad` value is 1, or leading spaces will be padded if the `fpad` value is 0 or is not given. Leading zeros or spaces will be padded after any *sign* character. If only `width` is present, leading spaces will be padded.. If the value needs more characters than the specified width, it will be displayed in full, not truncated to the width. Here are examples of using `fwidth` and `fpad` to print floating numbers in decimal.
```altro
print([fsep=2, fwidth=20, fpad=1], 2000000.25, " ⭠ padding with zeros\n");
print([fwidth=20], 2000000.25, " ⭠ padding with spaces\n");
print([sci, fwidth=20], -2000000.25, " ⭠ padding with spaces after sign\n");
_______________________________________________________
output:
000000002,000,000.25 ⭠ padding with zeros 
          2000000.25 ⭠ padding with spaces
-       2.000000e+06 ⭠ padding with spaces after sign
```
Note that thousand separators do not count for leading zeros.

The `ftrim` value, when present with value equal to one, will trim trailing zeros, if any. If `showpoint` value is not given to one, the decimal point will also be trimed.
```altro
print([showpoint=1], 12.5, "      ⭠ general format with options that always shows fractional part\n");
print([fixed], 12.5, "    ⭠ fixed format which shows fractional part \n");
print([sci], 10., " ⭠ scientific format which shows fractional part \n");
print([showpoint=1, ftrim=1], 2.25, "        ⭠ trailing zero trimmed in general format\n");
print([fixed, ftrim=1], 2.25, "         ⭠ trailing zero trimmed in scientific format\n");
print([sci, ftrim=1], 1e+01, "        ⭠ trailing zero trimmed in scientific format\n");
_______________________________________________________
output:
12.5000      ⭠ general format with options that always shows fractional part
12.500000    ⭠ fixed format which shows fractional part
1.000000e+01 ⭠ scientific format which shows fractional part
2.25         ⭠ trailing zero trimmed in general format
2.25         ⭠ trailing zero trimmed in scientific format
1e+01        ⭠ trailing zero trimmed in scientific format
```

**Float-Point Number Format with Packed Format String**

You can use the following format string to define a format to print a float-point number:<br>
:*format*\[*sign*]*#*]\[*0*]\[*width*]\[*sep*]\[*prec*]

The *format* can be one of the following characters:
| format     | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| g          | fbase=0 (general)      |
| G          | fbase=0 (general), fupper=1      |
| f          | fbase=1 (fixed)  |
| F          | fbase=1 (fixed), fupper=1  |
| e          | fbase=2 (scientific)        |
| E          | fbase=2 (scientific), fupper=1       |
| a          | fbase=3 (hexadecimal scientific)       |
| a          | fbase=3 (hexadecimal scientific), fupper=1      |
The base character is mandatory.

The *sign* can be one of the following characters:
| sign       | equivalents |
|:---------- |:------------|
| -          | fsign=0 (show negative sign only)    |
| +          | fsign=1 (show sign always)    |
| space      | fsign=2 (show positive sign as a space)    |

The `#` character, if present, is equivalent to `showpoint=1`, and causes the fractional part to be aways printed even if the value has no fractional part.

The `0` character, if present, is equivalent to `fpad=1`.  The `width` field is an integer, which is equivalent to `fwdith=<the field value>`, and used to give the minimum width for a numeric value.  If both `0` and `width` present, leading zeros will be padded after any *sign* character to meet the specified width. If only `width` is present, leading spaces will be padded.

The *sep* can be one of the following characters:
| shorthand  | equivalents |
|:---------- |:------------|
| L          | fsep=1 (locale awareness separator)    |
| '          | fsep=2 (comma separator for decimals and space for other formats)    |

The *prec* field value is formed of a decimal point followed by the precision integer, and is eqivalent to `prec=<the filed value>`. 

Examples of using packed string for integer formats:
```altro
print([:f], 1000000000.25, " ⭠ [:f] fixed\n");
print([:e], 1000000000.25, " ⭠ [:e] scientific\n");
print([:g], 1000000000.25, " ⭠ [:g] general, \n");
print([:g#], 1000000000.25, " ⭠ [:g#] general, show fractional part\n");
print([:g#/], 1000000000.25, " ⭠ [:g#] general, show fractional part but remove trailing zeros\n");
print([:g.14], 1000000000.25, " ⭠ [:g.14] general, prec=14\n");
print([:g#.14], 1000000000.25, " ⭠ [:g#.14] general, prec=14, trailing  zeros to make up 14 digits \n");
print([:g+], 1000000000.25, " ⭠ [:g+] general, show positive sign\n");
print([:g/'], 1000000000.25, " ⭠ [:g/'] general, remove trailing zeros, comma separated\n");
print([:g24'], 1000000000.25, " ⭠ [:g24'] general, commas separated, leading spaces to make up width of 24\n");
print("Using ", setlocale(LC_NUMERIC, "de_DE"), " for numeric output:\n");
print([:gL], 1000000000.25, " ⭠ [:gL] general, separated by locale-aware separators\n");
print([:E24L.12], 1000000000.25, " ⭠ [:E24L.12] scientific, fupper=1, fwidth=24, fsep=1(German), prec=12\n");
print([:g024L], 1000000000.25, " ⭠ [:g+024L] general, fwidth=24, fsep=1(German), fpad=1\n");
_______________________________________________________
output:
1000000000.250000 ⭠ [:f] fixed
1.000000e+09 ⭠ [:e] scientific
1e+09 ⭠ [:g] general,
1.00000e+09 ⭠ [:g#] general, show fractional part
1.e+09 ⭠ [:g#] general, show fractional part but remove trailing  zeros
1000000000.25 ⭠ [:g.14] general, prec=14
1000000000.2500 ⭠ [:g#.14] general, prec=14, trailing zeros to make up 14 digits
+1e+09 ⭠ [:g+] general, show positive sign
1,000,000,000.25 ⭠ [:g/'] general, remove trailing zeros, comma separated
    1,000,000,000.250000 ⭠ [:g24'] general, commas separated, leading spaces to make up width of 24
Using de_DE for numeric output:
1.000.000.000,250000 ⭠ [:gL] general, separated by locale-aware separators
      1,000000000250E+09 ⭠ [:E24L.12] scientific, fupper=1, fwidth=24, fsep=1(German), prec=12
00001.000.000.000,250000 ⭠ [:g+024L] general, fwidth=24, fsep=1(German), fpad=1
```

### Formatters for Composite Values

The following members are used for the format of composite values:
| name       | values  | Description                                                         |
|:---------- |:------- |:-------------------------------------------------------------- |
| cnum       |  int    | number of elements to be displayed per line, zero or default means single line. |
| cmax       |  int    | maximum number of elements to be displayed |
| csep       |  char   | separator character. '0' means no separator. Default is ','             |
| cstart     |  char   | starting character. '0' means no starting. Default is '('             |
| cend       |  char   | ending character. '0' means no ending. Default is ')'              |
| cnamed     |  0~1    | display value only(0)  display value and name or index(1)                 |
    
The `cnum` value indicates the format used to print the number.

You can use the following format string to define a format to print a float-point number:<br>
:*format*\[\[*start*]*sep*]\[*cnum*]\[*end*]\[*cmax*]

The *format* can be one of the following characters:
| format     | equivalents                                                  |
|:---------- |:------------------------------------------------------------ |
| c          | cnamed=0, print composite element values only                |
| C          | cnamed=1, print composite element name or index and values   |
The format character is mandatory.

The *start* character is the starting character for the composite value. The default starting character is `(`.  The *sep* character is the separator character between elements. The default separator character is `,`. The *end* character is the ending character for the composite value. The default ending character is `)`. Because characters '\0', `,` and `]` has specialing meaning in a format string (they are the terminator of a format string), you have to used a replacement character for  these characters. The following is a list of replacement characters:

| character       | equivalents |
|:---------- |:------------|
| n          | '\0': no character for start, end or separator is displayed    |
| b          | ']': the character is a right bracket    |
| c          | ',': the character character is a comma    |

The *cnum* field is an integer to indicated how many elements will be displayed per line. The *cmax* field value is formed of a decimal point followed by the integer to indicate the maximum number of elements to be displayed. 

Examples of printing an tuple value with format `c` and `C`:
```altro
p: tuple (name: string; age: int; address: string) = ("John Smith", 27, "47 Arbor Lane, FL 23034");
print("Unamed, all elemenets in one line with brackets:\n", [:c[c0b], p, '\n');
print("Named, one element per line, no brackets, no separactor:\n", [:Cnn1n], p, '\n');
_______________________________________________________
Unamed, all elemenets in one line with brackets:
[John Smith,27,47 Arbor Lane, FL 23034]
Named, one element per line, no brackets, no separactor:
name = John Smith
age = 27
address = 47 Arbor Lane, FL 23034
```
If the element is unnamed, using named format will cause index of the element being printed:
```altro
print([:Cnc1n.3], (1, 2, 3), '\n');
_______________________________________________________
[0] = 1
[1] = 2
[2] = 3
```
If *cmax* is given and not all elements are printed, `...` will be appended at the end of the output:
```altro
print([:cnc0n.3], (1, 2, 3, 5, 6, 7, 8, 9, 10), '\n');
_______________________________________________________
1,2,3, ...
```

## Using Formatters as Input Parameters in Print

A formatter can be used as an input parameter inserted in any position in the input list of a `print` function call. The formatter value takes effect on all inputs that follow the formatter until the end of the input list or a new formatter value of the same type which overrides the previous one. This resembles using [C++ output manipulators](https://cplusplus.com/reference/library/manipulators/) to change formatting parameters on streams, except that the formatter takes effect only within the current `print` function call. 

Here is an example of calling the `print` with multiple formatters:
```altro
for (x := 1; x<10; ++x)
    print( [bold], x,   // print x in bold
           [/bold],     // cancel bold attribute
           ": ",        // print string ": " in normal
           [:s<10],  x, "² = ", x²,  // print x, string "² = ", and x² together in specified alignment
           [:s<10], x, "³ = ", x³,   // print x, string "³ = ", and x³ together in specified alignment
           "\n"         // print a new line
         );
```
The output:<br><pre>
<b>1</b>: 1² = 1    1³ = 1
<b>2</b>: 2² = 4    2³ = 8
<b>3</b>: 3² = 9    3³ = 27
<b>4</b>: 4² = 16   4³ = 64
<b>5</b>: 5² = 25   5³ = 125
<b>6</b>: 6² = 36   6³ = 216
<b>7</b>: 7² = 49   7³ = 343
<b>8</b>: 8² = 64   8³ = 512
<b>9</b>: 9² = 81   9³ = 729
</pre>

When using packed format for string alignment and numeric format, all previous formatter values in that category, if any, are overridden, even when the respective value is not provided in the packed format. For instance,
```altro
for (x := 1; x<=1000000000; x*=10)
      print( [:d16'], x, // :d16' is a packed format for integer with ibase=dec, iwidth=16, isep=2
             ' ',
             [:X'], x,   // :X' is a packed format for integer with ibase=hex, isep=2
             "\n"
           );
```
When the formatter `[:X']` overrides the formatter `[:d16']`, all format values for integer format are overridden. The format value `iwidth`, though not presented in `[:X']`, the default value `0` is used to override the previous value `16`. Here is the output:<br>
```
               1 1
              10 A
             100 64
           1,000 3 E8
          10,000 27 10
         100,000 1 86 A0
       1,000,000 F 42 40
      10,000,000 98 96 80
     100,000,000 5 F5 E1 00
   1,000,000,000 3B 9A CA 00
```

## Using Formatters in Format String

A format string can be used in a formatter as the first input parameter in the input list of a `print` function call. In the format string, the place holder defined in curly brackets `{}` is used to represent a particular input parameter in the function call. The placeholders can be just an empty placeholder and the input parameter chosen is the one in the same order appearing in the input list:
```altro
print ([format="My name is {}, I'm {}\n"], "John", 36);
```
The output is
```
My name is John, I'm 36
```
Placeholders can be indexed, allowing us to change the order of the arguments or even repeat them:
```altro
print ([format="My name is {2}, I'm {1}\n"], 36, "John");
```
The output is
```
My name is John, I'm 36
```
Note that the index starts from 1, because the first input parameter is the format string itself.

In the placeholder brackets `{}`, you can put formatters to customize the output format. These formatters must be specific to the type of its corresponding input parameter, otherwise, it  does not take any effect. The formatter appear in the placeholder has no effect on the input parameters corresponding to the next placeholder, even the placeholder maps to the same input parameter: 
```altro
print ([format="binary 10: {1:[bin]}, default 10: {1}, Hexicadecimal 10: {1:[:X#02]} \n"], 10);
```
Output:
```
binary 10: 0b1010, default 10: 10, Hexicadecimal 10: 0X0A
```
If an unindexed placeholder follows an indexed placeholder, the input parameter is the next one to the input parameter corresponding to the previous indexed placeholder:
```altro
print ([format="My name is {1}, I'm {}\n"], "John", 36);
```
The output is
```
My name is John, I'm 36
```
If there are remaining input parameters after the input parameter with the largest index used by all placeholders in the format string, the remaining input parameters will be printed at the end:
```altro
print ([format="My name is {2}, I'm {1}"], 36, "John", " years old.\n");
```
The output is
```
My name is John, I'm 36 years old.
```
If the input parameter corresponding to the placeholder is a formatter, the input parameter next to the formatter will be used, and the formatter will be applied to the input parameter that is used: 
```altro
print ([format="My name is {1}, I'm {}"], [bold], "John", 36, " years old.\n");
```
The output is
<pre>
My name is <b>John</b>, I'm 36 years old.
</pre>
Note that the formatter `[bold]` is applied only to the input parameter corresponded by the placeholder.

The formatter value used in the first formatter with the format string will be applied to app parameters and the characters in the format string:
```altro
print ([format="My name is {1}, I'm {[/bold]}", bold], "John", 36, " years old.\n");
```
The `bold` format value is applied to all parameters and the characters in the format string, except for the integer 36 which has the formatter value that overrides the bold attribute. The output is:
<pre>
<b>My name is John, I'm </b>36<b> years old.</b>
</pre>


