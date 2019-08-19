/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CreateArchive.h"

#include "Shared/Helpers.h"
#include "Shared/constants.h"

void CreateArchive(const fs::path rel_path, const ConstArchive &ar,
				   const ConfigCollection &cfgs, const TemplateWrapper &engine,
				   std::string feed_url, TemplateData data)
{
	// fs::path short_path = fs::relative(rel_path, cfgs.rel_path_archive());

	// pagetitle
	data.Set({"pagetitle"}, "Archive " + rel_path.string());
	data.Set({"title"}, "Archive " + rel_path.string());

	// override RSS
	data.Set({"rsslink"}, feed_url);

	// find latest date in this archive
	auto pos = std::max_element(ar.begin(), ar.end(), time_smaller);
	data.Set({"date"}, dateToPrint((*pos)->time));

	// series
	data.Set({"series"}, boolean(false));

	data.Set({"what"}, rel_path.string());

	int i = 0;
	for (auto &s : ar)
	{
		const SingleItem &si = *s;
		switch (si.type)
		{
			case ItemType::Page:
				data.Set({"entries", i, "URL"},
						 cfgs.url(cfgs.rel_path_pages(si.s_slug)));
				break;

			case ItemType::Post:
				data.Set({"entries", i, "URL"},
						 cfgs.url(cfgs.rel_path_posts(si.s_slug)));
				break;
			default:
				THROW_FATAL("CreateArchive: Undefined ItemType in file '%1%'. "
							"Please fill a bugreport at %2%.",
							si.s_filename.string(), BUGTRACKER);
		}
		data.Set({"entries", i, "item"}, si.s_title);
		data.Set({"entries", i, "metadata"}, dateToPrint(si.time));
		++i;
	}

	// create the file
	PRINT("Creating a new archive with the %1% template from '%2%'",
		  cfgs.tpldir().string(), rel_path.string());

	fs::path targetdir{cfgs.outdir_root()};
	targetdir /= rel_path;
	fs::path targetfile{targetdir};
	targetfile /= "index.html";

	std::string rendered = engine.Render(cfgs.tpl_archiv(), data);
	if (rendered.empty())
	{
		THROW_FATAL("CreateArchive: file '%1%': Rendered data is empty",
					rel_path.string());
	}

	// write the file
	if (!fs::exists(targetdir))
	{
		fs::create_directories(targetdir);
	}
	write_file(targetfile, rendered);
}
