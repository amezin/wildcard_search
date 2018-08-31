#pragma once

#include <vector>
#include <string>

struct match
{
    const char *start;
    const char *end;

    match();
    match(const char *start, const char *end);

    std::size_t length() const;
    std::string str() const;
};

bool operator <(const match &, const match &);
bool operator >(const match &, const match &);
bool operator <=(const match &, const match &);
bool operator >=(const match &, const match &);
bool operator ==(const match &, const match &);
bool operator !=(const match &, const match &);

std::vector<match> wildcard_match(const char *str, const char *pattern);
match wildcard_longest(const char *str, const char *pattern);
