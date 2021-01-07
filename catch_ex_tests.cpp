#include <catch2/catch.hpp>
#include "catch_ex.h"

#include <iostream>
#include <sstream>

void test_callback(bool start) {
    if (start) {
        std::cout << "test start >>" << std::endl;
    }
    else {
        std::cout << "test ended <<" << std::endl;
    }
}

void mycallback1(bool start, const std::string& tag) {
    if (start) {
        std::cout << "mycallback1: tag (" << tag.c_str() << ") >>" << std::endl;
    }
    else {
        std::cout << "mycallback1: tag (" << tag.c_str() << ") <<" << std::endl;
    }
}

void mycallback2(bool start, const std::string& tag) {
    if (start) {
        std::cout << "mycallback2: tag (" << tag.c_str() << ") >>" << std::endl;
    }
    else {
        std::cout << "mycallback2: tag (" << tag.c_str() << ") <<" << std::endl;
    }
}

void mycallback3(bool start, const std::string& tag) {
    if (start) {
        std::cout << "mycallback3: tag (" << tag.c_str() << ") >>" << std::endl;
    }
    else {
        std::cout << "mycallback3: tag (" << tag.c_str() << ") <<" << std::endl;
    }
}
AUTO_START(
    catch_ex::register_test_callback(test_callback);
    catch_ex::register_tag_callback("tag1", mycallback1, false);
    catch_ex::register_tag_callback("[tag2]", mycallback2, false);
    catch_ex::register_tag_callback("tag1, tag2", mycallback3, true);
);

TEST_CASE("test-tag1-0", "[tag1]") {
}

TEST_CASE("test-tag1-1", "[tag1]") {
}

TEST_CASE("test-tag1-tag2-0", "[tag1], [tag2]") {
}

TEST_CASE("test-tag2-0", "[tag2]") {
}

TEST_CASE("parse_tags", "[catch_ex]") {
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