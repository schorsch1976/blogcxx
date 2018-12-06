/*
* blogcpp :: https://www.blogcpp.org
*/

#include "TextParseAll.h"
#include "ExcerptExtractorClass.h"
#include "Shared/Debug.h"

#if 0
#ifdef WITH_PLUGINS
// Process plug-ins:
Plugins plugins;
plugins.init(cfgs.cfg_plugins, cfgs.cfg_plugindir);
si.s_text = (lowercase(s_plugins) == "off"
	? ss_output.str()
	: plugins.processContentPlugins(ss_output.str()));
#else
si.s_text = ss_output.str();
#endif
#endif

#if USE_MARKDOWN_API == 1

#include "MarkdownWrapperClass.h"

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */)
{
	std::string txt = si.s_text;
	if (exceprt)
	{
		txt = ExcerptExtractor(txt, cfgs.excerpts()).extractExcerpt();
	}

	auto marked = MarkdownWrapper::markdownify(txt, !si.b_markdown, !si.b_emoji,
											   cfgs.embeds());

	return std::make_pair(marked.parsed_text, marked.used_languages);
}
#endif

#if USE_MARKDOWN_API == 2
#include "EmojiParserClass.h"

// we dont include cmark.h"
extern "C" char *cmark_markdown_to_html(const char *text, size_t len, int options);
#define CMARK_OPT_DEFAULT 0

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */)
{
	std::string txt = si.s_text;
	if (exceprt)
	{
		txt = ExcerptExtractor(txt, cfgs.excerpts()).extractExcerpt();
	}

	if (si.b_markdown)
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

	if (si.b_emoji)
	{
		txt = EmojiParser().parse(txt);
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
			used_languages.push_back(match[0]);
		}
	}
	std::sort(std::begin(used_languages), std::end(used_languages));
	auto last =
		std::unique(std::begin(used_languages), std::end(used_languages));
	used_languages.erase(last, std::end(used_languages));

	return std::make_pair(txt, used_languages);
}
#endif
