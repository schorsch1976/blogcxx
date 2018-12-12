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

bool strEndsWith(const std::string &s, const std::string &ending);

std::string lowercase(std::string input);
std::string translate(std::string input);

void openWithEditor(fs::path filename);
std::string read_file(fs::path filename);
void write_file(fs::path filename, const unsigned char *data, size_t len);
void write_file(fs::path filename, const std::string &data);

std::vector<std::string> vectorSplit(std::string inputstring,
									 std::string divider = ";");
bool vectorSort(std::string a, std::string b);

// -----------------------------
// Date/Time Helpers
// -----------------------------

std::string timeNow(const char *format = "%a, %d %b %Y %T %z");

void parseDatestringToTm(std::string in_datetime, std::string inputfile,
						 tm &input_tm);
std::string dateToPrint(const tm &tm_t, bool shortdate = false);

bool isFutureDate(tm tm_im);

std::string parseTmToPath(tm tm_t, std::string cfg_permalinks);

bool time_smaller(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
bool time_equal(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
bool time_greater(SingleItem::ConstPtr left, SingleItem::ConstPtr right);
