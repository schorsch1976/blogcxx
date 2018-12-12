/*
 * blogcxx :: https://www.blogcxx.de
 * Emoji parser class.
 */

#include "EmojiParserClass.h"

#include <cassert>
#include <memory>
#include <mutex> // once_flag
#include <regex>
#include <sstream>
#include <unordered_map>

#include "Shared/Debug.h"

#ifndef TEST_OLDER_COMPILERS
using namespace std::literals::string_literals; // for s
#endif

namespace
{

struct EmojiMaps
{
	EmojiMaps()
#ifndef TEST_OLDER_COMPILERS
		: // clang-format off
			// The emojis are kept in an assignment table for your and
			// my own convenience.
			// Code reference: http://emojipedia.org/people/
		m_emojis
		{
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
		// build the regex to replacement map
		for (auto &e : m_emojis)
		{
			std::string escaped = escape(e.first);

			// between words
			std::regex re_emoji_1("[[:space:]]" + escaped + "[[:space:]]");

			// begin of a line
			std::regex re_emoji_2("^" + escaped + "[[:space:]]");

			// end of a line
			std::regex re_emoji_3("[[:space:]]" + escaped + "$");

			// alone on a line
			std::regex re_emoji_4("^" + escaped + "$");

			std::string replacement_1 = " " + e.second + " ";
			std::string replacement_2 = e.second + " ";
			std::string replacement_3 = " " + e.second;
			std::string replacement_4 = e.second;

			m_regex_to_replacement.emplace_back(
				std::make_pair(re_emoji_1, replacement_1));
			m_regex_to_replacement.emplace_back(
				std::make_pair(re_emoji_2, replacement_2));
			m_regex_to_replacement.emplace_back(
				std::make_pair(re_emoji_3, replacement_3));
			m_regex_to_replacement.emplace_back(
				std::make_pair(re_emoji_4, replacement_4));
		}
	}

	std::string escape(std::string e) const
	{
		const std::string need_escape{"[\\^$.|?*+(){}"};

		std::string out;
		for (char c : e)
		{
			auto pos = need_escape.find(c);
			if (pos == std::string::npos)
			{
				out += c;
			}
			else
			{
				out += "\\";
				out += c;
			}
		}
		return out;
	}

	const std::unordered_map<std::string, std::string> m_emojis;
	std::vector<std::pair<std::regex, std::string>> m_regex_to_replacement;
};

std::shared_ptr<const EmojiMaps> sp_emoji_map;
} // namespace

EmojiParser::EmojiParser()
{
	// just initialize the emoji maps once
	static std::once_flag s_once_emojis;
	std::call_once(s_once_emojis, []() {
		sp_emoji_map = std::make_shared<const EmojiMaps>();
	});
	assert(sp_emoji_map);
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
		const auto emoji = sp_emoji_map->m_emojis.find(word);
		if (emoji != sp_emoji_map->m_emojis.end())
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
	std::istringstream iss(text);
	std::string line;
	std::string out;
	while (std::getline(iss, line))
	{
		// replace in this line every possible emoji
		for (auto &e : sp_emoji_map->m_regex_to_replacement)
		{
			line = std::regex_replace(line, e.first, e.second);
		}

		// readd the line break
		out += line + "\n";
	}

	return out;
}
