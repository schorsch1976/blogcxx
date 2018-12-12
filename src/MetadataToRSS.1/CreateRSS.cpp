#include "CreateRSS.h"

#include "RSSGeneratorClass.h"
/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Shared/AsyncWorker.h"
#include "Shared/Helpers.h"

void CreateRSS(const ConstMetadata &merged, const ConfigCollection &cfgs)
{
	PRINT("CreateRSS with %1% threads.", cfgs.num_threads());

	fs::path feed_out_dir = cfgs.outdir_root() / cfgs.rel_path_feed();
	if (!fs::exists(feed_out_dir))
	{
		// Create the RSS directory.
		LOG_WARN("Directory %1% is missing. Creating it.",
				 feed_out_dir.string());
		fs::create_directories(feed_out_dir);
	}

	AsyncWorker<std::string, void> creator(cfgs.num_threads());

	// archive feeds
	for (auto &ar : merged.archives)
	{
		// no RSS feeds for Year and Year/month
		if (ar.first.type == ArchiveType::Year ||
			ar.first.type == ArchiveType::YearMonth)
		{
			continue;
		}

		std::function<void()> fo = [&cfgs, ar, feed_out_dir]() {
			fs::path outfile = feed_out_dir / cfgs.feed_file(ar.first);
			RSSGenerator generator(outfile, cfgs.sitetitle(), cfgs);
			generator.createRSS(ar.second);
		};

		creator.Add(ar.first.path.string(), fo);
	}

	// total feed
	{
		auto posts = merged.all_posts;
		std::function<void()> fo = [&cfgs, posts, feed_out_dir]() {
			fs::path outfile = feed_out_dir;
			outfile /= "RSS.xml";
			RSSGenerator generator(outfile, cfgs.sitetitle(), cfgs);
			generator.createRSS(posts);
		};

		creator.Add("RSS.xml", fo);
	}
	// Create the RSS feed:
	creator.GetResults();
}
