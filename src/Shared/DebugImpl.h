/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include <stdexcept>

#include <boost/format.hpp>
#include <boost/log/core.hpp>

#include <boost/log/sources/severity_feature.hpp>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <boost/locale.hpp>

namespace Debug
{

namespace impl
{

namespace logging = boost::log;
namespace keywords = logging::keywords;
namespace sinks = logging::sinks;
namespace expr = logging::expressions;
namespace attributes = logging::attributes;

// ----------------------------------------------------------------------------
// enum for colorizing the output on the consoloe
// ----------------------------------------------------------------------------
enum class Color
{
	std = 0,
	blue,
	yellow,
	cyan,
	red
};

// ----------------------------------------------------------------------------
// typesave format a message with boost::format
// ----------------------------------------------------------------------------

// this is the termination function.
inline std::string Format(boost::format &f) { return f.str(); }

template <typename T, typename... Args>
std::string Format(boost::format &f, T v, Args... args)
{
	f % v;
	return Format(f, args...);
}

// ----------------------------------------------------------------------------
// the logging
// ----------------------------------------------------------------------------
namespace logging = boost::log;

// access to the two loggers
boost::log::sources::logger_mt &ConsoleLogger(Debug::impl::Color color);
boost::log::sources::severity_logger_mt<Debug::Level> &FileLogger();

enum class MsgType
{
	Print = 0,
	Log
};
BOOST_LOG_ATTRIBUTE_KEYWORD(msg_type, "MsgType", MsgType)
BOOST_LOG_ATTRIBUTE_KEYWORD(color, "Color", Color)

template <typename T, typename... Args>
void PRINT(const Color color, const T &fmt, Args... args)
{
	boost::format f(boost::locale::translate(fmt));
	BOOST_LOG(ConsoleLogger(color)) << Format(f, args...);
}

template <Debug::Level lvl, typename T, typename... Args>
void LOG_IMPL(const T &fmt, Args... args)
{
	// warning, error and fatal must be seen by the user
	if (lvl == Debug::Level::warning || lvl == Debug::Level::error ||
		lvl == Debug::Level::fatal)
	{
		Color color = Color::std;
		switch (lvl)
		{
		case Debug::Level::fatal:
			color = Color::cyan;
			break;
		case Debug::Level::error:
			color = Color::red;
			break;
		case Debug::Level::warning:
			color = Color::yellow;
			break;
		default:
			break;
		}
		impl::PRINT(color, fmt, args...);
	}
	boost::format f(boost::locale::translate(fmt));
	BOOST_LOG_SEV(FileLogger(), lvl) << Format(f, args...);
}

template <Debug::Level lvl, typename T, typename... Args>
void THROW(const T &fmt, Args... args)
{
	LOG_IMPL<lvl>(fmt, args...);
	throw THROWN();
}

} // namespace impl

} // namespace Debug
