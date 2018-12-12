/*
* blogcpp :: https://www.blogcpp.org
*/

#include "CleanupDirectory.h"

#include "Shared/Debug.h"

void CleanupDirectory(fs::path path)
{
	// ------------------------------------------------
	// Walk through the directory and clean it
	// ------------------------------------------------
	LOG_INFO("Cleaning up your old files ...");
	if (!fs::exists(path) || !fs::is_directory(path))
	{
		LOG_WARN("Directory '%1%' does not exists or is no directory.", path.string());
		return;
	}

	for (fs::directory_iterator end_dir_it, it(path); it != end_dir_it; ++it)
	{
		try
		{
			fs::remove_all(it->path());
		}
		catch (fs::filesystem_error &e)
		{
			// Something went wrong here.
			THROW_ERROR("Failed to clean up the existing output files: %1%",
						e.what());
		}
	}
}
