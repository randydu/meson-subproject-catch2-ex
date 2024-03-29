#include "catch_ex.h"
#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>


/* Generate demo outputs for readne.md

#include <catch_macros.h>

bool do_check() { return false; }
bool must_be_true() { return false; }

TEST_CASE("foo-1", "[bar]")
{
    WARN_MESSAGE("This is a warning.");
    ERROR_MESSAGE("Error found");
    std::cout << "Never be here!";
}

TEST_CASE("foo-2", "[bar]")
{
    CHECK_MESSAGE(do_check(), "check error found");
    REQUIRE_MESSAGE(must_be_true(), "fatal error found");
}
*/

void test_callback(bool start)
{
    if (start) {
        std::cout << "test start >>" << std::endl;
    } else {
        std::cout << "test ended <<" << std::endl;
    }
}

void mycallback1(bool start, const std::string& tag)
{
    if (start) {
        std::cout << "mycallback1: tag (" << tag << ") >>" << std::endl;
    } else {
        std::cout << "mycallback1: tag (" << tag << ") <<" << std::endl;
    }
}

void mycallback2(bool start, const std::string& tag)
{
    if (start) {
        std::cout << "mycallback2: tag (" << tag << ") >>" << std::endl;
    } else {
        std::cout << "mycallback2: tag (" << tag << ") <<" << std::endl;
    }
}

void mycallback3(bool start, const std::string& tag)
{
    if (start) {
        std::cout << "mycallback3: tag (" << tag << ") >>" << std::endl;
    } else {
        std::cout << "mycallback3: tag (" << tag << ") <<" << std::endl;
    }
}
AUTO_START({
    catch_ex::register_test_callback(test_callback);
    catch_ex::register_tag_callback("tag1", false, mycallback1);
    catch_ex::register_tag_callback("[tag2]", false, mycallback2);
    catch_ex::register_tag_callback("tag1, tag2", true, mycallback3);
});

TEST_CASE("test-tag1-0", "[tag1]")
{
}

TEST_CASE("test-tag1-1", "[tag1]")
{
}

TEST_CASE("test-tag1-tag2-0", "[tag1], [tag2]")
{
}

TEST_CASE("test-tag2-0", "[tag2]")
{
}

TEST_CASE("parse_tags", "[catch_ex]")
{
    using namespace catch_ex;

    CHECK_THAT(parse_tags("xxx"), Catch::Matchers::Equals(std::vector<std::string>{"xxx"}));
    CHECK_THAT(parse_tags("xxx  "), Catch::Matchers::Equals(std::vector<std::string>{"xxx"}));
    CHECK_THAT(parse_tags("  xxx"), Catch::Matchers::Equals(std::vector<std::string>{"xxx"}));
    CHECK_THAT(parse_tags("   xxx   "), Catch::Matchers::Equals(std::vector<std::string>{"xxx"}));
    CHECK_THAT(parse_tags("[xxx]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx"}));
    CHECK_THAT(parse_tags("xxx,yyy"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags(" xxx, yyy"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags(" xxx,, yyy"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags(" [xxx],, yyy"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags(" [xxx], [ yyy ]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags("[xxx], [ yyy ],"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags("[xxx], [ yyy ],[]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags("[xxx], [ yyy ],[ ]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags("[xxx, yyy , ]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
    CHECK_THAT(parse_tags("[xxx, [yyy] , ]"), Catch::Matchers::Equals(std::vector<std::string>{"xxx", "yyy"}));
}

TEST_CASE("parse_tag", "[catch_ex]")
{
    using namespace catch_ex;

    CHECK(parse_first_tag("xxx") == "xxx");
    CHECK(parse_first_tag(" xxx ") == "xxx");
    CHECK(parse_first_tag("[xxx]") == "xxx");
    CHECK(parse_first_tag("[xxx, ]") == "xxx");
    CHECK(parse_first_tag("[xxx, ], yyy") == "xxx");
}

TEST_CASE("same_tag", "[catch_ex]")
{
    using namespace catch_ex;

    CHECK(is_same_tag("xxx", "xxx"));
    CHECK(is_same_tag("xxx", "[xxx]"));
    CHECK(is_same_tag("[xxx]", "xxx"));
    CHECK(is_same_tag(" xxx ", "xxx"));
    CHECK(is_same_tag("[xxx, ]", "xxx"));
    CHECK(is_same_tag("[xxx, ], yyy", "xxx"));
}