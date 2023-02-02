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

A **formatter** is the specification of a printing format. It provides support for layout arrangement and alignment, common formats for numeric, string, and date/time data,  locale-specific output, and various text styles such as colors, underline, and boldness. A formatter is created by a formatter type `fmt` which has a set of members for various formats described below.

### Formats for alignment
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| fill      |  char   | filling character in alignment             |
| start     |  char   | starting character in alignment            |
| end       |  char   | ending character in alignment            |
| width     |  int    | width (number of screen columns) of alignment            |
| align     |  0~3    | alignment: left(0), right(1), center(2), internal(3)           |

### Formats for Numbers
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| base      |  0~3    | integer base: decimal(0), hexadecimal(1), octal(2), binay(3)             |
| fbase     |  0~4    | floating number base: optimized(0), fixed point(1), scientific(2), percentage(3), hexadecimal(4)             |
| precision |  int    | precision for printing floating number   |
| sign      |  0~2    | show negative sign only(0)  show positive and negative sign(1), show positive sign as a space(2)|
| locale    |  0~1    | no locale awareness(0)  locale awareness (1)                    |
| trim      |  0~1    | trim leading and trailing zeros: no trim(0)  trim(1)            |
| intbase   |  0~3    | decimal(0), hexadecimal(1), octal(2), binay(3)             |
| uppercase |  0~1    | no using uppercase(0)  using uppercase(1)                 |

### Formats for Apperance

Formats for appearance are supported only when the printed media support text colors, fonts, and decorations.

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

## Formatter Expression

Formatter values are listed in a pair of Bracket `[]`. Each format value is typically given by the format name followed by an equal sign with a format value.  For instance,
```alreo
[width=40, align=1, fill='─', color=1]
```
The above expression specifies a formatter for print right-aligned contents in red color with minimum column width 40 and dot filled spaces. Using this format to print a number:
```alreo
print([width=40, align=1, fill='─', color=1], 0.23456)
```
The output on screen is<br>
<span style="color:red">─────────────────────────────────0.23456</span><br>

In addition to the typical form of `fromat_name=fromat_value`, a number of shorthands and aliases are also provided for readability and convenience. For instance, instead of using integer for color code, you can also use color name such as `red`, and `green`. Format values can be packed in a specific string like Python output format. The following expressions give the same set of formatter values:
```altro
[width=40, align=1, fill='─', color=1]
[width=40, align=right, fill='─', color=red]
[width=40, right, fill='─', red]
[:─>40, red]
```
### Format Shorthands

**shorthands for colors**

The following common color names can be used. The first occurence of the color name stands for the text color, and the second occurence of the color name stands for the background color. For example:
```altro
[red, white]
```
stands for red text with white background. Using shorthands `/color` or `/bcolor` to reset text color or background color to the default color used by the printing media. Here are the of common color names:
| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| /color     | cancel text color, using default text color   |
| /bcolor    | cancel background color, using default background color   |
| black      | color=0 or bcolor=0    |
| blue       | color=4 or bcolor=4 (blue)   |
| bblue      | color=12 or bcolor=12 (birght blue)   |
| bcyan      | color=14 or bcolor=14 (birght cyan)   |
| bgreen     | color=10 or bcolor=10 (birght green)   |
| bmagenta   | color=13 or bcolor=13 (birght magenta)   |
| bred       | color=9 or bcolor=9 (birght red)   |
| bwhite     | color=15 or bcolor=15 (birght white)   |
| byellow    | color=11 or bcolor=11 (birght yellow)   |
| cyan       | color=6 or bcolor=6 (cyan)   |
| gray       | color=8 or bcolor=8 (gray)   |
| green      | color=2 or bcolor=2 (green)   |
| magenta    | color=5 or bcolor=5 (magenta)   |
| red        | color=1 or bcolor=1 (red)   |
| white      | color=7 or bcolor=7 (white)   |
| yellow     | color=3 or bcolor=3 (yellow)   |

**shorthands for text style**

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

**shorthands for fill and align**

The following shorthands are provided for text alignment:

| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| left       | align=0 (left)   |
| right      | align=1 (right)   |
| center     | align=2 (center)   |
| internal   | align=3 (internal)   |

You can also use the following format to specift a fill-and-align format:<br>
:\[\[\[*start*]*fill*]*align*]\[*width*]\[*end*]

The *start* is an optional character put at the very beginning of the aligned and filled text, and the *end* is another optional character put at the end.

The *width* is an integer for the minimum number of column width required for the output in this format. If the content exceeds the width, nothing is trimmed. Note that the width is not necessarily the number of characters because some characters may occupy two column spaces.

The *fill* is an optional character to fill the space required for the minimum width. If the fill character is not given, space is assumed. If the fill character occupies two column spaces and the number of column spaces is an odd number, a space will be used between the filling and the text.

The *align* is a single character that gives the alignment to use. It can have any of the values <, >, ^, or =. The meaning of these is as follows:
* \< – The text is left-justified in the field width. This is the default alignment.
* \> – The text is right-justified in the field width.
* ^ – The text is centered in the field width. Any filling will be distributed evenly on the left and right sides of the value. If an odd number of padding characters is needed, the extra one will always be on the right.

If the first character is immediately followed by one of the alignment characters, that first character is treated as the fill character to use. The fill and align values only make sense if you also specify a width value, although it is not an error to specify them without width.

Here are example of using different fill-and-align format to print a number 1.23:
```
format     output
─────────────────────────────────
:*<10      1.23******
:*>10      ******1.23
:*^10      ***1.23***
:|<10|     |1.23      |
:|>10|     |      1.23|
:|^10|     |   1.23   |
```
The start and the end characters are not counted in width.

**shorthands for numeric values**

The following shorthands are provided for text numeric values:

| shorthand  | equivalents                                                        |
|:---------- |:------------------------------------------------------------------ |
| dec        | base=0 (decimal)      |
| hex        | base=1 (hexadecimal)  |
| oct        | base=2 (octal)        |
| bin        | base=3 (binay)        |
| fopt       | fbase=0 (optimized format for floating point number)      |
| fixed      | fbase=1 (fixed point format)  |
| sci        | fbase=2 (scientific)        |
| %          | fbase=3 (percentage)        |

You can also use the following format to specift a fill-and-align format:<br>
:\[*sign*]*#*]\[*0*]\[*width*]\[*prec*]\[*L*]\[*type*]

The sign value specifies how the sign for an numeric value is to be printed. It can take the following options:

* + – A sign should always be printed for both negative and non-negative values.
* - – A sign should only be printed for negative values. This is the default.
* (space) – A sign should be printed for negative values, and a space for non-negative values.

The # character, if used, causes a decimal point character to always be printed, even if there is no fractional part. This does not apply to infinity and NaN values.

The 0 character is only valid when also specifying a width value. If present it pads the field with 0 characters after any sign character and/or base indicator. The width value is used to give the minimum width for a numeric value. If the output value needs more characters than the specified width, it will be displayed in full, not truncated to the width.

The *prec* value is formed by a decimal point followed by an integer to indicate the precision. The precision value is only valid for floating-point. It is used to determine how many digits after decimals should be printed.

The L character, when present, indicates that the value should be printed in a locale-aware format, for instance, using comma as decimal point of German locale. 

The *type* character gives differnt floating-point presentation as given below.

* e – Prints the value in scientific notation. If no prec value is given, it defaults to 6.
* f – Prints the value in fixed-point notation. If no prec value is given, it defaults to 6.
* g – Prints the value in optimized way, which picks between e and f form depending on which ofrm is the shortest.
* a – Prints the value using scientific notation, but with the number represented in hexadecimal. Because e is a valid hex digit, the exponent is indicated with a p character.
