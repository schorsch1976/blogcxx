/*
 * blogcxx :: https://www.blogcxx.de
 * Return type for Markdown actions
 */

#pragma once

#include <string>
#include <vector>

class MarkdownReturn
{
public:
	std::vector<std::string> used_languages;
	std::string parsed_text;
};
