/*
 * blogcxx :: https://www.blogcxx.de
 * Debug logging functionality class [header].
 */

#pragma once

#include <boost/log/trivial.hpp>

namespace Log
{
using Level = boost::log::trivial::severity_level;

// ----------------------------------------------------------------------------
// this exception is used to indicate that it was printed by THROW_xxxx
// ----------------------------------------------------------------------------
class THROWN
{
public:
	THROWN() = default;
};

// ----------------------------------------------------------------------------
// instatiate this to setup the logging
// ----------------------------------------------------------------------------
struct Status
{
	Status();
	void SetFileVerbosity(Log::Level verbosity);
	void SetConsoleVerbosity(Log::Level verbosity);
	~Status();
};

} // namespace Debug

#include "Log-Impl.h"

// ----------------------------------------------------------------------------
// variadic template that uses typesafe operations. Just use the classical
// printf style %d types or %1% %2% to order them. Boost::format will take care
// of the type safety
// ----------------------------------------------------------------------------
template <typename T, typename... Args>
void LOG_TRACE(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::trace>(fmt, args...);
}

template <typename T, typename... Args>
void LOG_DEBUG(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::debug>(fmt, args...);
}

template <typename T, typename... Args>
void LOG_INFO(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::info>(fmt, args...);
}

template <typename T, typename... Args>
void LOG_WARN(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::warning>(fmt, args...);
}

template <typename T, typename... Args>
void LOG_ERROR(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::error>(fmt, args...);
}

template <typename T, typename... Args>
void LOG_FATAL(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::fatal>(fmt, args...);
}

// we need this defines always
template <typename T, typename... Args>
void PRINT(const T &fmt, Args... args)
{
	Log::impl::LOG_IMPL<Log::Level::info>(fmt, args...);
}

template <typename T, typename... Args>
void THROW_ERROR(const T &fmt, Args... args)
{
	Log::impl::THROW<Log::Level::error>(fmt, args...);
}

template <typename T, typename... Args>
void THROW_FATAL(const T &fmt, Args... args)
{
	Log::impl::THROW<Log::Level::fatal>(fmt, args...);
}
