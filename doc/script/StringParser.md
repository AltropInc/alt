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
    func skipc(num: int=1);
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

The constructor of the parser, `parser (str: string; is_file: bool=false)`, takes a string to be parsed, or a text file name given by the `str` input if `is_file` is set to true. The member functions of `parser` is explained as below.
* **func getc(): char;** It returns the character at the current position and advance the position to the next character.
* **func geti(base: int): long;** It returns the integer value at the current position and advance the position to the next character after the string that represents the parsed integer value.
* 
