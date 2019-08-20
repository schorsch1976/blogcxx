/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CreatePost.h"

#include "Shared/Helpers.h"
#include "Shared/TextParseAll.h"

void CreatePost(SingleItem::ConstPtr s, const ConstArchive &series,
				const ConfigCollection &cfgs, const TemplateWrapper &engine,
				TemplateData data)
{
	const SingleItem &si = *s;

	data.Set({"title"}, si.s_title);
	data.Set({"date"}, dateToPrint(si.time));
	data.Set({"changedate"}, dateToPrint(si.time));

	data.Set({"author"}, si.s_author);
	data.Set({"author-url"}, cfgs.url(cfgs.rel_path_authors(si.s_author)));

	// pagetitle
	data.Set({"pagetitle"}, si.s_title);

	// series
	if (si.s_series.size() > 0 && series.size() == 1)
	{
		LOG_WARN("CreatePost: Series '%1%' has just one post.",
				 si.s_filename.string());
	}
	if (si.s_series.size() > 0 && series.size() > 1)
	{
		data.Set({"series", "name"}, si.s_series);

		int this_index_in_series = -1;
		for (int i = 0; i < static_cast<int>(series.size()); ++i)
		{
			if (series[i] == s)
			{
				this_index_in_series = i;
				break;
			}
		}

		if (this_index_in_series < 0)
		{
			THROW_FATAL("CreatePost: Cant find his post in series list. This "
						"is a bug. Please report it at %1%.",
						BUGTRACKER);
		}

		if (this_index_in_series > 0)
		{
			data.Set({"series", "prev", "name"},
					 series[this_index_in_series - 1]->s_title);
			data.Set({"series", "prev", "url"},
					 cfgs.url(cfgs.rel_path_posts(
						 series[this_index_in_series - 1]->s_slug)));
		}
		if (this_index_in_series < static_cast<int>(series.size()) - 1)
		{
			data.Set({"series", "next", "name"},
					 series[this_index_in_series + 1]->s_title);
			data.Set({"series", "next", "url"},
					 cfgs.url(cfgs.rel_path_posts(
						 series[this_index_in_series + 1]->s_slug)));
		}
	}

	// permalink
	data.Set({"permalink"}, cfgs.url(cfgs.rel_path_posts(si.s_slug)));

	// tags of this post
	int i = 0;
	data.Set({"tags_title"}, "Tags");
	for (auto &tag : si.tags)
	{
		data.Set({"tags", i, "name"}, tag);
		data.Set({"tags", i, "slug"}, "tag-slug");
		data.Set({"tags", i, "URL"}, cfgs.url(cfgs.rel_path_tags(tag)));
		++i;
	}

	// cats of this post
	i = 0;
	data.Set({"cats_title"}, "Categories");
	for (auto &cat : si.cats)
	{
		data.Set({"categories", i, "name"}, cat);
		data.Set({"categories", i, "slug"}, "tag-slug");
		data.Set({"categories", i, "URL"},
				 cfgs.url(cfgs.rel_path_categories(cat)));
		++i;
	}

	// disable comments, if requested
	if (!si.b_comments)
	{
		data.Set({"comments-header"}, "<!-- No comments -->");
		data.Set({"comments-body"}, "<!-- No comments -->");
	}

	// and the text
	std::string text;
	std::vector<std::string> used_highlight_languages;
	std::tie(text, used_highlight_languages) = TextParseAll(si, cfgs);
	data.Set({"text"}, text);

	// headscripts (pluginsystem )
	data.Set({"headscripts"}, ""); // TODO

	// highlight
	i = 0;
	if (used_highlight_languages.size())
	{
		std::sort(std::begin(used_highlight_languages),
				  std::end(used_highlight_languages));
		auto last = std::unique(std::begin(used_highlight_languages),
								std::end(used_highlight_languages));
		used_highlight_languages.erase(last,
									   std::end(used_highlight_languages));

		for (std::string lang : used_highlight_languages)
		{
			data.Set({"highlightdata", i, "lang"}, lang);
			++i;
		}
	}
	// create the file
	PRINT("Creating a new post with the %1% template from '%2%'",
		  cfgs.tpldir().string(), si.s_filename);

	fs::path targetdir{cfgs.outdir_root()};
	targetdir /= cfgs.rel_path_posts(si.s_slug);
	fs::path targetfile{targetdir};
	targetfile /= "index.html";

	LOG_TRACE("CreatePost: %1%", data.to_string());

	std::string rendered = engine.Render(cfgs.tpl_post(), data);
	if (rendered.empty())
	{
		THROW_FATAL("CreatePost: file '%1%': Rendered data is empty.");
	}

	// write the file
	if (!fs::exists(targetdir))
	{
		fs::create_directories(targetdir);
	}
	write_file(targetfile, rendered);
}
