/*
 * blogcxx :: https://www.blogcxx.de
 * Debug logging functionality class [header].
 */

#include <iomanip>
#include <mutex> // once flag

#include "Debug.h"
#include "constants.h"

#include <boost/core/null_deleter.hpp>

#include <boost/log/attributes/attribute.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/support/date_time.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#ifdef _WIN32
#include <windows.h>
#include <VersionHelpers.h> // getversion for color output
#endif

namespace
{
using namespace Debug;
using namespace Debug::impl;

// the file sink
typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink_t;
static boost::shared_ptr<file_sink_t> sp_file_sink;
static std::once_flag s_once_file;

// the console sink (for PRINT and ERRORS)
typedef sinks::synchronous_sink<sinks::text_ostream_backend> console_sink_t;
static boost::shared_ptr<console_sink_t> sp_console_sink;
static std::once_flag s_once_console;

// is color available?
static bool sb_color_available = false;

} // namespace

// public interface
namespace Debug
{
// ----------------------------------------------------------------------------
// this exception is used to indicate that it was printed by THROW_xxxx
// ----------------------------------------------------------------------------
const char *THROWN::what() const throw()
{
	return "THROWN by Debug: THROW_(ERROR or DEBUG)";
}

using namespace boost::log;

// see: https://www.boost.org/doc/libs/1_68_0/libs/log/doc/html/log/detailed/expressions.html
// The operator is used when putting the color to log
struct color_tag;

logging::formatting_ostream& operator<<
(
	logging::formatting_ostream& strm,
	logging::to_log_manip< Color, color_tag > const& manip
)
{
#ifdef UNIX
	// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
	static const char* strings[] =
	{
		"\033[1;37m",	// std = white
		"\033[1;34m",	// blue
		"\033[1;33m",	// yellow
		"\033[1;36m",	// cyan
		"\033[1;31m"	// red
	};

	Color color = manip.get();
	if (static_cast<std::size_t>(color) < sizeof(strings) / sizeof(*strings))
	{
		strm << strings[static_cast<int>(color)];
	}
	else
	{
		// else red
		strm << strings[static_cast<int>(Color::red)];
	}

#endif
#ifdef _WIN32
	static std::once_flag s_detect_version;
	std::call_once(s_detect_version, []()
	{
		// https://docs.microsoft.com/de-de/windows/desktop/SysInfo/version-helper-apis

		// just on Win10 is color available
		sb_color_available = IsWindowsVersionOrGreater(10, 0, 0);
	});

	if (sb_color_available)
	{
		static const char* strings[] =
		{
			"^<ESC^>[37m [37m",	// std = white
			"^<ESC^>[34m [34m",	// blue
			"^<ESC^>[33m [33m",	// yellow
			"^<ESC^>[36m [36m",	// cyan
			"^<ESC^>[31m [31m"	// red
		};

		Color color = manip.get();
		if (static_cast<std::size_t>(color) < sizeof(strings) / sizeof(*strings))
		{
			strm << strings[static_cast<int>(color)];
		}
		else
		{
			// else red
			strm << strings[static_cast<int>(Color::red)];
		}
	}
#endif
	return strm;
}

Status::Status()
{
	boost::shared_ptr<logging::core> core = logging::core::get();

	logging::add_common_attributes();

	// this finaly changed the behaviour
	// logging::add_console_log(std::cout,
	//						 boost::log::keywords::format = "%Message%");

	// see
	// https://www.boost.org/doc/libs/1_68_0/libs/log/example/doc/tutorial_filtering.cpp
	// https://www.boost.org/doc/libs/1_68_0/libs/log/doc/html/log/tutorial/advanced_filtering.html
	std::call_once(s_once_console, [=]() {
		auto backend = boost::make_shared<sinks::text_ostream_backend>();

		backend->add_stream(
			boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));

		// Wrap it into the frontend and register in the core
		sp_console_sink.reset(new console_sink_t(backend));
		core->add_sink(sp_console_sink);

		// You can manage filtering and formatting through the sink interface
		sp_console_sink->set_filter(expr::attr<MsgType>("MsgType") ==
									MsgType::Print);
		sp_console_sink->set_formatter(expr::stream << expr::attr<Color, color_tag>("Color") << expr::smessage);
	});

	// setup the debug log file sink
	std::call_once(s_once_file, [=]() {
		// Create a backend and initialize it with a stream
		auto backend = boost::make_shared<sinks::text_file_backend>();
		backend->set_file_name_pattern(DEBUGLOGFILE);

		// Wrap it into the frontend and register in the core
		sp_file_sink.reset(new file_sink_t(backend));
		core->add_sink(sp_file_sink);

		// the verbosity gets added later
		sp_file_sink->set_filter(expr::attr<MsgType>("MsgType") ==
								 MsgType::Log);

		// this creates debug entries like
		// [2018-11-27 18:58:20.994303][DEBUG][0x00001770]...
		// clang-format off
			sp_file_sink->set_formatter
				(
					expr::stream
					<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "]"
					<< "[" << std::setw(7) << std::setfill(' ') << expr::attr< logging::trivial::severity_level >("Severity") << "]"
					<< "[" << expr::attr< attributes::current_thread_id::value_type  >("ThreadID") << "] "
					<< expr::message
				);
		// clang-format on
	});
}
Status::~Status()
{
	boost::shared_ptr<logging::core> core = logging::core::get();

	if (sp_file_sink)
	{
		core->remove_sink(sp_file_sink);
		sp_file_sink->flush();
		sp_file_sink.reset();
	}

	if (sp_console_sink)
	{
		core->remove_sink(sp_console_sink);
		sp_console_sink->flush();
		sp_console_sink.reset();
	}
}

void Status::SetVerbosity(Debug::Level verbosity)
{
	using namespace impl;
	if (sp_file_sink)
	{
		if (verbosity == Debug::Level::trace)
		{
			sp_file_sink->reset_filter();
		}
		else
		{
			sp_file_sink->set_filter(
				expr::attr<Debug::Level>("Severity") >= verbosity &&
				expr::attr<MsgType>("MsgType") == MsgType::Log);
		}
	}
}

} // namespace Debug

namespace Debug
{
namespace impl
{
static std::vector<boost::shared_ptr<boost::log::sources::logger_mt>>
	sp_logger_console;
static boost::shared_ptr<boost::log::sources::severity_logger_mt<Debug::Level>>
	sp_file_logger;

// access to the two loggers
boost::log::sources::logger_mt &ConsoleLogger(Debug::impl::Color color)
{
	static std::once_flag s_once;
	std::call_once(s_once, []() {
		for (int i = 0; i < static_cast<int>(Debug::impl::Color::red); ++i)
		{
			auto logger = boost::make_shared<boost::log::sources::logger_mt>();
			logger->add_attribute(
				"MsgType",
				boost::log::attributes::constant<MsgType>(MsgType::Print));
			logger->add_attribute("Color",
								  boost::log::attributes::constant<Color>(
									  static_cast<Debug::impl::Color>(i)));

			sp_logger_console.push_back(logger);
		}
	});

	int idx = static_cast<int>(color);
	assert(idx >= 0 && idx <= static_cast<int>(Debug::impl::Color::red));
	assert(sp_logger_console[idx] != nullptr);
	return *sp_logger_console[idx];
}
boost::log::sources::severity_logger_mt<Debug::Level> &FileLogger()
{
	static std::once_flag s_once;
	std::call_once(s_once, []() {
		sp_file_logger = boost::make_shared<
			boost::log::sources::severity_logger_mt<Debug::Level>>();
		boost::log::sources::severity_logger_mt<Debug::Level> slg;
		sp_file_logger->add_attribute(
			"MsgType", boost::log::attributes::constant<MsgType>(MsgType::Log));
		sp_file_logger->add_attribute(
			"Color", boost::log::attributes::constant<Color>(Color::std));
	});
	return *sp_file_logger;
}
} // namespace impl
} // namespace Debug