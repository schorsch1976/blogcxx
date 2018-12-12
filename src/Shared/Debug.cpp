/*
 * blogcpp :: https://www.blogcpp.org
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

namespace
{
using namespace Debug;
using namespace Debug::impl;

#ifdef WITH_DEBUGLOG
// the file sink
typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink_t;
static boost::shared_ptr<file_sink_t> sp_file_sink;
static std::once_flag s_once_file;
#endif

// the console sink (for PRINT and ERRORS)
typedef sinks::synchronous_sink<sinks::text_ostream_backend> console_sink_t;
static boost::shared_ptr<console_sink_t> sp_console_sink;
static std::once_flag s_once_console;

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
		sp_console_sink->set_formatter(expr::stream << expr::smessage);
	});

// setup the debug log file sink
#ifdef WITH_DEBUGLOG
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
#endif
}
Status::~Status()
{
	boost::shared_ptr<logging::core> core = logging::core::get();

#ifdef WITH_DEBUGLOG
	if (sp_file_sink)
	{
		core->remove_sink(sp_file_sink);
		sp_file_sink->flush();
		sp_file_sink.reset();
	}
#endif
	if (sp_console_sink)
	{
		core->remove_sink(sp_console_sink);
		sp_console_sink->flush();
		sp_console_sink.reset();
	}
}

void Status::SetVerbosity(Debug::Level verbosity)
{
#ifdef WITH_DEBUGLOG
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
#endif
}

} // namespace Debug

namespace Debug
{
namespace impl
{
static boost::shared_ptr<boost::log::sources::logger_mt> sp_logger_console;
static boost::shared_ptr<boost::log::sources::severity_logger_mt<Debug::Level>>
	sp_file_logger;

// access to the two loggers
boost::log::sources::logger_mt &ConsoleLogger()
{
	static std::once_flag s_once;
	std::call_once(s_once, []() {
		sp_logger_console =
			boost::make_shared<boost::log::sources::logger_mt>();
		sp_logger_console->add_attribute(
			"MsgType",
			boost::log::attributes::constant<MsgType>(MsgType::Print));
	});
	return *sp_logger_console;
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
	});
	return *sp_file_logger;
}
} // namespace impl
} // namespace Debug