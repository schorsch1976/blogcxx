/*
 * blogcxx :: https://www.blogcxx.de
 * Debug logging functionality class [header].
 */

#include <iomanip>
#include <mutex> // once flag
#include <vector>
#include <chrono>

#include "Log.h"
#include "Backend.h"

namespace
{
using namespace Log;
using namespace Log::impl;

static std::once_flag s_once_backend;
std::shared_ptr<Log::impl::Backend> sp_backend;

} // namespace

// public interface
namespace Log
{
Status::Status()
{
	std::call_once(s_once_backend, [=]() {
		sp_backend = std::make_shared<impl::Backend>();
	});
}
Status::~Status()
{
	if (sp_backend)
	{
		sp_backend.reset();
	}
}

void Status::SetFileVerbosity(Log::Level verbosity)
{
	if (sp_backend)
	{
		sp_backend->SetFileVerbosity(verbosity);
	}
}

void Status::SetConsoleVerbosity(Log::Level verbosity)
{
	if (sp_backend)
	{
		sp_backend->SetConsoleVerbosity(verbosity);
	}
}

} // namespace Log

namespace Log
{
namespace impl
{

void IMPLTobackend(Log::Level lvl, std::string msg)
{
	if (sp_backend)
	{
		sp_backend->Add(lvl, msg);
	}
}

} // namespace impl
} // namespace Log