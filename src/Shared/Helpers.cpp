/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Helpers.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>

#include <regex>
#include <sstream>

#include <boost/locale.hpp>
#include <boost/locale/date_time.hpp>

#include "Debug.h"

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
	auto wsback = find_if_not(inputstring.rbegin(), inputstring.rend(),
							  [](int c) { return isspace(c); })
					  .base();
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

bool strEndsWith(const std::string &s, const std::string &ending)
{
	// true if <s> ends with <ending>.
	return (s.size() >= ending.size()) &&
		   equal(ending.rbegin(), ending.rend(), s.rbegin());
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
	std::remove(std::begin(file_contents), std::end(file_contents), '\r');

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

std::vector<std::string> vectorSplit(std::string inputstring,
									 std::string divider)
{
	// Returns a vector of elements in the <inputstring>.
	std::vector<std::string> ret;

	if (inputstring.size() == 0)
	{
		// Skip empty strings.
		return ret;
	}

	std::regex re(divider); // Tokenize.
	std::sregex_token_iterator it(inputstring.begin(), inputstring.end(), re,
								  -1);
	std::sregex_token_iterator reg_end;

	for (; it != reg_end; ++it)
	{
		std::string toadd = trim(it->str());
		if (toadd.empty())
		{
			// Empty elements could happen if the user writes ";;" or
			// if we have a page which only has tags, no categories. In
			// this case, we should not try to add an "empty item" as
			// that would not make any sense, would it?
			continue;
		}
		ret.push_back(trim(it->str()));
	}

	return ret;
}

// -----------------------------
// Date/Time Helpers
// -----------------------------
std::string timeNow(const char *format)
{
	// Returns a formatted "now()" string. The output format defaults to ISO
	// time.
	std::stringstream ss_time;
	time_t t = time(0);
#if (defined(_WIN32) && !__INTEL_COMPILER)
	// Microsoft wants to have localtime_s here.
	tm now;
	localtime_s(&now, &t);
#else
	tm *now = localtime(&t);
#endif
#ifdef _WIN32
	ss_time << std::put_time(&now, format);
#else
	ss_time << std::put_time(now, format);
#endif
	return ss_time.str();
}

std::string dateToPrint(const pt::ptime& time, bool shortdate /*= false*/)
{
	// we got a global locale
	using namespace boost::locale;

	std::ostringstream oss;
	if (shortdate)
	{
		oss << as::date << time;
	}
	else
	{
		oss << as::datetime << time;
	}

	return oss.str();
}

bool isFutureDate(pt::ptime time)
{
	// Returns true is time is in the future; else, false.
	return time < pt::second_clock::local_time();
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
