# `sys` Library Class

The `sys` library class of Altro contains a set of useful constants, classes, and functions to manipulate various parts of the runtime environment. It provides facilities like standard input, output, error reporting, debugging, abd accessing to features in the underlying operating system.

The `sys` class is an incorporeal class. All constants and functions are defined in class scope, that is, you access these constants and functions directly using the class name, for instance, `sys.HOME_PATH`, `sys.getline()`, `sys.print(msg)`, etc.

## Constants Defined in `sys` Library

The `sys` class provides the following meta constants:

| name           | type   | description                                                           |
|:------------------- |:------ |:---------------------------------------------------------------- |
| CPU_COUNT           | int    | number of cpu cores in the system |
| PAGE_SIZE           | int    | number of cpu cores in the system |
| CHCHE_LINE_SIZE     | int    | number of cpu cores in the system |
| OS_TYPE             | string | the operating system name, e.g. andriod, linux, mac, win, cygwin |
| OS_MAJOR            | int    | major version of the operating system |
| OS_MINOR            | int    | minor version of the operating system |
| UNDERLYING_OS_TYPE  | string | the underlying operating system, e.g. andriod, linux, mac, win |
| UNDERLYING_OS_MAJOR | int    | major version of the underlying operating system |
| UNDERLYING_OS_MINOR | int    | minor version of the underlying operating system |
| ALT_CORE_VERSION    | double | the version of the execuatable used to start this application |
| SYS_NAME            | string | the name of the execuatable used to start this application |
| APP_NAME            | string | the file name  of the application starting file|
| USER_NAME           | string | the current user name |
| APP_PATH            | string | the path where the application starting file is loaded |
| SYS_PATH            | string | the system path |
| DATA_PATH           | string | the data path for applications |
| HOME_PATH           | string | the home path of the current user |
| CURRENT_PATH        | string | the path where the execuatable that starts this applications |
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
| [parser](StringParser.md)         | string or text file parser |

## Functions Defined in `sys` Library

The `sys` library provides the following functions:

**functions to get application formation**

* **func argv():string...;** It returns argv a string stream where argv[0] is the name of the program being executed, argv[1] is the first parameter supplied on the command line, and argv[2] is the second parameter supplied on the command line, and so forth. 
* **func app_lang():string;** It returns the language name currently used by the application, or a null string if the application language is not set.
* **func app_region():string;** It returns the region name currently used by the application, , or a null string if the application region is not set.
* **func locale_info(lang:string; region:string=null):string;** It returns the locale information of the given language and region. For instance, `locale_info("zh", "TW")`, `locale_info("Chinese", "Taiwan")`, `locale_info("中文", "台灣")`, all three calls returns `"Chinese (中文), Taiwan (台灣)"`.


