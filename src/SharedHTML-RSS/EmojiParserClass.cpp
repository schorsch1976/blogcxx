/*
 * blogcpp :: https://www.blogcpp.org
 * Emoji parser class.
 */

#include "EmojiParserClass.h"

#include <regex>
#include <sstream>

#include "Shared/Debug.h"

#ifndef TEST_OLDER_COMPILERS
using namespace std::literals::string_literals; // for s
#endif

EmojiParser::EmojiParser()
#ifndef TEST_OLDER_COMPILERS
	: // clang-format off
	// The emojis are kept in an assignment table for your and
	// my own convenience.
	// Code reference: http://emojipedia.org/people/
		emojis {
		{ ":-)", u8"\U0001F642"s },
		{ ";-)", u8"\U0001F609"s },
		{ ":-D", u8"\U0001F600"s },
		{ ":-(", u8"\U00002639"s },
		{ ":'(", u8"\U0001F62D"s },
		{ ":-|", u8"\U0001F610"s },
		{ ">:)", u8"\U0001F608"s },
		{ ">:-)", u8"\U0001F608"s },
		{ ">:(", u8"\U0001F620"s },
		{ ">:-(", u8"\U0001F620"s },
		{ ":-*", u8"\U0001F618"s },
		{ ":-O", u8"\U0001F62E"s },
		{ ":-o", u8"\U0001F62E"s },
		{ ":-S", u8"\U0001F615"s },
		{ ":-s", u8"\U0001F615"s },
		{ ":-#", u8"\U0001F636"s },
		{ "0:-)", u8"\U0001F607"s },
		{ ":o)", u8"\U0001F921"s },
		{ "<_<", u8"\U0001F612"s },
		{ "^^", u8"\U0001F60A"s },
		{ "^_^", u8"\U0001F60A"s },
		{ "<3", u8"\U00002764"s },
		{ "m(", u8"\U0001F926"s }
	}
// clang-format on
#endif
{
}

std::string EmojiParser::clear(const std::string text)
{
	// Removes emojis from <text>.
	if (text.length() == 0)
	{
		return "";
	}

	std::istringstream iss(text);
	std::stringstream ret;
	std::string word;

	LOG_DEBUG("Emojis: Clearing '%1%' ...", text);

	while (iss >> word)
	{
		// Remove <word> if it is an emoji.
		const auto emoji = emojis.find(word);
		if (emoji != emojis.end())
		{
			ret << "";
		}
		else
		{
			ret << word;
		}
		ret << " ";
	}

	return ret.str();
}

std::string EmojiParser::parse(const std::string text)
{
	// Puts emojis into <text>.
	if (text.length() == 0)
	{
		return "";
	}

	std::istringstream iss(text);
	std::stringstream temp;
	std::ostringstream ret;
	std::string word;

	LOG_DEBUG("Emojis: Parsing '%1%' ...", text);

	// Try to find leading and trailing white spaces. Add them back
	// after all parsing has been done.
	std::smatch match;
	std::regex whitespaces("^([\\s\\t]*).*?([\\s\\t]*)$");
	bool bHasSurroundingWS = std::regex_search(text, match, whitespaces);

	if (bHasSurroundingWS && (match[1].length() > 0))
	{
		// Add the leading spaces:
		temp << match[1];
	}

	while (iss >> word)
	{
		// Add every word to the output string; convert it first
		// if it's an emoji.
		const auto emoji = emojis.find(word);
		if (emoji != emojis.end())
		{
			temp << emoji->second;
		}
		else
		{
			temp << word;
		}
		temp << " "; // This is double
	}

	// Remove the last space again. (Alternatively, count the tokens beforehand,
	// but that would probably double the calculations.)
	if (temp.str().length() > 0)
	{
		std::string truncated = temp.str();
		truncated.erase(truncated.end() - 1, truncated.end());
		temp.str(truncated);
	}

	// We need a copy of temp here, since the previous if(){} might have
	// destroyed our poor little stringstream. :-(
	ret << temp.str();
	if (bHasSurroundingWS && match.size() == 3 && (match[2].length() > 0))
	{
		// Add the trailing spaces:
		ret << match[2];
	}

	return ret.str();
}
