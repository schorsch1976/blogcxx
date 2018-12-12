/*
 * blogcxx :: https://www.blogcxx.de
 * Emoji parser class [header].
 */

#pragma once

#include <string>

class EmojiParser
{
public:
	EmojiParser();

	std::string clear(const std::string text);
	std::string parse(const std::string text);
};
