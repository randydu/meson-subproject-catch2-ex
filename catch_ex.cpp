#include <algorithm>
#include <exception>
#include <map>
#include <tuple>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "catch_ex.h"

namespace catch_ex {
namespace {
using callback_handle_t = int;

class CallbackMan
{
    friend CallbackMan& man();

private:
    static CallbackMan* man_;

    test_callback_t cb_test{nullptr}; // one and only test callback.

    using tag_callback_info_t = std::tuple<tag_callback_t, bool /* shared */>;
    std::vector<tag_callback_info_t> cbi_tags;

    using tag_callbacks_t = std::map<std::string, std::vector<callback_handle_t>>;
    tag_callbacks_t cb_tags; // tag => callbacks

    bool verbose_printing_enabled_ = false;
    std::vector<callback_handle_t> no_handles;

public:
    bool verbose_printing_enabled() const
    {
        return verbose_printing_enabled_;
    }
    void verbose_printing_enabled(bool v)
    {
        verbose_printing_enabled_ = v;
    }

    void onTestStart()
    {
        if (cb_test)
            cb_test(true);
    }
    void onTestEnded()
    {
        if (cb_test) {
            cb_test(false);
        }
    }

    static void clean()
    {
        if (man_) {
            delete man_;
            man_ = nullptr;
        }
    }

    const auto& getCallbackInfo(callback_handle_t hCallback) const
    {
        return cbi_tags[hCallback];
    }

    const std::vector<callback_handle_t>& getCallbackHandles(const std::string& tag) const
    {
        auto it = cb_tags.find(tag);
        return it == cb_tags.end() ? no_handles : it->second;
    }

    void register_test_callback(test_callback_t test_callback)
    {
        if (cb_test)
            throw std::logic_error("global test callback can only be registered once!");
        cb_test = test_callback;
    }

    void register_tag_callback(const std::string& tag_or_tags, tag_callback_t tag_callback, bool shared)
    {
        cbi_tags.emplace_back(tag_callback, shared);
        callback_handle_t h = cbi_tags.size() - 1;

        for (const auto& tag : parse_tags(tag_or_tags)) {
            auto it = cb_tags.find(tag);
            if (it == cb_tags.end())
                cb_tags.emplace(tag, tag_callbacks_t::value_type::second_type{h});
            else
                it->second.push_back(h);
        }
    }
};

CallbackMan* CallbackMan::man_{nullptr};

// delayed object initialization until use.
CallbackMan& man()
{
    if (CallbackMan::man_ == nullptr)
        CallbackMan::man_ = new CallbackMan();
    return *CallbackMan::man_;
}

// helper class to support reverse-for-each in c++
template <typename T>
class reverse
{
private:
    T& iterable_;

public:
    explicit reverse(T& iterable) : iterable_{iterable} {}
    auto begin() const { return std::rbegin(iterable_); }
    auto end() const { return std::rend(iterable_); }
};

class MyListener : public Catch::TestEventListenerBase
{
private:
    typedef Catch::TestEventListenerBase inherited;

    std::string cur_testcase; // current testcase

    using invoked_callback_t = std::pair<callback_handle_t, std::string /* tag */>;
    std::vector<invoked_callback_t> invoked_cbs;        // invoked callbacks when current testcase starting
    std::vector<invoked_callback_t> invoked_shared_cbs; // invoked callbacks shared by multiple testcases (tags).

public:
    using TestEventListenerBase::TestEventListenerBase; // inherit constructor
    virtual ~MyListener() override = default;

    virtual void testRunStarting(Catch::TestRunInfo const& testRunInfo) override
    {
        inherited::testRunStarting(testRunInfo);

        man().onTestStart();
    }
    virtual void testRunEnded(Catch::TestRunStats const& testRunStats) override
    {
        man().onTestEnded();

        // we have to clean up shared callbacks upon test-run ending.
        // TODO: clean up shared callback when the last associated testcase / tag ending.
        if (!invoked_shared_cbs.empty()) {
            for (auto const& cb : reverse(invoked_shared_cbs)) {
                const auto& [fcb, shared] = man().getCallbackInfo(cb.first);
                fcb(false, cb.second);
            }
            invoked_shared_cbs.clear();
        }

        CallbackMan::clean(); // not used anymore

        inherited::testRunEnded(testRunStats);
    }

    void testCaseStarting(Catch::TestCaseInfo const& _testInfo) override
    {
        inherited::testCaseStarting(_testInfo);

        if (man().verbose_printing_enabled()) {
            std::cout << "testcase-name (" << _testInfo.name << "), tag: (";
            for (auto const& tag : _testInfo.tags)
                std::cout << tag << ", ";
            std::cout << ") starting..." << std::endl;
        }

        cur_testcase = _testInfo.name;
        invoked_cbs.clear();

        for (auto const& tag : _testInfo.tags) {
            auto const& handles = man().getCallbackHandles(tag);
            if (handles.empty())
                continue;

            for (auto h : handles) {
                auto const& [tag_callback, shared] = man().getCallbackInfo(h);

                // [1] A callback can be associated with a testcase multiple times from different tags,
                // however, it only be invoked once for a testcase, no matter it is shared or not.
                //
                // [2] For a shared callback (shared == true), it can only be invoked once for all associated testcases.
                if (shared) {
                    if (std::any_of(invoked_shared_cbs.begin(), invoked_shared_cbs.end(), [&](invoked_callback_t const& cb) {
                            return cb.first == h;
                        }))
                        continue;
                } else {
                    if (std::any_of(invoked_cbs.begin(), invoked_cbs.end(), [&](invoked_callback_t const& cb) {
                            return cb.first == h;
                        }))
                        continue;
                }

                tag_callback(true, tag);

                (shared ? invoked_shared_cbs : invoked_cbs).push_back(std::make_pair(h, tag));
            }
        }
    }
    void testCaseEnded(Catch::TestCaseStats const& _testCaseStats) override
    {
        if (man().verbose_printing_enabled()) {
            std::cout << "testcase-name (" << _testCaseStats.testInfo.name << "), tag: (";
            for (auto const& tag : _testCaseStats.testInfo.tags)
                std::cout << tag << ", ";
            std::cout << ") ended!" << std::endl;
        }

        // should be paired with previous testCaseStarting().
        assert(cur_testcase == _testCaseStats.testInfo.name);

        // Ending-Callbacks are invoked in reversed order
        for (auto const& cb : reverse(invoked_cbs)) {
            auto const& [tag_callback, _] = man().getCallbackInfo(cb.first);
            tag_callback(false, cb.second);
        }

        cur_testcase.clear();
        invoked_cbs.clear();

        inherited::testCaseEnded(_testCaseStats);
    }
};

CATCH_REGISTER_LISTENER(MyListener);

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

} // namespace

void register_test_callback(test_callback_t test_callback)
{
    if (!test_callback)
        return;
    man().register_test_callback(test_callback);
}

void register_tag_callback(const std::string& tag_or_tags, bool shared, tag_callback_t tag_callback)
{
    if (!tag_callback)
        return;
    man().register_tag_callback(tag_or_tags, tag_callback, shared);
}

void enable_verbose_printing(bool enable)
{
    man().verbose_printing_enabled(enable);
}
bool verbose_printing()
{
    return man().verbose_printing_enabled();
}

bool is_same_tag(const std::string& tag1, const std::string& tag2)
{
    return tag1 == tag2 || parse_first_tag(tag1) == parse_first_tag(tag2);
}

std::string parse_first_tag(std::string const& arg)
{
    std::string tag;
    std::istringstream stm(arg);
    while (std::getline(stm, tag, ',')) {
        if (tag.empty())
            continue;
        ltrim(rtrim(tag));

        if (tag[0] == '[') {
            tag = tag.substr(1);
            ltrim(tag);
            if (tag.empty())
                continue;
        }

        if (*tag.rbegin() == ']') {
            tag = tag.substr(0, tag.size() - 1);
            rtrim(tag);
            if (tag.empty())
                continue;
        }

        if (!tag.empty()) {
            return tag;
        }
    }

    return "";
}

std::vector<std::string> parse_tags(std::string const& arg)
{
    std::vector<std::string> tags;
    std::string tag;
    std::istringstream stm(arg);
    while (std::getline(stm, tag, ',')) {
        if (tag.empty())
            continue;
        ltrim(rtrim(tag));

        if (tag[0] == '[') {
            tag = tag.substr(1);
            ltrim(tag);
            if (tag.empty())
                continue;
        }

        if (*tag.rbegin() == ']') {
            tag = tag.substr(0, tag.size() - 1);
            rtrim(tag);
            if (tag.empty())
                continue;
        }

        if (!tag.empty()) {
            tags.push_back(tag);
        }
    }

    return tags;
}
} // namespace catch_ex
