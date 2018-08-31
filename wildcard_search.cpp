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

/*
 * O(str_len * str_len * pattern_len) time complexity.
 * O(pattern_len * str_len) additional memory.
 */
std::vector<match> wildcard_match(const char *str, const char *pattern)
{
    std::vector<match> all_matches;

    auto str_len = std::strlen(str);
    auto pattern_len = std::strlen(pattern);

    dp_matrix<std::vector<const char *>> matches(pattern_len + 1);
    for (size_t i = 0; i <= str_len; i++) {
        matches[i][0].push_back(&str[i]);

        for (size_t j = 1; j <= pattern_len; j++) {
            if (i) {
                if (pattern[j - 1] == '?' || pattern[j - 1] == str[i - 1]) {
                    matches[i][j] = matches[i - 1][j - 1];
                }
            }

            if (pattern[j - 1] == '*') {
                if (i) {
                    std::set_union(matches[i][j - 1].begin(), matches[i][j - 1].end(),
                                   matches[i - 1][j].begin(), matches[i - 1][j].end(),
                                   back_inserter(matches[i][j]));
                } else {
                    matches[i][j] = matches[i][j - 1];
                }
            }
        }

        for (auto &match : matches[i][pattern_len]) {
            all_matches.push_back({ match, &str[i] });
        }

        matches.next_row();
    }

    return all_matches;
}
