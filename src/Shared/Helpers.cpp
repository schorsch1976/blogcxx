/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Helpers.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>

#include "Shared/regex.h"
#include <sstream>

#include <boost/locale.hpp>
#include <boost/locale/date_time.hpp>

#include "Log/Log.h"

// clang-format off
#ifdef _WIN32
// To open files with their default editor, Windows's API is used.
// However, <windows.h> adds weird versions of std::min and std::max
// if it's not told not to.
#  define NOMINMAX // Tell that.
#  include <windows.h>
#  include <ShellApi.h>
#elif defined(__linux__) || defined(__unix__)
#  include <stdlib.h>
#  include <unistd.h>
#else
// I can't really support everything. However, maybe the checks above
// are just incomplete? There is a bug tracker for you ... wink! ;-)
#  error This file is expected to break on your system. Please examine the source code and check if you could contribute a fix.
#endif
// clang-format on

std::string trim(std::string inputstring)
{
	// Removes leading and trailing whitespaces from <inputstring>.
	auto wsfront = std::find_if_not(inputstring.begin(), inputstring.end(),
									[](int c) { return isspace(c); });
	auto wsback =
		find_if_not(inputstring.rbegin(), inputstring.rend(), [](int c) {
			return isspace(c);
		}).base();
	return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

std::string hyphenise(std::string input)
{
	// Generates a valid file name from an input string.
	const std::string forbiddenChars = "\\/:?\"<>|'";
	transform(
		input.begin(), input.end(), input.begin(), [&forbiddenChars](char ch) {
			// replace all invalid characters by hyphens
			return forbiddenChars.find(ch) != std::string::npos ? '-' : ch;
		});

	return input;
}

std::string lowercase(std::string input)
{
	std::string uinput = boost::locale::to_lower(input);
	return uinput;
}
std::string translate(std::string input)
{
	return boost::locale::translate(input);
}

void openWithEditor(fs::path filename)
{
// Opens <filename> in your default editor.
#ifdef _WIN32
	// Windows version, using the Windows API.
	ShellExecuteA(NULL, "open", filename.string().c_str(), NULL, NULL, SW_SHOW);
#else
	// Other system, other ways...
	std::string editor = "";

	if (getenv("VISUAL") != NULL)
	{
		editor = getenv("VISUAL");
	}
	else if (getenv("EDITOR") != NULL)
	{
		editor = getenv("EDITOR");
	}
	else
	{
#ifdef __linux__
		editor = "xdg-open";
#else
		THROW_ERROR("No standard editor found.");
#endif
	}

// TODO: system(string(editor + " \"" + filename + "\"").c_str()) == 0;
#endif
}

std::string read_file(fs::path filename)
{
	std::string file_contents;
	std::ifstream ifs(filename.string());
	ifs.unsetf(std::ios::skipws);
	std::copy(std::istream_iterator<unsigned char>(ifs),
			  std::istream_iterator<unsigned char>(),
			  std::back_inserter(file_contents));

	std::vector<std::vector<unsigned char>> BOMS{{{0xEF, 0xBB, 0xBF}},
												 {{0xFE, 0xFF}},
												 {{0xFF, 0xFE}},
												 {{0x00, 0x00, 0xFE, 0xFF}},
												 {{0xFF, 0xFE, 0x00, 0x00}}};

	// remote UTF BOM
	for (auto &bom : BOMS)
	{
		if (file_contents.size() > bom.size())
		{
			bool is_bom = true;
			for (size_t i = 0; i < bom.size(); ++i)
			{
				if (static_cast<unsigned char>(file_contents[i]) != bom[i])
				{
					is_bom = false;
				}
			}
			if (is_bom)
			{
				file_contents =
					std::string(std::begin(file_contents) + bom.size(),
								std::end(file_contents));
				break;
			}
		}
	}

	// remove any "\r"
	file_contents.erase(
		std::remove(std::begin(file_contents), std::end(file_contents), '\r'),
		file_contents.end());

	return file_contents;
}

void write_file(fs::path filename, const unsigned char *data, size_t len)
{
	std::ofstream ofs(filename.string(), std::ios::binary);
	if (!ofs.is_open())
	{
		THROW_FATAL("Could not open file for writing: %1%", filename.string());
	}

	ofs.write(reinterpret_cast<const char *>(data), len);
}

void write_file(fs::path filename, const std::string &data)
{
	std::ofstream ofs(filename.string(), std::ios::binary);
	if (!ofs.is_open())
	{
		THROW_FATAL("Could not open file for writing: %1%", filename.string());
	}

	std::ostream_iterator<char> osi(ofs, "");
	std::copy(data.begin(), data.end(), osi);
}

// -----------------------------
// Date/Time Helpers
// -----------------------------
std::string dateToPrint(const pt::ptime &time, time_fmt fmt)
{
	using namespace boost::locale;

	tm tm_t = pt::to_tm(time);
	date_time_period_set s;
	s.add(period::year(tm_t.tm_year + 1900));
	s.add(period::month(tm_t.tm_mon));
	s.add(period::hour(tm_t.tm_hour));
	s.add(period::minute(tm_t.tm_min));
	s.add(period::second(tm_t.tm_sec));

	date_time dt(s);

	std::ostringstream oss;
	switch (fmt)
	{
		case time_fmt::locale_short:
			oss << as::date_short << dt;
			break;
		case time_fmt::locale_date_time:
			oss << as::date_full << dt;
			break;
		case time_fmt::iso_short:
			oss << std::put_time(&tm_t, "%Y-%m-%d");
			break;
		default:
		case time_fmt::iso_date_time:
			oss << std::put_time(&tm_t, "%Y-%m-%d %H:%M:%S");
			break;
		case time_fmt::rss_date_time:
			oss << std::put_time(&tm_t, "%a, %d %b %Y %T %z");
			break;
	};

	return oss.str();
}

bool isFutureDate(pt::ptime time)
{
	// Returns true is time is in the future; else, false.
	return time > pt::second_clock::local_time();
}

bool time_smaller(SingleItem::ConstPtr left, SingleItem::ConstPtr right)
{
	return left->time < right->time;
};

bool time_equal(SingleItem::ConstPtr left, SingleItem::ConstPtr right)
{
	return left->time == right->time;
};

bool time_greater(SingleItem::ConstPtr left, SingleItem::ConstPtr right)
{
	return left->time > right->time;
};
