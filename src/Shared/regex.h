#pragma once

#if USE_REGEX_API==1
#include <regex>
namespace rx = std;
#endif

#if USE_REGEX_API==2
#include <boost/regex.hpp>
namespace rx = boost;
#endif

#ifndef USE_REGEX_API
#error "Regex API no defined"
#endif
