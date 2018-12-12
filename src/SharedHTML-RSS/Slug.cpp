/*
* blogcpp :: https://www.blogcpp.org
*/

#include "Slug.h"

#include <boost/locale.hpp>

#include "EmojiParserClass.h"
#include "Shared/Debug.h"
#include "Shared/Helpers.h"

#include <iomanip>
#include <regex>

std::string createBasicSlug(std::string input)
{
	// Creates a basic slug.
	// Mainly used for category/tag/archive slugs.
	std::string ret;

	// Lowercase:
	std::string uinput = boost::locale::to_lower(input);

	// Umlauts/diacritics -> ASCII:
	ret = boost::locale::normalize(uinput, boost::locale::norm_nfd);

	// Emojis:
	EmojiParser eparser;
	ret = eparser.clear(ret);

	// Hyphenization:
	ret = trim(hyphenise(ret));
	transform(ret.begin(), ret.end(), ret.begin(),
			  [](char ch) { return ch == ' ' ? '-' : ch; });

	// Avoid double hyphenization though:
	ret = std::regex_replace(ret, std::regex("--+"), "-");

	// It's rather wise not to end a slug with ".", "\" or "/".
	// Also we could remove trailing hyphens and exclamation marks while we're
	// here.
	std::string::iterator it = ret.end() - 1;
	if (*it == '.' || *it == '\\' || *it == '/' || *it == '!' || *it == '-')
	{
		ret.erase(it);
	}

	return ret;
}
