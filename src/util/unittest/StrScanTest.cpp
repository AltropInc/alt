#include <util/types/Enum.h>
#include <util/types/EnumSet.h>
#include <util/string/StrScan.h>
#include <util/string/StreamParser.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <cstring>
#include <locale>
#include <iconv.h>
//#include <util/system/OS.h>

#include <w32api/winnls.h>

template<typename T>
T testValue(const char* val_str)
{
    alt::StrParser sp (val_str);
    T res;
    sp >> res;
    return res;
}

namespace alt {
ENUM(Day, uint8_t, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday);
using Days = EnumSet<Day>;
} // namespace alt

BOOL LocaleEnumprocex(
  LPWSTR Arg1,
  DWORD Arg2,
  LPARAM Arg3
)
{
    std::wstring ws( Arg1 ); 
    std::wcout << ws << std::endl;
    return true;
}

TEST_CASE( "Scan Numbers", "[StrParser]" ) {
    //std::cout << alt::getAppDataPath(true) <<std::endl;
    //std::cout << alt::getAppDataPath(false) <<std::endl;
    /*char src[] = "abcčde";
    char dst[100];
    size_t srclen = 6;
    size_t dstlen = 12;
    char * pIn = src;
    char * pOut = ( char*)dst;
    iconv_t conv = iconv_open("UTF-8", "CP1250");
    iconv(conv, &pIn, &srclen, &pOut, &dstlen);
    iconv_close(conv);
    fprintf(stderr,"out: %s\n",dst);*/
    /*
    try
    {
        std::locale my_default_loc("en_US.utf8");
        std::cout << "1 " << my_default_loc.name() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
    try
    {
        std::locale my_default_loc("en_US.UTF-8");
        std::cout << "2 " << my_default_loc.name() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
    */
    //LPARAM lParam; LPVOID lpReserved;
    //EnumSystemLocalesEx(LocaleEnumprocex,LOCALE_ALL,0,nullptr);

    //std::cout << "Default locale =" << setlocale(LC_ALL,"zh_CN.UTF-8") << std::endl;
    //std::cout << "Current locale =" << setlocale(LC_ALL,nullptr) << std::endl;
    //std::cout << "Currency symbol =" << localeconv()->int_curr_symbol << std::endl;
    //std::cout << "Sarah (שרה) is spelled" <<  std::endl;

    {
        int res = testValue<int>("1234");
        REQUIRE(res == 1234);
    }
    {
        int res = testValue<int>("0XEF345A");
        REQUIRE(res == 0XEF345A);         
    }
    {
        int res = testValue<int>("06534");
        REQUIRE(res == 06534);         
    }
    {
        int res = testValue<int>("-06534");
        REQUIRE(res == -06534);         
    }
    {
        double res = testValue<double>("12.625");
        REQUIRE(res == 12.625);        
    }
    {
        double res = testValue<double>("1.625e+12");
        REQUIRE(res == 1.625e+12);        
    }
    {
        double res = testValue<double>("1.625e-12");
        REQUIRE(res == 1.625e-12);        
    }
    {
        double res = testValue<double>("-1.625e-12");
        REQUIRE(res == -1.625e-12);        
    }
    {
        double res = testValue<double>("1.625e+0");
        REQUIRE(res == 1.625e+0);        
    }
    {
        double res = testValue<double>("-0.3125");
        REQUIRE(res == -0.3125);        
    }
    {
        double res = testValue<double>("-20.078125");
        REQUIRE(res == -20.078125);        
    }
    {
        double res = testValue<double>("Infinity");
        REQUIRE(res == std::numeric_limits<double>::max());        
    }
    {
        double res = testValue<double>("-Infinity");
        REQUIRE(res == std::numeric_limits<double>::min());        
    }
}

TEST_CASE( "Scan Enum", "[StrParser]" ) {
    {
        std::string str("Monday,Tuesday,Friday");
        alt::StrParser sp (str.c_str());
        alt::Day res1, res2, res3;
        sp >> res1 >> res2 >> res3;
        REQUIRE (res1==alt::Day::Monday);
        REQUIRE (res2==alt::Day::Tuesday);
        REQUIRE (res3==alt::Day::Friday);
    }
}

TEST_CASE( "Scan String", "[StrParser]" ) {
    {
        std::string str("Monday,Tuesday,Friday");
        std::vector<std::string> res = testValue<std::vector<std::string>>(str.c_str());
        REQUIRE (res.size()==3);
        REQUIRE (res[0]=="Monday");
        REQUIRE (res[1]=="Tuesday");
        REQUIRE (res[2]=="Friday");
    }
    {
        std::string str("Monday,Tuesday,Friday");
        std::string res1, res2, res3;
        alt::StrParser sp (str.c_str());
        sp >> res1 >> res2 >> res3;
        REQUIRE (res1=="Monday");
        REQUIRE (res2=="Tuesday");
        REQUIRE (res3=="Friday");
    }
    {
        char source[64];
        std::strcpy(source,"Monday,Tuesday,Friday");
        alt::StrRef res1, res2, res3;
        alt::StrParser sp (source);
        // StrRef points to source directly, must clear spearator to get terminator of the string
        sp.setClearSeparator(true);
        sp >> res1 >> res2 >> res3;
        REQUIRE (res1=="Monday");
        REQUIRE (res2=="Tuesday");
        REQUIRE (res3=="Friday");
    }
    {
        std::string str("Monday,Tuesday,Friday");
        alt::StrRefInLength res1, res2, res3;
        alt::StrParser sp (str.c_str());
        sp >> res1 >> res2 >> res3;
        REQUIRE (res1=="Monday");
        REQUIRE (res2=="Tuesday");
        REQUIRE (res3=="Friday");
    }
}

TEST_CASE( "Scan Char", "[StrParser]" ) {
    {
        char res = testValue<char>("c");
        REQUIRE(res == 'c');  
    }
    {
        // scan plain character without decoding any '\' format
        char res = testValue<char>("\\n");
        REQUIRE(res == '\\');  
    }
    {
        alt_char_t res = testValue<alt_char_t>("c");
        REQUIRE(res == U'c');  
    }
    {
        alt_char_t res = testValue<alt_char_t>("\\n");
        REQUIRE(res == U'\n');  
    }
    {
        alt_char_t res = testValue<alt_char_t>("\\x2A");
        REQUIRE(res == U'*');  
    }
    {
        alt_char_t res = testValue<alt_char_t>("\\x2A");
        REQUIRE(res == U'*');  
    }
    {
        // UFT-16 hex
        alt_char_t res = testValue<alt_char_t>("\\X03A0");
        REQUIRE(res == U'Π');  
    }
    {
        // UFT-16 hex
        alt_char_t res = testValue<alt_char_t>("\\u03A0");
        REQUIRE(res == U'Π'); 
    }
    {
        // UFT-32 Decimal
        alt_char_t res = testValue<alt_char_t>("\\#928;");
        REQUIRE(res == U'Π'); 
    }
    {
        alt_char_t res = testValue<alt_char_t>("\xCF\x80");
        REQUIRE(res == U'π'); 
    }
    {
        alt_char_t res = testValue<alt_char_t>("\xC2\xA2");
        REQUIRE(res == U'¢'); 
    }
    {
        // for little endian, we'll read A2 firist, then 00
        alt_char_t res = testValue<alt_char_t>("\u00A2");
        REQUIRE(res == U'¢');  
    }
    {
        // UFT8 Encode in string
        alt_char_t res = testValue<alt_char_t>("\xE3\x81\x84");
        REQUIRE(res == U'い');  
    }
    {
        // UFT-32 Hex
        alt_char_t res = testValue<alt_char_t>("\\U3044");
        REQUIRE(res == U'い');  
    }
    {
        // UFT8 Encode in string
        alt_char_t res = testValue<alt_char_t>("\xF0\x93\x80\x80");
        REQUIRE(res == U'𓀀');  
    }
    {
        // UFT-32 Hex
        alt_char_t res = testValue<alt_char_t>("\\U13000 ");
        REQUIRE(res == U'𓀀');  
    }
    {
        // UFT-32 Decimal
        alt_char_t res = testValue<alt_char_t>("\\#13719;");
        REQUIRE(res == U'㖗');  
    }
}

#if 0
TEST_CASE( "Scan Token", "[TFLangParser]" ) {
    {
        alt::ParserStreamContext context;
        std::string str("identifier1 いろは π4");
        alt::TFLangParser tf_parser (context, str);
        std::vector<std::string> identifiers;
        while (!tf_parser.done())
        {
            alt::Token tk = tf_parser.getToken();
            if (tk==alt::Token::Identifier)
            {
                identifiers.emplace_back(tf_parser.scannedStart(), tf_parser.scannedLength());
            }
        }
        REQUIRE(identifiers.size() == 3); 
        REQUIRE(identifiers[0]=="identifier1");
        REQUIRE(identifiers[1]=="いろは");
        REQUIRE(identifiers[2]=="π4");
    }
    {
        std::string str("identifier1:=\"いろは π4\"+.4");
        alt::ParserStreamContext context;
        alt::TFLangParser tf_parser (context, str);

        REQUIRE(tf_parser.getToken()==alt::Token::Identifier);
        REQUIRE(tf_parser.scanned<11>("identifier1"));

        REQUIRE(tf_parser.getToken()==alt::Token::Operator);
        REQUIRE(tf_parser.scanned<2>(":="));

        REQUIRE(tf_parser.getToken()==alt::Token::Literal);
        REQUIRE(tf_parser.scannedValueType()==alt::StrScan::STRING);
        REQUIRE(tf_parser.getScannedString()=="いろは π4");

        REQUIRE(tf_parser.getToken()==alt::Token::Operator);
        REQUIRE(tf_parser.scanned('+'));

        REQUIRE(tf_parser.getToken()==alt::Token::Literal);
        REQUIRE(tf_parser.scannedValueType()==alt::StrScan::DOUBLE);
        REQUIRE(tf_parser.getScannedDouble()==0.4); 
    }
    {
        std::stringstream str("π:double=3.14;\n ch𓀀:='\\#13719;'");
        alt::ParserStreamContext context(256);
        alt::TFLangParser tf_parser (context, &str);

        REQUIRE(tf_parser.getToken()==alt::Token::Identifier);
        REQUIRE(tf_parser.scanned<2>("π"));

        REQUIRE(tf_parser.getToken()==alt::Token::Operator);
        REQUIRE(tf_parser.scanned(':'));

        REQUIRE(tf_parser.getToken()==alt::Token::Identifier);
        REQUIRE(tf_parser.scanned<6>("double"));

        REQUIRE(tf_parser.getToken()==alt::Token::Operator);
        REQUIRE(tf_parser.scanned('='));

        REQUIRE(tf_parser.getToken()==alt::Token::Literal);
        REQUIRE(tf_parser.scannedValueType()==alt::StrScan::DOUBLE);
        REQUIRE(tf_parser.getScannedDouble()==3.14);

        REQUIRE(tf_parser.getToken()==alt::Token::SemiColon);

        REQUIRE(tf_parser.getToken()==alt::Token::Identifier);
        REQUIRE(tf_parser.scanned<6>("ch\xF0\x93\x80\x80"));

        REQUIRE(tf_parser.getToken()==alt::Token::Operator);
        REQUIRE(tf_parser.scanned<2>(":="));

        REQUIRE(tf_parser.getToken()==alt::Token::Literal);
        REQUIRE(tf_parser.scannedValueType()==alt::StrScan::CHAR);
        REQUIRE(tf_parser.getScannedChar()==U'㖗');
    }
    {
        //std::stringstream str("/*comments*/ // line comments\n /*long long  .\n comments \n continue*/ end");
        std::stringstream str("/*comments*/ // line comments\n"
                              " /*long long  .\n"
                              " comments \n"
                              " continue /* nested\n"
                              " nest continued */ \n"" nested exit */ end");
        alt::ParserStreamContext context(256);
        alt::TFLangParser tf_parser (context, &str);

        REQUIRE(tf_parser.getToken()==alt::Token::Comment);
        std::cout << tf_parser.getScannedString() << std::endl;

        REQUIRE(tf_parser.getToken()==alt::Token::Comment);
        std::cout << tf_parser.getScannedString() << std::endl;

        REQUIRE(tf_parser.getToken()==alt::Token::Comment);
        std::cout << tf_parser.getScannedString() << std::endl;

        REQUIRE(tf_parser.getToken()==alt::Token::Identifier);
        std::string scanned;
        tf_parser.fetchScanned(scanned);
        std::cout << "SCANNED:" << scanned << std::endl;
        REQUIRE(scanned=="end");
    }

}
#endif
