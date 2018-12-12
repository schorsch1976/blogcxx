/*
* blogcpp :: https://www.blogcpp.org
*/

#include "CreateIndex.h"

#include "Shared/Helpers.h"
#include "SharedHTML-RSS/TextParseAll.h"

void CreateIndex(const ConstArchive &posts, const ConstArchive &pages,
				 const ConfigCollection &cfgs, const TemplateWrapper &engine,
				 TemplateData data)
{
	data.Set({"title"}, cfgs.sitetitle());
	data.Set({"date"}, timeNow());
	data.Set({"changedate"}, timeNow());

	data.Set({"author"}, cfgs.author());
	data.Set({"authorslug"}, "author-slug");

	// pagetitle
	data.Set({"pagetitle"}, cfgs.sitetitle());

	// series
	data.Set({"series"}, boolean(false));

	// the pages
	int i = 0;
	for (auto &post : posts)
	{
		data.Set({"posts", i, "altclass"}, "altclass");
		data.Set({"posts", i, "stickyclass"}, "stickyclass");
		data.Set({"posts", i, "URL"},
				 cfgs.url(cfgs.rel_path_posts(post->s_slug)));
		data.Set({"posts", i, "title"}, post->s_title);
		data.Set({"posts", i, "author"}, post->s_author);
		data.Set({"posts", i, "date"}, dateToPrint(post->time));
		data.Set({"posts", i, "changedate"}, dateToPrint(post->changetime));

		std::string text;
		std::vector<std::string> used_highlight_languages;
		std::tie(text, used_highlight_languages) =
			TextParseAll(*post, cfgs, true);

		data.Set({"posts", i, "text"}, text);
		data.Set({"posts", i, "count"}, std::to_string(i));

		data.Set({"posts", i, "shortened"}, boolean(true));
		data.Set({"posts", i, "morelink"},
				 cfgs.url(cfgs.rel_path_posts(post->s_slug)));
		++i;
	}

	LOG_ERROR("Pagination is still empty.");
	data.Set({"pagination"}, "");

	// create the file
	PRINT("Creating the index.html with the %1% template.",
		  cfgs.tpldir().string());

	fs::path targetdir{cfgs.outdir_root()};
	fs::path targetfile{targetdir};
	targetfile /= "index.html";

	std::string rendered = engine.Render(cfgs.tpl_index(), data);
	if (rendered.empty())
	{
		THROW_FATAL("CreateIndex: file '%1%': Rendered data is empty.");
	}

	// write the file
	if (!fs::exists(targetdir))
	{
		fs::create_directories(targetdir);
	}
	write_file(targetfile, rendered);
}
