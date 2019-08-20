/*
* blogcxx :: https://www.blogcxx.de
* Debug logging Backend
*/

#pragma once

#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Log.h"

namespace Log
{
namespace impl
{

// ---------------------------------------------------------
// Time/chrono utils
// ---------------------------------------------------------
using clock_t = std::chrono::system_clock;
using duration_t = std::chrono::nanoseconds;
using time_point_t = std::chrono::time_point<clock_t>;

std::string to_iso_extended_string(time_point_t tp);

// ---------------------------------------------------------
// An entry on the queue
// ---------------------------------------------------------
struct Entry
{
	time_point_t time;
	Level lvl;
	std::string msg;
};

// ---------------------------------------------------------
// the backend
// ---------------------------------------------------------
class Backend
{
public:
	Backend();
	~Backend();

	void SetFileVerbosity(Level lvl);
	void SetConsoleVerbosity(Level lvl);

	void Add(Level lvl, std::string msg);

private:
	// the thread function
	void Execute();
	void SendToConsole(const Entry &e);
	void SendToFile(const Entry &e);

	std::vector<Entry> m_queue;
	bool m_color_available;

	Level m_console_verbosity;
	Level m_file_verbosity;
	std::ofstream m_ofs;

	// thread must be last
	using lock_t = std::unique_lock<std::mutex>;
	mutable std::mutex m_monitor;
	std::atomic<bool> m_run;
	std::thread m_thread;
};

} // ns impl
} // ns Log
