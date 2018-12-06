/*
 * blogcpp :: https://www.blogcpp.org
 * Emoji parser class [header].
 */

#pragma once

#include <string>
#include <unordered_map>

class EmojiParser
{
public:
	EmojiParser();

	std::string clear(const std::string text);
	std::string parse(const std::string text);

private:
	const std::unordered_map<std::string, std::string> emojis;
};
