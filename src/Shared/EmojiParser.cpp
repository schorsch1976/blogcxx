/*
 * blogcxx :: https://www.blogcxx.de
 * Emoji parser.
 */

#include "EmojiParser.h"

#include "Shared/regex.h"
#include <cassert>
#include <memory>
#include <mutex> // once_flag
#include <sstream>
#include <unordered_map>

#include <boost/locale.hpp>

#include "Log/Log.h"

namespace
{
std::string u8(std::wstring w)
{
	return boost::locale::conv::utf_to_utf<char>(w);
}
struct EmojiMaps
{
	EmojiMaps()
	{
		// The emojis are kept in an assignment table for your and
		// my own convenience.
		// Code reference: http://emojipedia.org/people/
		m_emojis[":-)"] = u8(L"\U0001F642");
		m_emojis[";-)"] = u8(L"\U0001F609");
		m_emojis[":-D"] = u8(L"\U0001F600");
		m_emojis[":-("] = u8(L"\U00002639");
		m_emojis[":'("] = u8(L"\U0001F62D");
		m_emojis[":-|"] = u8(L"\U0001F610");
		m_emojis[">:)"] = u8(L"\U0001F608");
		m_emojis[">:-)"] = u8(L"\U0001F608");
		m_emojis[">:("] = u8(L"\U0001F620");
		m_emojis[">:-("] = u8(L"\U0001F620");
		m_emojis[":-*"] = u8(L"\U0001F618");
		m_emojis[":-O"] = u8(L"\U0001F62E");
		m_emojis[":-o"] = u8(L"\U0001F62E");
		m_emojis[":-S"] = u8(L"\U0001F615");
		m_emojis[":-s"] = u8(L"\U0001F615");
		m_emojis[":-#"] = u8(L"\U0001F636");
		m_emojis["0:-)"] = u8(L"\U0001F607");
		m_emojis[":o)"] = u8(L"\U0001F921");
		m_emojis["<_<"] = u8(L"\U0001F612");
		m_emojis["^^"] = u8(L"\U0001F60A");
		m_emojis["^_^"] = u8(L"\U0001F60A");
		m_emojis["<3"] = u8(L"\U00002764");
		m_emojis["m("] = u8(L"\U0001F926");

		// build the regex to replacement map
		for (auto &e : m_emojis)
		{
			std::string escaped = escape(e.first);

			// between words
			rx::regex re_emoji_1("[[:space:]]" + escaped + "[[:space:]]");

			// begin of a line
			rx::regex re_emoji_2("^" + escaped + "[[:space:]]");

			// end of a line
			rx::regex re_emoji_3("[[:space:]]" + escaped + "$");

			// alone on a line
			rx::regex re_emoji_4("^" + escaped + "$");

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

	std::unordered_map<std::string, std::string> m_emojis;
	std::vector<std::pair<rx::regex, std::string>> m_regex_to_replacement;
};

std::shared_ptr<const EmojiMaps> sp_emoji_map;

void init()
{
	// just initialize the emoji maps once
	static std::once_flag s_once_emojis;
	std::call_once(s_once_emojis, []() {
		sp_emoji_map = std::make_shared<const EmojiMaps>();
	});
	assert(sp_emoji_map);
}

} // namespace

namespace EmojiParser
{
std::string clear(const std::string text)
{
	init();

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

std::string parse(const std::string text)
{
	init();

	std::istringstream iss(text);
	std::string line;
	std::string out;
	while (std::getline(iss, line))
	{
		// replace in this line every possible emoji
		for (auto &e : sp_emoji_map->m_regex_to_replacement)
		{
			line = rx::regex_replace(line, e.first, e.second);
		}

		// readd the line break
		out += line + "\n";
	}

	return out;
}
} // ns EmojiParser
