#include "CreateRSS.h"

#include "Shared/Helpers.h"
#include "Shared/TextParseAll.h"

#include <iomanip>

void CreateRSS(const fs::path outfile, const std::string title,
			   const ConstArchive ar, const ConfigCollection &cfgs,
			   const TemplateWrapper &engine)
{
	// Creates the site's RSS.xml file.
	PRINT("Creating the RSS feed '%1%.", outfile.string());

	TemplateData data;
	data.Set({"title"}, title);
	data.Set({"link"}, cfgs.url(""));
	data.Set({"description"}, cfgs.sitetitle());
	data.Set({"lastBuildDate"}, dateToPrint(pt::second_clock::local_time(),
											time_fmt::date_time_rss));

	for (int i = 0; i < cfgs.maxitems() && i < static_cast<int>(ar.size()); ++i)
	{
		const SingleItem &post = *ar[i];

		std::string desc =
			TextParseAll(post, cfgs, !cfgs.fullfeed(), false /* no markdown */)
				.first;

		data.Set({"items", i, "title"}, post.s_title);
		data.Set({"items", i, "description"}, desc);
		data.Set({"items", i, "link"},
				 cfgs.url(cfgs.rel_path_posts(post.s_slug)));
		data.Set({"items", i, "pubDate"},
				 dateToPrint(post.time, time_fmt::date_time_rss));
	}

	LOG_DEBUG("RSS: %1%", data.to_string());

	std::string rendered = engine.Render(cfgs.tpl_RSS(), data);
	if (rendered.empty())
	{
		THROW_FATAL("CreateRSS: file '%1%': Rendered data is empty.");
	}
	// write the file
	write_file(outfile, rendered);
}
