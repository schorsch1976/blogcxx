/*
* blogcpp :: https://www.blogcpp.org
*/


#include "Shared/Debug.h"

#include "CreateHTML.h"

#include "Shared/AsyncWorker.h"
#include "Shared/Helpers.h"
#include "Shared/constants.h"
#include "Shared/filesystem.h"
#include "SharedHTML-RSS/CleanupDirectory.h"

#include <algorithm>
#include <chrono>

#include "CreateArchive.h"
#include "CreateIndex.h"
#include "CreatePage.h"
#include "CreatePost.h"

#include "TemplateWrapper.h"

TemplateData GenerateCommonTemplateData(const ConstMetadata &merged,
										const ConfigCollection &cfgs)
{
	int i = 0;
	TemplateData ret;

	ret.Set({"sitetitle"}, cfgs.sitetitle());
	ret.Set({"subtitle"}, cfgs.subtitle() ? *cfgs.subtitle() : "");

	// rsslink
	ret.Set({"rsslink"}, cfgs.url(cfgs.feeddir()));
	ret.Set({"baseurl"}, cfgs.url(""));

	ret.Set({"hljsver"}, HIGHLIGHT_VER);

	ret.Set({"defaultclass"}, "");
	ret.Set({"feedmeta"}, "");
	ret.Set({"opengraphmeta"}, "");
	ret.Set({"prevmeta"}, "");
	ret.Set({"nextmeta"}, "");
	ret.Set({"keywordsmeta"}, "");

	ret.Set({"blogcpp"}, APP_URL);

	// pagenav
	{
		std::vector<SingleItem::ConstPtr> sorted_page_nav;
		for (auto &p : merged.all_items)
		{
			if (p->type == ItemType::Page)
			{
				sorted_page_nav.push_back(p);
			}
		}
		std::sort(std::begin(sorted_page_nav), std::end(sorted_page_nav),
				  [](const SingleItem::ConstPtr lhs,
					 const SingleItem::ConstPtr rhs) -> bool {
					  return lhs->i_position < rhs->i_position;
				  });
		i = 0;
		for (auto &p : sorted_page_nav)
		{
			ret.Set({"pagenav", i, "title"}, p->s_title);
			ret.Set({"pagenav", i, "cssclass"}, "");
			ret.Set({"pagenav", i, "link"},
					cfgs.url(cfgs.rel_path_pages(p->s_slug)));
			i++;
		}
	}

	// latestposts
	std::vector<SingleItem::ConstPtr> latest_posts;
	for (auto &si : merged.all_items)
	{
		if (si->type != ItemType::Post)
		{
			continue;
		}
		latest_posts.push_back(si);
	}
	std::sort(std::begin(latest_posts), std::end(latest_posts), time_greater);
	while (latest_posts.size() > cfgs.maxhistory())
	{
		latest_posts.pop_back();
	}
	i = 0;
	for (auto &si : latest_posts)
	{
		ret.Set({"latestposts", i, "title"}, si->s_title);
		ret.Set({"latestposts", i, "date"}, dateToPrint(si->time, true));
		ret.Set({"latestposts", i, "link"},
				cfgs.url(cfgs.rel_path_posts(si->s_slug)));
		++i;
	}

	// archives
	{
		std::map<int, std::vector<ArchiveData>> archives_years_months;
		for (auto &ar : merged.archives)
		{
			const ArchiveData &ad = ar.first;
			switch (ad.type)
			{
				// because the year month archives get grouped by year
				case ArchiveType::YearMonth:
					archives_years_months[ad.time.tm_year + 1900].push_back(ad);
					break;
				case ArchiveType::Year:
				case ArchiveType::Author:
				case ArchiveType::Categories:
				case ArchiveType::Tags:
				case ArchiveType::Series:
					break;
				default:
					THROW_FATAL("GenerateCommonTemplateData: Unknown archive "
								"type. Please fill a bugreport at %1%",
								BUGTRACKER);
			}
		}

		// sort the month in the year
		for (auto &ar : archives_years_months)
		{
			std::sort(std::begin(ar.second), std::end(ar.second));
		}

		int y = 0;
		for (auto &ar : archives_years_months)
		{
			ret.Set({"archives", y, "year"}, std::to_string(ar.first));
			ret.Set({"archives", y, "URL"},
					cfgs.url(cfgs.rel_path_archive_year(ar.second[0].time)));

			int m = 0;
			for (auto &ad : ar.second)
			{
				ret.Set({"archives", y, "months", m, "month"},
						std::to_string(ad.time.tm_mon + 1));
				ret.Set({"archives", y, "months", m, "URL"},
						cfgs.url(cfgs.rel_path_archive_year_month(ad.time)));
				++m;
			}
			++y;
		}
	}

	// categories
	i = 0;
	for (auto &cat : merged.categories)
	{
		ret.Set({"allcats", i, "catname"}, cat.first);
		ret.Set({"allcats", i, "URL"},
				cfgs.url(cfgs.rel_path_categories(cat.first)));
		++i;
	}

	// tags
	i = 0;
	for (auto &tag : merged.tags)
	{
		ret.Set({"alltags", i, "tagname"}, tag.first);
		ret.Set({"alltags", i, "URL"}, cfgs.url(cfgs.rel_path_tags(tag.first)));
		++i;
	}

	// series
	i = 0;
	for (auto &series : merged.series)
	{
		ret.Set({"allseries", i, "seriesname"}, series.first);
		ret.Set({"allseries", i, "slug"}, series.first);
		++i;
	}

	// Blogroll
	{
		std::vector<int> links;

		// First, number the links.
		for (int i = 0; i < MAX_CONFIG_SIDEBAR_LINKS; i++)
		{
			if (cfgs.link_text(i) && cfgs.link_url(i))
			{
				links.push_back(i);
			}
		}

		// Now traverse them.
		for (size_t i = 0; i < links.size(); ++i)
		{
			ret.Set({"links", static_cast<int>(i), "URL"},
					*cfgs.link_url(links[i]));

			ret.Set({"links", static_cast<int>(i), "text"},
					*cfgs.link_text(links[i]));

			// if no title set, set the url as title
			if (cfgs.link_title(links[i]))
			{
				ret.Set({"links", static_cast<int>(i), "title"},
						*cfgs.link_title(links[i]));
			}
			else
			{
				ret.Set({"links", static_cast<int>(i), "title"},
						*cfgs.link_url(links[i]));
			}
		}
	}

	return ret;
}

void CreateHTML(const ConstMetadata &merged, const ConfigCollection &cfgs)
{
	LOG_INFO("CreateHTML: Separating posts and pages.");
	ConstArchive posts;
	ConstArchive pages;
	for (SingleItem::ConstPtr c : merged.all_items)
	{
		if (c->type == ItemType::Page)
		{
			pages.push_back(c);
		}
		else if (c->type == ItemType::Post)
		{
			posts.push_back(c);
		}
		else
		{
			THROW_FATAL("CreateHTML: Unknown type in '%1'. "
						"This should have been found at 'CheckMetadata().'"
						"Please fill a Bugreport at %2%.",
						c->s_filename.string(), BUGTRACKER);
		}
	}
	std::sort(std::begin(posts), std::end(posts), time_greater);
	std::sort(std::begin(pages), std::end(pages), time_greater);

	if (!fs::exists(cfgs.outdir_root()))
	{
		LOG_INFO("Creating outdir.");
		fs::create_directories(cfgs.outdir_root());
	}
	else if (!fs::is_directory(cfgs.outdir_root()))
	{
		THROW_ERROR("'%1%' is not a directory.", cfgs.outdir_root().string());
	}

	PRINT("Cleanup '%1%'.", cfgs.outdir_root());
	CleanupDirectory(cfgs.outdir_root());

	// generate common Template data
	LOG_DEBUG("Generating common Template data (sidebars, categories, series "
			  "and tags).");
	TemplateData common_tpl_data{GenerateCommonTemplateData(merged, cfgs)};
	TemplateWrapper engine(cfgs.tpldir());

	PRINT("Sorting pages.");
	std::sort(std::begin(pages), std::end(pages),
			  [](SingleItem::ConstPtr lhs, SingleItem::ConstPtr rhs) -> bool {
				  return lhs->i_position < rhs->i_position;
			  });

	PRINT("Writing %1% posts and %2% pages with %3% threads ...", posts.size(),
		  pages.size(), cfgs.num_threads());
	AsyncWorker<std::string, void> creator(cfgs.num_threads());

	auto start_time = std::chrono::steady_clock::now();
	int active_page = 0;
	for (const SingleItem::ConstPtr si : pages)
	{
		std::function<void()> fo = [si, active_page, &cfgs, &engine,
									&common_tpl_data]() {
			CreatePage(si, active_page, cfgs, engine, common_tpl_data);
		};
		creator.Add(si->s_filename.string(), fo);
		++active_page;
	}

	for (const SingleItem::ConstPtr si : posts)
	{
		std::function<void()> fo = [si, &merged, &cfgs, &engine,
									&common_tpl_data]() {
			auto pos = merged.series.find(si->s_series);
			if (pos != merged.series.end())
			{
				CreatePost(si, pos->second, cfgs, engine, common_tpl_data);
			}
			else
			{
				CreatePost(si, ConstArchive(), cfgs, engine, common_tpl_data);
			}
		};
		creator.Add(si->s_filename.string(), fo);
	}

	// create archives
	for (const auto &c : merged.archives)
	{
		std::function<void()> fo = [c, &cfgs, &engine, &common_tpl_data]() {
			const ArchiveData &ad = c.first;
			const ConstArchive &ar = c.second;

			CreateArchive(ad.path, ar, cfgs, engine, common_tpl_data);
		};

		std::string name = c.first.path.string();
		creator.Add(name, fo);
	}

	// create Index
	creator.Add("index.html",
				[&posts, &pages, &cfgs, &engine, &common_tpl_data]() {
					CreateIndex(posts, pages, cfgs, engine, common_tpl_data);
				});

	auto result = creator.GetResults();

	PRINT("Copy static files.");
	fs::copy(cfgs.tpldir() / cfgs.rel_path_static(),
			 cfgs.outdir_root() / cfgs.rel_path_static());

	auto end_time = std::chrono::steady_clock::now();

	PRINT("Generated %1% files in %2% milliseconds.", result.size(),
		  std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
																start_time)
			  .count());
}
