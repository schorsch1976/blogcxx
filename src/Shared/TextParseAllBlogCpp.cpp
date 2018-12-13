#include "TextParseAll.h"

#include "ExcerptExtractorClass.h"
#include "Shared/Debug.h"

#include "MarkdownWrapperClass.h"

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */, bool markdown /* = true */)
{
	std::string txt = si.s_text;
	if (exceprt)
	{
		txt = ExcerptExtractor(txt, cfgs.excerpts()).extractExcerpt();
	}

	auto marked = MarkdownWrapper::markdownify(
		txt, !(si.b_markdown && markdown), !si.b_emoji, cfgs.embeds());

	return std::make_pair(marked.parsed_text, marked.used_languages);
}
