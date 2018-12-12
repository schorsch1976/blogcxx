#include "TextParseAll.h"

#include "EmojiParserClass.h"
#include "ExcerptExtractorClass.h"
#include "Shared/Debug.h"

#include "cmark.h"

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */, bool markdown /* = true */)
{
	std::string txt = si.s_text;
	if (exceprt)
	{
		txt = ExcerptExtractor(txt, cfgs.excerpts()).extractExcerpt();
	}

	if (si.b_emoji)
	{
		txt = EmojiParser().parse(txt);
	}

	if (si.b_markdown && markdown)
	{

		char *parsed_raw =
			cmark_markdown_to_html(txt.c_str(), txt.size(), CMARK_OPT_DEFAULT);
		if (!parsed_raw)
		{
			THROW_FATAL("TextParseAll: cmark could not parse text: '%1%'.",
						txt);
		}
		txt = std::string(parsed_raw);
		free(parsed_raw);
	}

	// get used languages
	std::vector<std::string> used_languages;
	std::regex codelang("^\\s{4}::::(\\S+)");
	std::istringstream iss(si.s_text);
	std::string line;
	while (std::getline(iss, line))
	{
		std::smatch match;
		if (std::regex_match(line, match, codelang))
		{
			used_languages.push_back(match[1]);
		}
	}
	std::sort(std::begin(used_languages), std::end(used_languages));
	auto last =
		std::unique(std::begin(used_languages), std::end(used_languages));
	used_languages.erase(last, std::end(used_languages));

	return std::make_pair(txt, used_languages);
}