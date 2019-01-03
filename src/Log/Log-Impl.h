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

namespace Log
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

// access to the logger
boost::log::sources::severity_logger_mt<Log::Level> &Logger(Log::impl::Color color);

BOOST_LOG_ATTRIBUTE_KEYWORD(color, "Color", Color)

template <Log::Level lvl, typename T, typename... Args>
void LOG_IMPL(const T &fmt, Args... args)
{
	Color color = Color::std;
	switch (lvl)
	{
		case Log::Level::fatal:
			color = Color::cyan;
			break;
		case Log::Level::error:
			color = Color::red;
			break;
		case Log::Level::warning:
			color = Color::yellow;
			break;
		default:
			color = Color::std;
			break;
	}

	boost::format f(boost::locale::translate(fmt));
	BOOST_LOG_SEV(Logger(color), lvl) << Format(f, args...);
}

template <Log::Level lvl, typename T, typename... Args>
void THROW(const T &fmt, Args... args)
{
	LOG_IMPL<lvl>(fmt, args...);
	throw THROWN();
}

} // namespace impl

} // namespace Log
