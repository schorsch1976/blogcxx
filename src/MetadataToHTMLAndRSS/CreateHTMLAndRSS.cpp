/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Log/Log.h"

#include "CreateHTMLAndRSS.h"

#include "Shared/AsyncWorker.h"
#include "Shared/CleanupDirectory.h"
#include "Shared/CopyDirectory.h"
#include "Shared/Helpers.h"
#include "Shared/constants.h"
#include "Shared/filesystem.h"

#include <algorithm>
#include <chrono>

#include "CreateArchive.h"
#include "CreateIndex.h"
#include "CreatePage.h"
#include "CreatePost.h"
#include "CreateRSS.h"

#include "TemplateWrapper.h"

TemplateData GenerateCommonTemplateData(const TemplateWrapper &engine,
										const ConstMetadata &merged,
										const ConfigCollection &cfgs)
{
	int i = 0;
	TemplateData ret;

	ret.Set({"sitetitle"}, cfgs.sitetitle());
	ret.Set({"subtitle"}, cfgs.subtitle() ? *cfgs.subtitle() : "");

	// rsslink
	ret.Set({"rsslink"}, cfgs.url(cfgs.rel_path_feed() / "RSS.xml"));
	ret.Set({"baseurl"}, cfgs.url(""));

	ret.Set({"hljsver"}, HIGHLIGHT_VER);

	ret.Set({"defaultclass"}, "");
	ret.Set({"feedmeta"}, "");
	ret.Set({"opengraphmeta"}, "");
	ret.Set({"prevmeta"}, "");
	ret.Set({"nextmeta"}, "");
	ret.Set({"keywordsmeta"}, "");

	ret.Set({"blogcxx"}, APP_URL);

	// pagenav
	{
		i = 0;
		for (auto &p : merged.all_pages)
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
	for (auto &si : merged.all_posts)
	{
		latest_posts.push_back(si);
	}
	while (static_cast<int>(latest_posts.size()) > cfgs.maxhistory())
	{
		latest_posts.pop_back();
	}

	i = 0;
	for (auto &si : latest_posts)
	{
		ret.Set({"latestposts", i, "title"}, si->s_title);
		ret.Set({ "latestposts", i, "date" }, dateToPrint(si->time));
		ret.Set({"latestposts", i, "link"},
				cfgs.url(cfgs.rel_path_posts(si->s_slug)));
		++i;
	}

	// archives
	{
		std::map<year_t, std::vector<ArchiveData>> archives_years_months;
		for (auto &ar : merged.archives)
		{
			const ArchiveData &ad = ar.first;
			switch (ad.type)
			{
				// because the year month archives get grouped by year
				case ArchiveType::YearMonth:
					archives_years_months[ad.time.date().year()].push_back(ad);
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
						std::to_string(ad.time.date().month()));
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

	// render the comment templates
	if (cfgs.commenttype() && cfgs.commentdata())
	{
		TemplateData header;
		header.Set({"header"}, "header");
		header.Set({"commentdata"}, *cfgs.commentdata());
		std::string header_data = engine.Render(
			cfgs.commentdir() / std::string(*cfgs.commenttype() + ".txt"),
			header);

		TemplateData body;
		body.Set({"body"}, "body");
		body.Set({"commentdata"}, *cfgs.commentdata());
		std::string body_data = engine.Render(
			cfgs.commentdir() / std::string(*cfgs.commenttype() + ".txt"),
			body);

		ret.Set({"comments-header"}, header_data);
		ret.Set({"comments-body"}, body_data);
	}
	else
	{
		ret.Set({"comments-header"}, "<!-- No comments -->");
		ret.Set({"comments-body"}, "<!-- No comments -->");
	}
	return ret;
}

// this function adds function objects for all asyncronous tasks
// to the AsyncWorker. If some things must be done prior the jobs,
// this function handles them too. Like Creation and preparation
// of output directories
void CreateHTMLAndRSS(const ConstMetadata &merged, const ConfigCollection &cfgs)
{
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
	TemplateWrapper engine(
		{cfgs.tpldir(), cfgs.commentdir(), cfgs.tpl_RSS().parent_path()});
	TemplateData common_tpl_data{
		GenerateCommonTemplateData(engine, merged, cfgs)};

	PRINT("Writing %1% posts and %2% pages with %3% threads ...",
		  merged.all_posts.size(), merged.all_pages.size(), cfgs.num_threads());
	AsyncWorker<std::string, void> creator(cfgs.num_threads());

	auto start_time = std::chrono::steady_clock::now();
	int active_page = 0;
	for (const SingleItem::ConstPtr si : merged.all_pages)
	{
		std::function<void()> fo = [si, active_page, &cfgs, &engine,
									&common_tpl_data]() {
			CreatePage(si, active_page, cfgs, engine, common_tpl_data);
		};
		creator.Add(si->s_filename.string(), fo);
		++active_page;
	}

	// create Posts and Pages
	for (const SingleItem::ConstPtr si : merged.all_posts)
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

			std::string feed_url =
				cfgs.url(cfgs.rel_path_feed() / cfgs.feed_file(ad));
			CreateArchive(ad.path, ar, cfgs, engine, feed_url, common_tpl_data);
		};

		std::string name = c.first.path.string();
		creator.Add(name, fo);
	}

	// create Index(0-n)
	{
		// order the posts, put stickes (ordered by date) to the front
		ConstArchive sorted_posts = merged.all_posts;
		std::sort(std::begin(sorted_posts), std::end(sorted_posts),
				  [](SingleItem::ConstPtr lhs, SingleItem::ConstPtr rhs) {
					  // both sticky? Just sort by time
					  if (lhs->b_sticky && rhs->b_sticky)
					  {
						  return time_greater(lhs, rhs);
					  }
					  // one sticky? Just put the sticky one first
					  if (lhs->b_sticky && !rhs->b_sticky)
					  {
						  return true;
					  }
					  if (!lhs->b_sticky && rhs->b_sticky)
					  {
						  return false;
					  }

					  // both non sticky? Just put the newer one first
					  return time_greater(lhs, rhs);
				  });

		const size_t max_items = static_cast<size_t>(cfgs.maxitems());
		const size_t page_count = sorted_posts.size() % max_items != 0
									  ? sorted_posts.size() / max_items + 1
									  : sorted_posts.size() / max_items;

		const auto pages = merged.all_pages;

		size_t current_post = 0;
		for (size_t page = 0; page < page_count; ++page)
		{
			ConstArchive posts;
			for (size_t post = 0;
				 post < max_items && current_post < sorted_posts.size();
				 ++post, ++current_post)
			{
				posts.push_back(sorted_posts[current_post]);
			}

			creator.Add("index-" + std::to_string(page) + ".html",
						[page, page_count, posts, pages, &cfgs, &engine,
						 &common_tpl_data]() {
							CreateIndex(page, page_count, posts, pages, cfgs,
										engine, common_tpl_data);
						});
		}
	}

	// add copy static files
	creator.Add("CopyStaticFiles", [&cfgs]() {
		PRINT("Copy static files.");

		fs::create_directories(cfgs.outdir_root() / cfgs.rel_path_static());

		CopyDirectory(cfgs.tpldir() / cfgs.rel_path_static(),
					  cfgs.outdir_root() / cfgs.rel_path_static());
	});

	// add copy static media
	creator.Add("CopyMedia", [&cfgs]() {
		if (fs::exists(cfgs.indir() / cfgs.rel_path_media()))
		{
			PRINT("Copying media files.");
			fs::create_directories(cfgs.outdir_root() / cfgs.rel_path_media());

			CopyDirectory(cfgs.indir() / cfgs.rel_path_media(),
				cfgs.outdir_root() / cfgs.rel_path_media());
		}
	});

	// RSS: archive feeds
	{
		fs::path feed_out_dir = cfgs.outdir_root() / cfgs.rel_path_feed();
		if (!fs::exists(feed_out_dir))
		{
			// Create the RSS directory.
			LOG_WARN("Directory %1% is missing. Creating it.",
					 feed_out_dir.string());
			fs::create_directories(feed_out_dir);
		}

		for (auto &ar : merged.archives)
		{
			// no RSS feeds for Year and Year/month
			if (ar.first.type == ArchiveType::Year ||
				ar.first.type == ArchiveType::YearMonth)
			{
				continue;
			}

			std::function<void()> fo = [&cfgs, &engine, ar, feed_out_dir]() {
				fs::path outfile = feed_out_dir / cfgs.feed_file(ar.first);
				CreateRSS(outfile, cfgs.sitetitle(), ar.second, cfgs, engine);
			};

			creator.Add("RSS/" + ar.first.path.string(), fo);
		}

		// RSS: total feed
		{
			auto posts = merged.all_posts;

			std::function<void()> fo = [&cfgs, &engine, posts, feed_out_dir]() {
				fs::path outfile = feed_out_dir / "RSS.xml";
				CreateRSS(outfile, cfgs.sitetitle(), posts, cfgs, engine);
			};

			creator.Add("RSS/RSS.xml", fo);
		}
	}

	auto result = creator.GetResults();

	auto end_time = std::chrono::steady_clock::now();

	PRINT("Generated %1% files in %2% milliseconds.", result.size(),
		  std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
																start_time)
			  .count());
}
