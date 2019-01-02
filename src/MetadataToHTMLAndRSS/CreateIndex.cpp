/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CreateIndex.h"

#include "Shared/Helpers.h"
#include "Shared/TextParseAll.h"
#include "Shared/constants.h"

std::string to_target_index(size_t page)
{
	if (page == 0)
	{
		return "index.html";
	}
	else
	{
		return "index-" + std::to_string(page) + ".html";
	}
};

void CreateIndex(size_t page, size_t page_count, const ConstArchive &posts,
				 const ConstArchive &pages, const ConfigCollection &cfgs,
				 const TemplateWrapper &engine, TemplateData data)
{
	PRINT("Creating Index %1% of %2%. (%3%)", page + 1, page_count,
		  cfgs.tpldir().string());

	if (posts.empty())
	{
		THROW_FATAL("CreateIndex: Posts are empty. This is a bug. Please "
					"report it at %1%",
					BUGTRACKER);
	}
	data.Set({"title"}, cfgs.sitetitle());
	data.Set({"date"}, dateToPrint((*posts.begin())->time));
	data.Set({"changedate"}, dateToPrint((*posts.begin())->changetime));

	// blog page is active
	data.Set({"defaultclass"}, "active");

	// pagetitle
	data.Set({"pagetitle"}, cfgs.sitetitle());

	// series
	data.Set({"series"}, boolean(false));

	for (int i = 0; i < static_cast<int>(posts.size()); ++i)
	{
		SingleItem::ConstPtr post = posts[i];

		data.Set({"posts", i, "altclass"}, i % 2 == 0 ? "even" : "odd");
		data.Set({"posts", i, "stickyclass"}, post->b_sticky ? "sticky" : "");
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
	}

	// fill the pagination
	if (page_count > 1)
	{
		int prev_index = 0;
		int next_index = 0;
		for (size_t p = 0; p < page_count; ++p)
		{
			if (p < page)
			{
				data.Set({"pagination_prev", prev_index, "link"},
						 cfgs.url(to_target_index(p)));
				data.Set({"pagination_prev", prev_index, "text"},
						 std::to_string(p + 1));
				++prev_index;
			}
			else if (p == page)
			{
				data.Set({"pagination_active"}, std::to_string(p + 1));
			}
			else
			{
				data.Set({"pagination_next", next_index, "link"},
						 cfgs.url(to_target_index(p)));
				data.Set({"pagination_next", next_index, "text"},
						 std::to_string(p + 1));
				++next_index;
			}
		}
	}

	// create the file
	fs::path targetdir{cfgs.outdir_root()};
	fs::path targetfile{targetdir};
	targetfile /= to_target_index(page);

	LOG_TRACE("CreateIndex: %1%", data.to_string());

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
