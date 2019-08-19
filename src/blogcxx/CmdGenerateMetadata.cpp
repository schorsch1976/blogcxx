/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CmdGenerateMetadata.h"

#include "Shared/AsyncWorker.h"
#include "Shared/Helpers.h"
#include "Shared/Metadata.h"
#include "Shared/SingleItem.h"
#include "Shared/constants.h"
#include "Shared/filesystem.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iterator>
#include "Shared/regex.h"
#include <sstream>
#include <string>

#include <boost/optional/optional_io.hpp>

#include "FilesToMetadata/CollectPostData.h"

namespace
{

void PrintInfo(const ConfigCollection &cfgs)
{
	PRINT("Site generation started.\n\n");

	PRINT("Max. concurrent threads:  %1%.\n", cfgs.num_threads());

	PRINT("Site title:         %1%", cfgs.sitetitle());
	PRINT("Site subtitle:      %1%", cfgs.subtitle());
	PRINT("Site URL:           %1%", cfgs.url(""));
	PRINT("Incoming directory: %1%", cfgs.indir());
	PRINT("Outgoing directory: %1%", cfgs.outdir_root());
	PRINT("Path for articles:  %1%", cfgs.indir_posts().string());
	PRINT("Path for pages:     %1%", cfgs.indir_pages().string());
	PRINT("Localization:       %1%", cfgs.locale());
	PRINT("HTML template:      %1%", cfgs.tpldir().string());
}

AllFilePaths GatherFileNames(const ConfigCollection &cfgs)
{
	AsyncWorker<fs::path, std::vector<fs::path>>::DoneT all_result;

	while (all_result.empty())
	{
		using func_t = std::function<std::vector<fs::path>(fs::path)>;

		func_t func = [](fs::path p) -> std::vector<fs::path> {
			std::vector<fs::path> ret;

			for (fs::recursive_directory_iterator it(p), end; it != end; ++it)
			{
				if (!fs::is_directory(it->path()))
				{
					ret.push_back(it->path());
				}
			}

			return ret;
		};

		AsyncWorker<fs::path, std::vector<fs::path>> gatherer(
			cfgs.num_threads());
		gatherer.Add(cfgs.indir_posts(), std::bind(func, cfgs.indir_posts()));
		gatherer.Add(cfgs.indir_pages(), std::bind(func, cfgs.indir_pages()));

		try
		{
			all_result = gatherer.GetResults();
		}
		catch (...)
		{
			LOG_WARN("No files found.");
			throw NoFilesFound();
		}
	}

	AllFilePaths ret;
	ret.posts = all_result[cfgs.indir_posts()];
	ret.pages = all_result[cfgs.indir_pages()];
	return ret;
}

} // namespace

Metadata CmdGenerateMetadata(const ConfigCollection &cfgs)
{
	PrintInfo(cfgs);
	AllFilePaths all_in_files;

	LOG_INFO("Processing started with %1% threads.", cfgs.num_threads());
	LOG_INFO("Output directory: %1%.", cfgs.outdir_root().string());

	LOG_INFO("Gathering files.");
	all_in_files = GatherFileNames(cfgs);

	LOG_INFO("Collecting all posts and pages...");
	return CollectPostData(cfgs, all_in_files);
}
