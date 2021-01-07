#ifndef CATCH_MACROS_H_INCLUDED
#define CATCH_MACROS_H_INCLUDED

#include <catch2/catch.hpp>

#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK((cond)); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE((cond)); } while((void)0, 0)

#define WARN_MESSAGE(msg) do { WARN(msg); } while((void)0, 0)
#define ERROR_MESSAGE(msg) do { FAIL(msg); } while((void)0, 0)

#define CHECK_EQUAL(a, b) do { CHECK((a) == (b)); } while((void)0, 0)

//used to migrate from boost macros.
#define BOOST_CHECK_MESSAGE CHECK_MESSAGE
#define BOOST_REQUIRE_MESSAGE REQUIRE_MESSAGE
#define BOOST_CHECK_EQUAL CHECK_EQUAL



#endif