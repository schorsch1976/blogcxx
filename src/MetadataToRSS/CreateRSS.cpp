#include "CreateRSS.h"

#include "RSSGeneratorClass.h"
/*
* blogcpp :: https://www.blogcpp.org
*/

#include "Shared/AsyncWorker.h"

void CreateRSS(const ConstMetadata &merged, const ConfigCollection &cfgs)
{
	PRINT("CreateRSS with %1% threads.", cfgs.num_threads());

	if (!fs::exists(cfgs.feeddir()))
	{
		// Create the RSS directory.
		LOG_WARN("Directory %1% is missing. Creating it.", cfgs.feeddir());
		fs::create_directories(cfgs.feeddir());
	}

	AsyncWorker<std::string, void> creator(cfgs.num_threads());

	for (auto& ar : merged.archives)
	{
		std::function<void()> fo = [& cfgs, ar]() {
			fs::path outfile = cfgs.feeddir();
			outfile /= (ar.first.path.stem().string() + ".xml");
			RSSGenerator generator(outfile, cfgs.sitetitle(), cfgs);
			generator.createRSS(ar.first, ar.second);
		};

		creator.Add(ar.first.path.string(), fo);
	}

	// Create the RSS feed:
	creator.GetResults();
}
