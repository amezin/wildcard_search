#include "slower_solutions_for_tests.h"

#include <algorithm>
#include <cassert>
#include <cstring>

/*
 * --- Naive recursive solution ---
 */

static void wildcard_match_naive_internal(const char *str, const char *pattern, std::size_t i, std::vector<match> &matches)
{
    if (!*pattern) {
        matches.push_back({ str, &str[i] });
        return;
    }

    if (*pattern == '*') {
        wildcard_match_naive_internal(str, pattern + 1, i, matches);
    }

    if (!str[i]) {
        return;
    }

    if (str[i] == *pattern || *pattern == '?') {
        wildcard_match_naive_internal(str, pattern + 1, i + 1, matches);
    }
    if (*pattern == '*') {
        wildcard_match_naive_internal(str, pattern, i + 1, matches);
    }
}

std::vector<match> wildcard_match_naive(const char *str, const char *pattern)
{
    std::vector<match> all_matches;
    auto str_len = std::strlen(str);
    for (size_t i = 0; i <= str_len; i++) {
        wildcard_match_naive_internal(str + i, pattern, 0, all_matches);
    }
    std::sort(all_matches.begin(), all_matches.end());
    auto unique_end = std::unique(all_matches.begin(), all_matches.end());
    if (unique_end != all_matches.end()) {
        all_matches.erase(unique_end, all_matches.end());
    }
    return all_matches;
}

static void wildcard_longest_naive_internal(const char *str, const char *pattern, std::size_t i, match &longest)
{
    if (!*pattern) {
        match this_match(str, &str[i]);
        if (this_match.length() > longest.length()) {
            longest = this_match;
        } else if (this_match.length() == longest.length() && this_match < longest) {
            longest = this_match;
        }
        return;
    }

    if (*pattern == '*') {
        wildcard_longest_naive_internal(str, pattern + 1, i, longest);
    }

    if (!str[i]) {
        return;
    }

    if (str[i] == *pattern || *pattern == '?') {
        wildcard_longest_naive_internal(str, pattern + 1, i + 1, longest);
    }
    if (*pattern == '*') {
        wildcard_longest_naive_internal(str, pattern, i + 1, longest);
    }
}

match wildcard_longest_naive(const char *str, const char *pattern)
{
    match longest;
    auto str_len = std::strlen(str);
    for (size_t i = 0; i <= str_len; i++) {
        wildcard_longest_naive_internal(str + i, pattern, 0, longest);
    }
    return longest;
}

/*
 * --- Recursive solution that can be rewritten as DP solution ---
 */

/*
 * Matches of j pattern chars that ends at str[i] (str[i] is the first char past the end)
 */
static std::vector<const char *> match_at(const char *str, const char *pattern, std::size_t i, std::size_t j)
{
    if (j == 0) {
        return { &str[i] };
    }

    if (i) {
        if (pattern[j - 1] == '?' || pattern[j - 1] == str[i - 1]) {
            return match_at(str, pattern, i - 1, j - 1);
        }
    }

    if (pattern[j - 1] == '*') {
        auto res1 = match_at(str, pattern, i, j - 1);
        if (i) {
            auto res2 = match_at(str, pattern, i - 1, j);
            std::vector<const char *> res_merged;
            std::set_union(res1.begin(), res1.end(), res2.begin(), res2.end(), back_inserter(res_merged));
            return res_merged;
        } else {
            return res1;
        }
    }

    return {};
}

std::vector<match> wildcard_match_recursive(const char *str, const char *pattern)
{
    std::vector<match> all_matches;
    auto str_len = std::strlen(str);
    auto pattern_len = std::strlen(pattern);
    for (size_t i = 0; i <= str_len; i++) {
        auto res = match_at(str, pattern, i, pattern_len);
        for (auto &r : res) {
            all_matches.push_back({ r, &str[i] });
        }
    }
    return all_matches;
}
