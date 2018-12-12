/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CreatePage.h"

#include "Shared/Helpers.h"
#include "SharedHTML-RSS/TextParseAll.h"

void CreatePage(SingleItem::ConstPtr s, int page_index,
				const ConfigCollection &cfgs, const TemplateWrapper &engine,
				TemplateData data)
{
	const SingleItem &si = *s;

	data.Set({"title"}, si.s_title);
	data.Set({"date"}, parseTmToPath(si.time, cfgs.permalink_format()));
	data.Set({"changedate"},
			 parseTmToPath(si.changetime, cfgs.permalink_format()));

	data.Set({"author"}, si.s_author);
	data.Set({"authorslug"}, "author-slug");

	// pagetitle
	data.Set({"pagetitle"}, si.s_title);

	// series
	data.Set({"series"}, boolean(false));

	// tags of this post
	int i = 0;
	data.Set({"tags_title"}, "Tags");
	for (auto &tag : si.tags)
	{
		data.Set({"tags", i, "name"}, tag);
		data.Set({"tags", i, "slug"}, "tag-slug");
		++i;
	}
	// and the text
	std::string text;
	std::vector<std::string> used_highlight_languages;
	std::tie(text, used_highlight_languages) = TextParseAll(si, cfgs);
	data.Set({"text"}, text);

	// active page
	data.Set({"pagenav", page_index, "cssclass"}, " active");

	// create the file
	PRINT("Creating a new page with the %1% template from '%2%'",
		  cfgs.tpldir().string(), si.s_filename);

	fs::path targetdir{cfgs.outdir_root()};
	targetdir /= cfgs.rel_path_pages(si.s_slug);
	fs::path targetfile{targetdir};
	targetfile /= "index.html";

	std::string rendered = engine.Render(cfgs.tpl_page(), data);
	if (rendered.empty())
	{
		THROW_FATAL("CreatePage: file '%1%': Rendered data is empty.");
	}

	// write the file
	if (!fs::exists(targetdir))
	{
		fs::create_directories(targetdir);
	}
	write_file(targetfile, rendered);
}
