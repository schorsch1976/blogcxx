/*
 * blogcpp :: https://www.blogcpp.org
 * Some everyday constant definitions.
 */

#pragma once

#include <string>

#ifdef WITH_DEBUGLOG
const std::string DEBUGLOGFILE = "debug.txt";
#endif
const std::string APP_VERSION = "10-alpha";
const std::string APP_URL = "https://www.blogcpp.org";
const std::string BUGTRACKER = "https://bitbucket.org/tux_/blogcpp/issues";
const std::string HEADER_DIVIDER = "----------";
const std::string NEWLINE =
	"\n"; // Also on Windows. Modern Notepad will eat it.
const std::string HIGHLIGHT_VER = "9.13.1";
