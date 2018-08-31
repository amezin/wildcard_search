#pragma once

#include "wildcard_search.h"

std::vector<match> wildcard_match_naive(const char *str, const char *pattern);
std::vector<match> wildcard_match_recursive(const char *str, const char *pattern);

match wildcard_longest_naive(const char *str, const char *pattern);
