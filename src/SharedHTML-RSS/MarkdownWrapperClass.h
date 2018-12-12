/*
 * blogcxx :: https://www.blogcxx.de
 * Markdown-to-HTML wrapper, allowing a more abstract usage of the
 * MarkdownWrapper [header].
 */

#pragma once

#include <regex>
#include <sstream>
#include <string>

#include "EmojiParserClass.h"
#include "MarkdownParserClass.h"
#include "MarkdownReturnClass.h"

class MarkdownWrapper
{
public:
	static MarkdownReturn markdownify(std::string inputtext,
									  bool basic_markdown, bool no_emojis,
									  bool cfg_embeds);
};
