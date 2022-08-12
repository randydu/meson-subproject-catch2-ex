# Catch2 Extension

This library provides some useful message macros and callback utilities for catch2 based unit tests.

## Dependency
catch2

## Compiler Compatibility

C++17

## Macros

```c++

#include <catch_macros.h>

CHECK_MESSAGE(do_check(), "error found");
REQUIRE_MESSAGE(must_be_true(), "fatal error found");
WARN_MESSAGE("This is a warning.");
ERROR_MESSAGE("Error found");

```

To run some code when the whole test executive starts or ends:

```c++
#include <catch_ex.h>

AUTO_START({
   std::cout << "Test begins!...";
});

AUTO_EXIT({
   std::cout << "Test end!";
});
```

## Callbacks

A callback function is called when an unit test starts or ends, it can be used to implement global or tag based unit test fixtures.


In the following example, we define a global test callback for the whole unit test instance:

```c++
#include <catch_ex.h>

void test_callback(bool start)
{
    if (start) {
        std::cout << "test start >>" << std::endl;
    } else {
        std::cout << "test ended <<" << std::endl;
    }
}

//Register a global callback for the whole test
REGISTER_TEST_CALLBACK(test_callback);
```

We can also define callback for unit tests with a tag or multiple tags:

```c++
//will be called only once (shared-callback) for all tests with [tag1]
void mycallback1(bool start, const std::string& tag)
{
    if (start) {
        std::cout << "mycallback1: tag (" << tag.c_str() << ") >>" << std::endl;
    } else {
        std::cout << "mycallback1: tag (" << tag.c_str() << ") <<" << std::endl;
    }
}

//not-shared callback, will be invoked for each execution of all associated unit tests
void mycallback2(bool start, const std::string& tag)
{
    if (start) {
        std::cout << "mycallback2: tag (" << tag.c_str() << ") >>" << std::endl;
    } else {
        std::cout << "mycallback2: tag (" << tag.c_str() << ") <<" << std::endl;
    }
}

//shared callback, only be invoked once for all associated unit tests
void mycallback3(bool start, const std::string& tag){}

TEST_CASE("test-a1", "[tag1]"){} //will invoke mycallback1 upon execution
TEST_CASE("test-a2", "[tag1]"){} //will invoke mycallback1 upon execution

TEST_CASE("test-b", "[tag2]"){} //will invoke mycallback2 upon execution
TEST_CASE("test-c", "[tag3]"){} //will invoke mycallback2 upon execution

///////// Callback Registration //////////////////////
REGISTER_SHARED_TAG_CALLBACK("tag1", mycallback1);
//not-shared callback, mycallback2 called for both test-b & test-c.
REGISTER_TAG_CALLBACK("tag2, tag3", mycallback2);
//shared callback, mycallback3 only called once.
REGISTER_SHARED_TAG_CALLBACK("[tag2], [tag3]", mycallback3);
```

To show the process of callback executions, you can enable detailed printing as follows:

```c++
ENABLE_VERBOSE_PRINT;
```

