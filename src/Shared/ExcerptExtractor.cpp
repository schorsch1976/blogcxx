/*
 * blogcxx :: https://www.blogcxx.de
 * Excerpt extractor.
 */

#include "ExcerptExtractor.h"
#include "Shared/Debug.h"

namespace Excerpt
{

std::pair<std::string, bool> extract(std::string string_to_extract, int max_sentences)
{
	// Returns the excerpt and sets the "shortened" bool to "true" if
	// applicable. If <max_sentences> is 0, this method will only check for
	// manually set
	// <!--more--> links, similar to WordPress.
	std::stringstream ret;
	int sentence_counter = 1;
	bool shortened = false;

	// A "sentence" is - EITHER - text ending with ! or ? or . or : and a space
	// character or a line break - OR - text ending with a comma and a line
	// break (for addressing your readers).
	rx::regex re_sentence("(.*?)([!\\?\\.:][\\s\\n]|,\\n)");

	// If the user defined a "<!--more-->" item somewhere in the text, it
	// supersedes the automatically determined end of the excerpt. We also
	// accept "<!-- more -->".
	rx::regex re_morelink("<!--\\s?more\\s?-->");

	// Find the first <cfg_excerpt> sentences (ending with '!', '?' or '.')
	// and put them into our ret-Stream.
	auto it_sentcs_begin = rx::sregex_iterator(
		string_to_extract.begin(), string_to_extract.end(), re_sentence);
	auto it_sentcs_end = rx::sregex_iterator();

	for (auto it = it_sentcs_begin; it != it_sentcs_end;
		 ++it, sentence_counter++)
	{
		// Traverse through the sentences.
		if (rx::regex_search((*it).str(), re_morelink) ||
			(max_sentences > 0 && sentence_counter > max_sentences))
		{
			// The user added a manual break here, or the maximum number of
			// preview sentences would be exceeded. Add the <more...> link
			// accordingly.
			shortened = true;
			break;
		}

		ret << (*it).str();
	}

	return std::make_pair(ret.str(), shortened);
}

} // ns Excerpt