/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include <boost/format.hpp>
#include <boost/locale.hpp>

namespace Log
{
namespace impl
{

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
void IMPLTobackend(Log::Level lvl, std::string msg);

template <Log::Level lvl, typename T, typename... Args>
void LOG_IMPL(const T &fmt, Args... args)
{
	boost::format f(boost::locale::translate(fmt));
	IMPLTobackend(lvl, Format(f, args...));
}

template <Log::Level lvl, typename T, typename... Args>
void THROW(const T &fmt, Args... args)
{
	LOG_IMPL<lvl>(fmt, args...);
	throw THROWN();
}

} // namespace impl
} // namespace Log
