/*
 * blogcpp :: https://www.blogcpp.org
 * Excerpt extractor class [header].
 */

#pragma once

#include <iterator>
#include <regex>
#include <sstream>
#include <string>

class ExcerptExtractor
{
public:
	ExcerptExtractor(std::string in_string, int in_excerpt_length);

	std::string extractExcerpt();
	bool shortened;

private:
	std::string string_to_extract;
	int max_sentences;
};
