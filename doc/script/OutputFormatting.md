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

A **formatter** is the specification of a printing format. It provides support for layout arrangement and alignment, common formats for numeric, string, and date/time data,  locale-specific output, and various text styles such as colors, underline, and boldness. A formatter is created by a formatter type `fmt` which has a set of members for various formats described as below.

### Formats for Apperance

Formatts for apparance is suppoted only when the printed media support tetxt colors, fonts, and decorations. 

| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| fg        |  0~255  | text color, see [ANSI escape color code assignment] (https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) |
| bg        |  0~255  | background color, see [ANSI escape color code assignment] (https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) |
| font      |  0~9    | 0 for primary (default) font, 1~9 for alternative fonts        |
| intense   |  0~3    | normal(0)  bold(1), faint(2), gothic(3)                        |
| italic    |  0~1    | no italic(0)  italic(1)                      |
| underline |  0~2    | no underline(0)  underline(1), double underline(2)             |
| overline  |  0~1    | no overline(0)  overline(1)                      |
| strike    |  0~1    | no strike through(0)  strike through(1)                      |
| reverse   |  0~1    | reverse text and background colors: no reverse(0)  reverse(1)      |
| conceal   |  0~1    | hidden text: no conceal(0)  conceal(1)      |

### Formats for Numbers
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| sign      |  0~2    | show negative sign only(0)  show positive and negative sign(1), show positive sign in a space(2)|
| locale    |  0~1    | no locale awareness(0)  locale awareness (1)                    |
| trim      |  0~1    | trim leading and trailing zeros: no trim(0)  trim(1)            |
| intbase   |  0~3    | decimal(0), hexadecimal(1), octal(2), binay(3)             |
| base      |  0~3    | integer base: decimal(0), hexadecimal(1), octal(2), binay(3)             |
| fbase     |  0~4    | floating number base: decimal(0), decimal(1), scientific(2), percentage(3), hexadecimal(4)             |
| precision |  int    | precision for printing floating number   |
| uppercase |  0~1    | no using uppercase(0)  using uppercase(1)                 |

### Formats for alignment
| name      | values  | Description                                                         |
|:--------- |:------- |:-------------------------------------------------------------- |
| fill      |  char   | filling character in alignment             |
| start     |  char   | starting character in alignment            |
| end       |  char   | ending character in alignment            |
| width     |  int    | width (number of screen columns) of alignment            |
| align     |  0~3    | alignment: left(0), right(1), center(2), internal(3)           |





