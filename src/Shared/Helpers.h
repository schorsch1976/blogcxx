/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include "SingleItem.h"
#include "filesystem.h"

#include <string>
#include <vector>

std::string trim(std::string inputstring);
std::string hyphenise(std::string input);

std::string lowercase(std::string input);
std::string translate(std::string input);

void openWithEditor(fs::path filename);
std::string read_file(fs::path filename);
void write_file(fs::path filename, const unsigned char *data, size_t len);
void write_file(fs::path filename, const std::string &data);

// -----------------------------
// Date/Time Helpers
// -----------------------------

enum class time_fmt
{
	locale_short,
	locale_date_time,
	iso_short,
	iso_date_time,
	rss_date_time
};

std::string dateToPrint(const pt::ptime &time,
						time_fmt fmt = time_fmt::locale_date_time);

bool isFutureDate(pt::ptime time);

bool time_smaller(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
bool time_equal(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
bool time_greater(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
