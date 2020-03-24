#define CATCH_CONFIG_MAIN

#include <util/types/Enum.h>
#include <util/types/EnumSet.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <cstring>


namespace alt {
ENUM(Digit, uint8_t, Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine);
using Digits = EnumSet64<Digit>;
} // namespace alt

TEST_CASE( "Enum Set To/From Name", "[EnumSet]" ) {
    alt::Digits digits(alt::Digit::Two, alt::Digit::Five);
    std::string str = digits.toString();
	REQUIRE(str == "(Two,Five)");
    alt::Digits digits_back = alt::Digits::fromString(str);
    REQUIRE(digits_back==digits);
}

TEST_CASE( "Enum Set Flip", "[EnumSet]" ) {
    alt::Digits evens(alt::Digit::Zero, alt::Digit::Two, alt::Digit::Four,
                       alt::Digit::Six, alt::Digit::Eight);
    alt::Digits odds(alt::Digit::One, alt::Digit::Three, alt::Digit::Five,
                       alt::Digit::Seven, alt::Digit::Nine);
	REQUIRE(evens.flip() == odds);
}

TEST_CASE( "Enum Set Count", "[EnumSet]" ) {
    alt::Digits digits(alt::Digit::Seven, alt::Digit::Nine, alt::Digit::Four);
	REQUIRE(digits.count() == 3);
}

TEST_CASE( "Enum Set Union", "[EnumSet]" ) {
    alt::Digits digit_set1(alt::Digit::Seven, alt::Digit::Nine, alt::Digit::Four);
    alt::Digits digit_set2(alt::Digit::Seven, alt::Digit::Nine, alt::Digit::Five);
	REQUIRE((digit_set1 | digit_set2).count() == 4);
    REQUIRE((digit_set1 & digit_set2).count() == 2);
}



