# Locale

The **locale** class provides internationalization services for locale-sensitive operations in a specific geographical or cultural region. For example, displaying a number is a locale-sensitive operation in which the number should be formatted according to the customs and conventions of a specific native country or region; and the text displayed in a graphic user interface is a locale-sensitive operation in which the text should be printed in a specific native language.

## Locale Identification

A locale for a specific geographical or cultural region is identified by the language code and the region code. For example: ("en", "US") represents a locale for (English, The United States) and ("en", "GB") represents a locale for (English, United Kingdom). Here "en" is the language code for English, "US" is the region code for The United States, and "GB" is the region code for The United Kingdom.

For a list of language codes, see [ISO 639 alpha-2 or alpha-3 language code](https://en.wikipedia.org/wiki/ISO_639). For a list of language codes, see [ISO 3166-1 countries and their subdivisions code](https://en.wikipedia.org/wiki/ISO_3166-1). When a language has both an alpha-2 code and an alpha-3 code, the alpha-2 code must be used. The actual lists of supported language and region codes are system dependent

For convenience and readability, you can use full name or even native names for some common languages and regions. Example: ("French", "Switzerland") and ("Français", "Suisse") both represent a locale of ("fr", "CH"), and ("Chinese", "Taiwan") and ("中文", "台灣") both represent a locale of ("zh", "TW"). Internally, full names will be translated into codes to find the related locale. This translation is defined in the file "LocaleList.jason" in the "locale" folder distributed with your executable which is built on the alt library.


## Facet

A facet provides a common place for a set of aspects (punctuations, date format, time format, and dictionary) that can be shared by a set of locales. For instance, ("zh", "CN") and ("zh", "SG") both use Simplified Chinese, therefore, they can share most of the aspects in [("zh", "Hans")](https://www.localeplanet.com/icu/zh-Hans/index.html), where "Hans" represents a facet "Simplified Chinese".

## Set Application Locale

The function `set_app_locale` provided in the locale class is typically called during the application start to set the appropriate locale for the entire application:
```altro
func set_app_locale(language: string; region: string=null);
```
This function will not only affect all locale-sensitive operations to be called in future, but will also affect the existing locale-sensitive objects such as text in shown buttons and menus, locale-sensitive images and icons in the graphic user interface.

Here are some examples to call set_app_locale:
```altro
set_app_locale("de");
set_app_locale("de", "DE");
set_app_locale("German", "Germany");
set_app_locale("Deutsch", "Österreich");
set_app_locale("en:English:English:LTR", "LC:Saint Lucia:Saint Lucia:001");
```
The last call is used only when you want to add a new locale that is not provided in the existing system. For the language string, you can have:<br>
&nbsp;&nbsp;&nbsp;&nbsp;*language-code*:*language-name*:*language-native-name*:*language-orientation*<sub>opt</sub><br>
And for the region string, you can have:<br>
&nbsp;&nbsp;&nbsp;&nbsp;*region-code*:*region-name*:*region-native-name*:*facet-name*<sub>opt</sub><br>

Altro will perform localization for the following locale-sensitive operations:
* string compare, hash, and transform
* money format
* number format
* time and date format

Application developer can provide localization by providing facet files in the application's "locale" folder for the following:
* language specific punctuations
* more specific time and date formats
* graphic item translations

To provide language specific punctuations and more specific time and date formats, add a facet jason file in the application's "locale" folder. For example, to add a ("zh", "Hans") facet, put the following "zh_Hans.json" file under the application's "locale/zh_Hans/" folder:
```json
{
    "id": "zh_Hans",
    "dialect": "简体中文",
    "name": "Chinese",
    "punctuations": {
        "comma": "，",
        "question": "？",
        "exclamation": "！",
        "semicolon": "；",
        "colon": "：",
        "left parenthesis": "（" ,
        "right parenthesis": "）",
        "left bracket": "［" ,
        "right bracket": "］",
        "left black bracket": "【" ,
        "right black bracket": "】",
        "left quotation mark": "「" ,
        "right quotation mark": "」",
        "left double quotation mark": "『" ,
        "right double quotation mark": "』",
        "top quotation mark": "﹁" ,
        "bottom quotation mark": "﹂",
        "full stop": "。",
        "enumeration": "、",
        "ellipsis": "……",
        "middle dot": "‧",
        "em dash": "——",
        "en dash": "—",
        "wavy dash": "～",
        "open mark": "《",
        "close mark": "》",
        "open inner mark": "〈",
        "close inner mark": "〉"
    },
    "date format": {
        "element order": "eymdw",
        "long format": "y年m月d日",
        "short format": "y/m/d",
        "AD": "公元",
        "BC": "公元前",
        "short months": ["1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月"],
        "long months": ["一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"],
        "short weekdays": ["周日", "周一", "周二", "周三", "周四", "周五", "周六"],
        "long weekdays": ["星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"]
    },
    "time format": {
        "element order": "hms",
        "long format": "h点m分d秒",
        "short format": "h:m:s",
        "AM": "上午",
        "PM": "下午"
    }
}
```
To provide graphic text item translations, add a translation dictionary file in the application's "locale" folder. For example, to add text translation for the ("zh", "Hans") facet, put the "zh_Hans_dic.txt" file under the application's "locale/zh_Hans/" folder:
```
MenuNew:        "新建";
MenuOpen:       "打开";
MenuReload:     "恢复";
MenuClose:      "关闭";
MenuCloseAll:   "关闭所有";
MenuSave:       "保存";
MenuSaveAs:     "另存为";
MenuSaveCopy:   "保存副本";
MenuSaveAll:    "保存所有";
...
```
The above translation will be used by all regions for Simplified Chinese. If there is any difference between two regions that shares the same facet, an override facet can be placed in the region subfolder, for example, a translation dictionary "zh_SG_dic.txt" can be put in "locale/zh_SG/" folder for only items that need to be translated in a different way for Singapore reagion.

All locale-sensitive sound, images, and icons should also be placed in the appropriate subfolders in the application "locale' folder. A locale package is typically distributed separately with the application itself.







