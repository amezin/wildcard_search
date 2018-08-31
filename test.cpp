#include "wildcard_search.h"
#include "slower_solutions_for_tests.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <random>

static const char *ab_patterns[] = {
    "*****ba*****ab",
    "ba*****a",
    "ba*ab",
    "a*ab",
    "a*****ab",
    "*a*****ab",
    "ba*ab****",
    "****",
    "*",
    "aa?ab",
    "b*b",
    "a*a",
    "baaabab",
    "?baaabab",
    "*baaaba*",
};

static std::mt19937 gen;

static std::string gen_string(size_t len, const char *chars)
{
    std::string result;
    auto nchars = std::strlen(chars);
    std::uniform_int_distribution<std::size_t> char_dist(0, nchars - 1);
    while (result.size() < len) {
        result.push_back(chars[char_dist(gen)]);
    }
    return result;
}

static void verify_output(std::vector<match> &naive, std::vector<match> &output)
{
    if (naive != output) {
        std::abort();
    }
}

static void do_test(const char *str, const char *pattern)
{
    auto naive = wildcard_match_naive(str, pattern);
    auto recursive = wildcard_match_recursive(str, pattern);
    auto dp = wildcard_match(str, pattern);

    std::sort(naive.begin(), naive.end());
    std::sort(recursive.begin(), recursive.end());
    std::sort(dp.begin(), dp.end());

    verify_output(naive, recursive);
    verify_output(naive, dp);
    verify_output(recursive, dp);
}

static void do_test_longest(const char *str, const char *pattern)
{
    auto naive = wildcard_longest_naive(str, pattern);
    auto dp = wildcard_longest(str, pattern);
    if (naive != dp) {
        std::abort();
    }
}

static void do_random_ab_string_test(size_t len, void (*test_func)(const char *, const char *))
{
    for (size_t i = 0; i < 100; i++) {
        auto s = gen_string(len, "ab");
        for (size_t j = 0; j < sizeof(ab_patterns) / sizeof(*ab_patterns); j++) {
            test_func(s.c_str(), ab_patterns[j]);
        }
    }
}

int main()
{
    std::cerr << "Testing wildcard_longest on big strings" << std::endl;
    for (size_t i = 0; i < 100; i++) {
        auto s = gen_string(10000, "ab");
        for (size_t j = 0; j < sizeof(ab_patterns) / sizeof(*ab_patterns); j++) {
            wildcard_longest(s.c_str(), ab_patterns[j]);
        }
    }

    std::cerr << "Testing wildcard_match on big strings" << std::endl;
    for (size_t i = 0; i < 100; i++) {
        auto s = gen_string(100, "ab");
        for (size_t j = 0; j < sizeof(ab_patterns) / sizeof(*ab_patterns); j++) {
            wildcard_match(s.c_str(), ab_patterns[j]);
        }
    }

    for (size_t len = 1; len <= 16; len++) {
        std::cerr << "Testing wildcard_longest random string length=" << len << std::endl;
        do_random_ab_string_test(len, do_test_longest);
    }

    for (size_t len = 0; len <= 16; len++) {
        std::cerr << "Testing wildcard_match random string length=" << len << std::endl;
        do_random_ab_string_test(len, do_test);
    }
}
