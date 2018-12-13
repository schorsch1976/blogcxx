#include "TextParseAll.h"

#include "EmojiParser.h"
#include "ExcerptExtractor.h"
#include "Shared/Debug.h"

#include "cmark-gfm-core-extensions.h"
#include "cmark-gfm.h"

#include <mutex> // once flag
#include <sstream>

namespace
{
// This is a function that will make enabling extensions easier later on.
void addMarkdownExtension(cmark_parser *parser, const char *extName)
{
	cmark_syntax_extension *ext = cmark_find_syntax_extension(extName);
	if (ext)
	{
		cmark_parser_attach_syntax_extension(parser, ext);
	}
	else
	{
		THROW_FATAL("cmark-gfm: Extension not found: '%1%'", extName);
	}
}

// A function to convert HTML to markdown
std::string to_html(std::string markdown_string, bool hardbreaks)
{
	int options = CMARK_OPT_DEFAULT; // You can also use
									 // CMARK_OPT_STRIKETHROUGH_DOUBLE_TILDE to
									 // enforce double tilde.
	options |= CMARK_OPT_SMART;
	options |= CMARK_OPT_STRIKETHROUGH_DOUBLE_TILDE;
	options |= CMARK_OPT_FOOTNOTES;
	options |= CMARK_OPT_GITHUB_PRE_LANG;
	options |= CMARK_OPT_UNSAFE;
	options |= CMARK_OPT_NORMALIZE;
	if (hardbreaks)
	{
		options |= CMARK_OPT_HARDBREAKS;
	}
	// Modified version of cmark_parse_document in blocks.c
	cmark_parser *parser = cmark_parser_new(options);

	///////////////////////
	/// Add extensions here
	///////////////////////
	addMarkdownExtension(parser, "strikethrough");
	addMarkdownExtension(parser, "table");
	addMarkdownExtension(parser, "autolink");
	addMarkdownExtension(parser, "tagfilter");

	cmark_node *doc;
	cmark_parser_feed(parser, markdown_string.c_str(), markdown_string.size());
	doc = cmark_parser_finish(parser);
	cmark_parser_free(parser);

	// Render
	char *html = cmark_render_html(doc, options, NULL);
	cmark_node_free(doc);

	if (!html)
	{
		THROW_FATAL("TextParseAll/to_html: cmark could not parse text: '%1%'.",
					markdown_string);
	}

	std::string ret(html);
	free(html);
	return ret;
}

} // namespace

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt /* = false */, bool markdown /* = true */)
{
	// as this functions can get called from many threads
	// (std::async), we need to make sure to call
	// cmark_gfm_core_extensions_ensure_registered()
	// just once
	static std::once_flag s_once_extensions;
	std::call_once(s_once_extensions,
				   [=]() { cmark_gfm_core_extensions_ensure_registered(); });

	std::string txt = si.s_text;
	if (exceprt)
	{
		bool shortened = false;
		std::tie(txt, shortened) = Excerpt::extract(txt, cfgs.excerpts());
	}

	if (si.b_emoji && si.b_markdown)
	{
		txt = EmojiParser::parse(txt);
	}

	if (si.b_markdown && markdown)
	{
		txt = to_html(txt, si.b_hardbreaks);
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
