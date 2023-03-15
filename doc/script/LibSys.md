# `sys` Library Class

The `sys` library class of Altro contains a set of useful constants, classes, and functions to manipulate various parts of the runtime environment. It provides facilities like standard input, output, error reporting, debugging, and accessing to features in the underlying operating system.

The `sys` class is an incorporeal class. All constants and functions are defined in class scope, that is, you access these constants and functions directly using the class name, for instance, `sys.HOME_PATH`, `sys.getline()`, `sys.print(msg)`, etc.

## Constants Defined in `sys` Library

The `sys` class provides the following meta constants for system/application-specific parameters:

| name           | type   | description                                                           |
|:------------------- |:------ |:---------------------------------------------------------------- |
| CPU_COUNT           | int    | number of cpu cores in the system |
| PAGE_SIZE           | int    | number of cpu cores in the system |
| CHCHE_LINE_SIZE     | int    | number of cpu cores in the system |
| OS_TYPE             | string | the operating system name, e.g. android, linux, mac, win, cygwin |
| OS_MAJOR            | int    | major version of the operating system |
| OS_MINOR            | int    | minor version of the operating system |
| UNDERLYING_OS_TYPE  | string | the underlying operating system, e.g. android, linux, mac, win |
| UNDERLYING_OS_MAJOR | int    | major version of the underlying operating system |
| UNDERLYING_OS_MINOR | int    | minor version of the underlying operating system |
| ALT_CORE_VERSION    | double | the version of the executable used to start this application |
| SYS_NAME            | string | the name of the executable used to start this application |
| APP_NAME            | string | the file name  of the application starting file|
| USER_NAME           | string | the current user name |
| APP_PATH            | string | the path where the application starting file is loaded |
| SYS_PATH            | string | the system path |
| DATA_PATH           | string | the data path for applications |
| HOME_PATH           | string | the home path of the current user |
| CURRENT_PATH        | string | the path where the executable that starts this applications |
| DEBUG               | bool   | true if the application is running in debug mode |
| IS_TOOL             | bool   | true if the application is running in tool mode |
| EOL                 | char   | end of line used by the operating system |
| LC_MONETARY         | int    | a [locale category](Locale.md) for rules and symbols to format monetary numeric data |
| LC_TIME             | int    | a [locale category](Locale.md) for rules and symbols to format time and date |
| LC_NUMERIC          | int    | a [locale category](Locale.md) for rules and symbols to format numeric data |
| LC_COLLATE          | int    | a [locale category](Locale.md) for rules to define character and string collation  |
| LC_CTYPE            | int    | a [locale category](Locale.md) for character attributes such as case and classification  |
| LC_ALL              | int    | a constant for [all locale categories](Locale.md)  |

## Classes Defined in `sys` Library

The `sys` library provides the following classes:

| name           | description                                                           |
|:-------------- |:---------------------------------------------------------------- |
| [fmt](OutputFormatting.md)  | printing format specification |
| [parser](StringParser.md)   | string or text file parser |

## Functions Defined in `sys` Library

The `sys` library provides the following functions:

**Functions to get system information**

* **func availmem(): ulong** It returns the amount of memory the application can use.
* **func getenv(env_var: string): string** It searches the environment list provided by the host environment in the OS, for a string that matches the string given in the input and returns a string that is associated with the matched environment list member. For instance, calling `getenv("PATH")` will return a possible string: `"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games"`
* **func path(): string...** It returns a list of strings that specifies the search path for modules. For instance, calling `path()` will return a possible string list: `("/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin", "/sbin", "/bin", "/usr/games")`

**Functions to get application information**

* **func argv():string...;** It returns a string stream where argv()[0] is the name of the program being executed, argv()[1] is the first parameter supplied on the command line, and argv()[2] is the second parameter supplied on the command line, and so forth. 

**Functions for Localization**

* **func applang():string;** It returns the language name currently used by the application, or a null string if the application language is not set.
* **func appregion():string;** It returns the region name currently used by the application, , or a null string if the application region is not set.
* **func langinfo(lang:string; rgn:string=null):string;** It returns the locale information of the given language and region. For instance, `locale_info("zh", "TW")`, `locale_info("Chinese", "Taiwan")`, `locale_info("中文", "台灣")`, all three calls returns `"Chinese (中文), Taiwan (台灣)"`.
* **func setlang(lang:string; rgn:string=null);** It sets the application's default language and region. The modifications remain in effect and influence the execution of all language-sensitive functions until the next call to `setlanguage`. This function will not only affect all locale-sensitive formats such as  to be used in future, but will also affect the existing locale-sensitive objects such as date, monetary, and character case, it will also affect language-sensitive text, images and icons in display. This function is typically called during the application start to set the appropriate locale for the entire application. For details on how to set application language, see [Locale](Locale.md).
* **func setlocale(cat: int; lang:string; rgn:string=null);** It sets the locale in the given category without affecting the entire application language setting and the locale change in a specific category only affects formats in that category. The category can be LC_MONETARY, LC_TIME, LC_NUMERIC, LC_COLLATE, LC_CTYPE, or LC_ALL. see [Set Category Locale](Locale.md) for details about the usage of this function.

**Functions for String Input and Output**

* **func getline(delim: char='\n'):string;** It reads a line of characters from the standard input. This function extracts characters from the input until the delimiting character and a new line character are encountered. If the delimiting character is not given or is given as a new line character '\n', the function to stop reading further whenever a new line character is encountered. 
* **func print(any);** It prints the specified input to the standard output. The input can be a value of `any` type. Multiple input values can be wrapped in a tuple, for example, `print("π = ", 3.14)`. Output format can be inserted in the input, for example, `print([:s>40, green, bold], "Hello", [yellow], "World")`. For usage detail, see [Output Formatting](OutputFormatting.md).
* **func err(any);** It prints the specified input to the standard error output. See explanation on `print`.
* **func str(any): string;** It converts the specified input to a string. The input of this function has the same rules as the one used for `print`, except that the [Appearance formatter](OutputFormatting.md) such as color, intense, and underline cannot be used. For example, `str([:g/'], 1000000000.25)` returns the string `"1,000,000,000.25"`.
* **func \_str(any): string;** This is the same as `str`, except that the string returned from this function is locale-sensitive. For example,<pre>
`enum DayOfWeek(Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday);`
`print(_str(DayOfWeek::Thursday))`;</pre>
Here `_str(DayOfWeek::Thursday)` returns a locale-sensitive string `"Thursday"`, and when print this string, it will be translated to the text in the current application locale. The printed text can be `Donnerstag` if the current application locale is in German.

**Functions for Diagnostics**

* **func assert(assertion:bool; msg: string);** It is used to add diagnostics in your program, and this function is evaluated only when the application is running in debug mode. If `assertion` evaluates to true, assert() does nothing. If `assertion` evaluates to false, `assert` raises an `ASSERTION` exception with the given diagnostic message.
* **func abort(msg: string);** Calling `abort` raises an `ABORTION` exception with the given message.
* **func throw(e: int, msg: string);** This function raises an exception of `e` with the given message. For more information about exceptions, see [Exceptions](Exceptions.md).
* 

