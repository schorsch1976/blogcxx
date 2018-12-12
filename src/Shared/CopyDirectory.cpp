#include "CopyDirectory.h"

#include "Shared/Debug.h"

void CopyDirectory(const fs::path &from, const fs::path &to)
{
	if (!fs::exists(from))
	{
		LOG_ERROR("CopyDirectory: 'from' does not exists.");
		return;
	}
	if (!fs::exists(to))
	{
		LOG_ERROR("CopyDirectory: 'to' does not exists.");
		return;
	}
	if (!fs::is_directory(from))
	{
		LOG_ERROR("CopyDirectory: 'from' is no directory.");
		return;
	}
	if (!fs::is_directory(to))
	{
		LOG_ERROR("CopyDirectory: 'to' is no directory.");
		return;
	}

	// create the directories
	for (fs::directory_iterator end_dir_it, it(from); it != end_dir_it; ++it)
	{
		try
		{
			const fs::path &src = it->path();
			if (!fs::is_directory(src))
			{
				fs::copy(src, to / src.filename());
				continue;
			}
			else
			{
				// directory, recursion
				fs::create_directories(to / src.filename());
				CopyDirectory(src, to / src.filename());
			}
		}
		catch (fs::filesystem_error &e)
		{
			// Something went wrong here.
			LOG_ERROR("CopyDirectory: %1%", e.what());
		}
	}
}
