#include "TextParseAll.h"

#include "ExcerptExtractor.h"
#include "Shared/Debug.h"

#include "MarkdownWrapperClass.h"

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */, bool markdown /* = true */)
{
	std::string txt = si.s_text;
	if (exceprt)
	{
		bool shortened = false;
		std::tie(txt, shortened) = Excerpt::extract(txt, cfgs.excerpts());
	}

	auto marked = MarkdownWrapper::markdownify(
		txt, !(si.b_markdown && markdown), !si.b_emoji, cfgs.embeds());

	return std::make_pair(marked.parsed_text, marked.used_languages);
}
