/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Slug.h"

#include <boost/locale.hpp>

#include "EmojiParser.h"
#include "Shared/Debug.h"
#include "Shared/Helpers.h"

#include <iomanip>
#include "Shared/regex.h"

std::string createBasicSlug(const SingleItem &si)
{
	// Creates a basic slug.
	std::string start;
	if (si.s_slug.empty())
	{
		start = si.s_title;
	}
	else
	{
		start = si.s_slug;
	}
	if (start.empty())
	{
		THROW_FATAL(
			"createBasicSlug: Item has no slug and no title: File: '%1%'",
			si.s_filename.string());
	}
	// Lowercase:
	std::string uinput = lowercase(start);

	// Umlauts/diacritics -> ASCII:
	std::string ret = boost::locale::normalize(uinput, boost::locale::norm_nfd);

	// Emojis:
	ret = EmojiParser::clear(ret);

	// Hyphenization:
	ret = trim(hyphenise(ret));
	transform(ret.begin(), ret.end(), ret.begin(),
			  [](char ch) { return ch == ' ' ? '-' : ch; });

	// Avoid double hyphenization though:
	ret = rx::regex_replace(ret, rx::regex("--+"), "-");

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
