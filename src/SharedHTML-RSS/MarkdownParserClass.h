/*
 * blogcpp :: https://www.blogcpp.org
 * Markdown-to-HTML class [header].
 */

#pragma once

#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "MarkdownReturnClass.h"
#include "MediaEmbedderClass.h"
#include "Shared/constants.h"

class MarkdownParser
{
public:
	MarkdownReturn markdownify(std::string inputtext, bool basic_markdown,
							   bool no_emojis);
	void prepare(std::string posttxt);
	void parse(const std::string inputline);
	std::string getParsedText() { return full_markdown_text; }
	std::vector<std::string> getCodeLanguages() { return used_code_languages; }
	void cleanup();

	MarkdownParser(bool basic_markdown, bool add_embeds);

private:
	std::string full_markdown_text;
	std::vector<std::string> used_code_languages;
	std::unordered_map<int, std::string> legend_urls;

	std::stringstream ss_codelang;

	bool in_blockquote;
	bool in_ol;
	bool in_ul;
	bool in_code;
	bool in_raw_html;

	bool dont_add_paragraphs;

	bool dont_use_full_markdown;
	bool use_embeds;
};
