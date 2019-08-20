/*
 * blogcxx :: https://www.blogcxx.de
 * Excerpt extractor [header].
 */

#pragma once

#include "Shared/regex.h"
#include <iterator>
#include <sstream>
#include <string>

namespace Excerpt
{
std::pair<std::string, bool> extract(std::string string_to_extract,
									 int max_sentences);
} // ns Excerpt
