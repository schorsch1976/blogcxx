#include "Backend.h"

#include <algorithm>
#include <ctime>

#ifdef _WIN32
#include <windows.h>

#include <VersionHelpers.h> // getversion for color output
#endif


namespace Log
{
namespace impl
{

std::string to_iso_extended_string(time_point_t tp)
{
	using namespace std;
	using namespace std::chrono;

	auto ttime_t = system_clock::to_time_t(tp);
	auto tp_sec = system_clock::from_time_t(ttime_t);
	microseconds ms = duration_cast<microseconds>(tp - tp_sec);

	std::tm ttm;
	ttm = *localtime(&ttime_t);

	char date_time_format[] = "%Y.%m.%dT%H:%M:%S";

	char time_str[] = "yyyy.mm.dd.HH-MM.SS.ffffff";

	strftime(time_str, strlen(time_str), date_time_format, &ttm);

	string result(time_str);
	result.append(".");
	auto sms = to_string(ms.count());
	while (sms.size() < 6)
	{
		sms = "0" + sms;
	}
	result.append(sms);
	return result;
}

Backend::Backend()
	:	m_color_available(true),
		m_ofs("debug.txt"),
		m_run(true)
{
	// https://docs.microsoft.com/de-de/windows/desktop/SysInfo/version-helper-apis
#ifdef _WIN32
	// just on Win10 is color available
	m_color_available = IsWindowsVersionOrGreater(10, 0, 0);
#endif

	// start thread
	m_thread = std::thread(std::bind(&Backend::Execute, this));
}

Backend::~Backend()
{
	m_run = false;
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void Backend::SetFileVerbosity(Level lvl)
{
	lock_t lk(m_monitor);
	m_file_verbosity = lvl;
}
void Backend::SetConsoleVerbosity(Level lvl)
{
	lock_t lk(m_monitor);
	m_console_verbosity = lvl;
}

void Backend::Add(Level lvl, std::string msg)
{
	Entry e;
	e.time = clock_t::now();
	e.lvl = lvl;
	e.msg = msg;

	lock_t lk(m_monitor);
	m_queue.push_back(e);
};

// the thread function
void Backend::Execute()
{
	while(m_run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// get the work from the queue
		std::vector<Entry> work;
		{
			lock_t lk(m_monitor);
			std::swap(work, m_queue);
		}

		for (auto& e : work)
		{
			SendToConsole(e);
			SendToFile(e);
		}
	}
}

void Backend::SendToConsole(const Entry& e)
{
	// ----------------------------------------------------------------------------
	// enum for colorizing the output on the consoloe
	// ----------------------------------------------------------------------------
#ifdef UNIX
	// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
	static const char *strings[] = {
		"\033[1;37m", // std = white
		"\033[1;34m", // blue
		"\033[1;33m", // yellow
		"\033[1;36m", // cyan
		"\033[1;31m"  // red
	};
#endif
#ifdef _WIN32
	static const char *strings[] = {
		"^<ESC^>[37m [37m", // std = white
		"^<ESC^>[34m [34m", // blue
		"^<ESC^>[33m [33m", // yellow
		"^<ESC^>[36m [36m", // cyan
		"^<ESC^>[31m [31m"  // red
	};
#endif
	enum class Color
	{
		std = 0,
		blue,
		yellow,
		cyan,
		red
	} color;
	switch (e.lvl)
	{
	case Level::fatal:
		color = Color::cyan;
		break;
	case Level::error:
		color = Color::red;
		break;
	case Level::warning:
		color = Color::yellow;
		break;
	default:
		color = Color::std;
		break;
	}

	// warning, error and fatal must be seen by the user
	if (e.lvl < Level::warning && e.lvl < m_console_verbosity)
	{
		return;
	}

	if (m_color_available)
	{
		std::cout << strings[static_cast<int>(color)];
	}
	std::cout << e.msg << std::endl;
}
void Backend::SendToFile(const Entry& e)
{
	if (e.lvl < m_file_verbosity)
	{
		return;
	}

	static const char *strings[] = {
		"trace  ",
		"debug  ",
		"info   ",
		"warning",
		"error  ",
		"fatal   "
	};

	int idx = static_cast<int>(e.lvl);
	if (idx < 0)
	{
		idx = 0;
	}
	if (idx >= sizeof(strings) / sizeof(*strings))
	{
		idx = sizeof(strings) / sizeof(*strings) - 1;
	}
	m_ofs	<< "[" << to_iso_extended_string(e.time)   << "]"
			<< "[" << strings[idx] << "] "
			<< e.msg << std::endl << std::flush;
}

} // ns impl
} // ns Log
