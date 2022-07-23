# Using External Classes

An ALT application can be broken into many class/object files in different folders. Each file contains a class or an object and has the extension `.alt`. Here is an example of the application folder:
```
📂class
   📃AppSettings.alt
   📃DefaultConfig.alt
   📃CommonOpts.alt
📂DesignTab
   📂class
      📃DesignSettings.alt
      📃DesignToolset.alt
   📃DesignTab.alt
📂PreviewTab
   📂class
      📃PreviewSettings.alt
      📃PreviewPanel.alt
   📃PreviewTab.alt
📃MainPage.alt
```
When we start an ALT application with a starting file, say, `MainPage.alt`, we need a way to access external classes in separate files.

## External Class Expression

An external class expression can be written in one of the following two methods: an expression with alias name:
```altscript
AliasClassName at "FilePathToCLassFile"
```
or an expression without an alias name:
```altscript
class at "FilePathToCLassFile"
```
In the given file path, the special character `%` denotes the folder which contains the starting file of the application. Let's say the file `DesignTab.alt' contains a `DesignTab` class, and the file `DesignSettings.alt' contains a `DesignSettings` class. The `MainPage` contained in the `MainPage.alt` can have the following external class expression to refer to `DesignTab` and `DesignSettings`:
```altscript
object MainPage
{
    object MyDesignTag: class at "%DesignTab/DesignTab" { }
    object MyDesignSettings: class at "%DesignTab/class/DesignSettings" { }
}
```
If we want to use an external classes multiple times, we can give an alias name in the external class expression the first time we use it, and afterwords, give the alias only:
```altscript
object MainPage
{
    object MyDesignTag1: MyDesignTab at "%DesignTab/DesignTab" { }
    object MyDesignTag2: MyDesignTab { }
    object MyDesignTag3: MyDesignTab { }
}
```
If the given alias is the same as the file name, we can omit the file name in the file path:
```altscript
object MainPage
{
    object MyDesignTag1: DesignTab at "%DesignTab/" { }
    object MyDesignTag2: DesignTab { }
    object MyDesignTag3: DesignTab { }
}
```
Note, the alias name for an external class file must be consistent. You cannot given different alias names for the same class file:
```altscript
object MainPage
{
    object MyDesignTag1: MyDesignTab1 at "%DesignTab/DesignTab" { }
    object MyDesignTag2: MyDesignTab2 at "%DesignTab/DesignTab" { }  // Error: inconsistent alias
}
```
If you have to give different alias names for the same external class, you can use type definition:
```altscript
object MainPage
{
    type MyDesignTab1 = class at "%DesignTab/DesignTab";
    type MyDesignTab2 = class at "%DesignTab/DesignTab";
    object MyDesignTag1: MyDesignTab1 { }
    object MyDesignTag2: MyDesignTab2 { }
}
```
