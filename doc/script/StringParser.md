# String Parser

The class `parser` is a string parser used to extract these data chunks from a string or a text file. The class `parser` is defined in the `sys` class as below:
```altro
data class paser
{
    parser (str: string; is_file: bool=false);
    func getc(): char;
    func skipc(num: int=1);
    func skipws();
    func peer(): char;
    func peernext(): char;
    func getint(base: int): long;
    func getnum(): double;
    func getstr(): string;
    func scan(to: char): string;
    func scan(to: string): string;
    func scanto(to: char...): string;
    func pos(): (int; int);
    func done(): bool;
    func is_valid(): bool; 
}
```
