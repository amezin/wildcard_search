#include "wildcard_search.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iterator>

match::match()
    : start(nullptr), end(nullptr)
{
}

match::match(const char *start, const char *end)
    : start(start), end(end)
{
}

std::size_t match::length() const
{
    auto diff = end - start;
    assert(diff >= 0);
    return size_t(diff);
}

std::string match::str() const
{
    return std::string(start, length());
}

static ptrdiff_t match_diff(const match &a, const match &b)
{
    if (a.start == b.start) {
        return a.end - b.end;
    }
    return a.start - b.start;
}

bool operator<(const match &a, const match &b)
{
    return match_diff(a, b) < 0;
}

bool operator>(const match &a, const match &b)
{
    return match_diff(a, b) > 0;
}

bool operator<=(const match &a, const match &b)
{
    return match_diff(a, b) <= 0;
}

bool operator>=(const match &a, const match &b)
{
    return match_diff(a, b) >= 0;
}

bool operator==(const match &a, const match &b)
{
    return match_diff(a, b) == 0;
}

bool operator!=(const match &a, const match &b)
{
    return match_diff(a, b) != 0;
}

namespace {

template<typename T>
class dp_matrix
{
public:
    explicit dp_matrix(size_t width, const T &init_value = T())
        : prev(width, init_value), cur(width, init_value), i(0)
    {
    }

    void next_row(const T &init_value = T())
    {
        i++;
        prev.swap(cur);
        fill(cur.begin(), cur.end(), init_value);
    }

    std::vector<T> &operator[](std::size_t row)
    {
        assert(row + 1 == i || row == i);
        if (row + 1 == i) {
            return prev;
        }
        if (row == i) {
            return cur;
        }
        std::abort();
    }

private:
    std::vector<T> prev, cur;
    std::size_t i;
};

struct all_matches
{
    std::vector<const char *> matches;
    typedef std::vector<match> result_type;

    all_matches()
    {
    }

    explicit all_matches(const char *match)
    {
        matches.push_back(match);
    }

    static all_matches merge(const all_matches &a, const all_matches &b)
    {
        all_matches merged;
        std::set_union(a.matches.begin(), a.matches.end(),
                       b.matches.begin(), b.matches.end(),
                       std::back_inserter(merged.matches));
        return merged;
    }

    void add_to_result(result_type &result, const char *match_end)
    {
        for (auto &match_start : matches) {
            result.push_back(match(match_start, match_end));
        }
    }
};

struct longest_match
{
    const char *match_start;
    typedef match result_type;

    longest_match()
        : match_start(nullptr)
    {
    }

    explicit longest_match(const char *match_start)
        : match_start(match_start)
    {
    }

    static longest_match merge(const longest_match &a, const longest_match &b)
    {
        if (!a.match_start) {
            return longest_match(b.match_start);
        } else if (!b.match_start) {
            return longest_match(a.match_start);
        } else {
            return longest_match(std::min(a.match_start, b.match_start));
        }
    }

    void add_to_result(result_type &result, const char *match_end)
    {
        if (!match_start) {
            return;
        }

        result_type this_match(match_start, match_end);
        if (result.length() < this_match.length()) {
            result = this_match;
        } else if (result.length() == this_match.length() && this_match < result) {
            result = this_match;
        }
    }
};

template<typename M>
typename M::result_type wildcard_match_internal(const char *str, const char *pattern)
{
    typename M::result_type all_matches;

    auto str_len = std::strlen(str);
    auto pattern_len = std::strlen(pattern);

    dp_matrix<M> matches(pattern_len + 1);
    for (size_t i = 0; i <= str_len; i++) {
        matches[i][0] = M(&str[i]);

        for (size_t j = 1; j <= pattern_len; j++) {
            if (i) {
                if (pattern[j - 1] == '?' || pattern[j - 1] == str[i - 1]) {
                    matches[i][j] = matches[i - 1][j - 1];
                }
            }

            if (pattern[j - 1] == '*') {
                if (i) {
                    matches[i][j] = M::merge(matches[i][j - 1], matches[i - 1][j]);
                } else {
                    matches[i][j] = matches[i][j - 1];
                }
            }
        }

        matches[i][pattern_len].add_to_result(all_matches, &str[i]);
        matches.next_row();
    }

    return all_matches;
}

}

/*
 * O(str_len * str_len * pattern_len) time complexity.
 * O(pattern_len * str_len) additional memory.
 */
std::vector<match> wildcard_match(const char *str, const char *pattern)
{
    return wildcard_match_internal<all_matches>(str, pattern);
}

/*
 * O(str_len * pattern_len) time complexity.
 * O(pattern_len) additional memory.
 */
match wildcard_longest(const char *str, const char *pattern)
{
    return wildcard_match_internal<longest_match>(str, pattern);
}
