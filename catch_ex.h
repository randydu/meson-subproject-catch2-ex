#ifndef CATCH_EX_H_INCLUDED
#define CATCH_EX_H_INCLUDED

#include <functional>

namespace catch_ex {
    typedef std::function<void(bool start)> test_callback_t;
    typedef std::function<void(bool start, const std::string& tag)> tag_callback_t;

    void register_test_callback(test_callback_t test_callback);
    void register_tag_callback(const std::string& tag_or_tags, tag_callback_t tag_callback, bool shared);

    void enable_verbose_printing(bool enable);
    bool verbose_printing();

    namespace {
        typedef std::function<void()> dummy_t;

        dummy_t dummy;
    }

    struct autostart {
        dummy_t f_;
        dummy_t g_;

        autostart(dummy_t&& f) : f_(std::move(f)) {
            if (f_) f_();
        }
        autostart(dummy_t&& f, dummy_t&& g) : f_(std::move(f)), g_(std::move(g)) {
            if (f_) f_();
        }
        ~autostart() {
            if (g_) g_();
        }
    };

    // for internal usage, published for unit-test.
    std::vector<std::string> parse_tags(std::string const& arg);
}

#ifndef COMBINE1
#define COMBINE1(X, Y) X##Y // helper macro
#endif

#ifndef COMBINE
#define COMBINE(X, Y) COMBINE1(X, Y)
#endif

#define AUTO_START(code_start) catch_ex::autostart COMBINE(autostart, __LINE__) ( [](){ code_start; } )

#define AUTO_EXIT(code_exit) auto COMBINE(autostart, __LINE__) = catch_ex::make_autostart(dummy, [&](){ code_exit; })

#define AUTO_START_EXIT(code_start, code_exit) auto COMBINE(autostart, __LINE__) = catch_ex::make_autostart([&](){ code_start; }, [&](){ code_exit; })

#define REGISTER_TEST_CALLBACK(code_start, code_exit) AUTO_START(catch_ex::register_test_callback([&](bool start){ if(start){ code_start; } else { code_exit; } }))

#endif