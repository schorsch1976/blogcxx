/*
 * blogcpp :: https://www.blogcpp.org
 * Some everyday constant definitions.
 */

#ifndef HAS_CONSTANTS
#define HAS_CONSTANTS // Once is enough.

#include <string>

using std::string;

#ifdef WITH_DEBUGLOG
const string DEBUGLOGFILE = "debug.txt";
#endif
const int APP_VERSION = 9;
const string APP_URL = "https://www.blogcpp.org";
const string BUGTRACKER = "https://bitbucket.org/tux_/blogcpp/issues";
const string HEADER_DIVIDER = "----------";
const string NEWLINE = "\n"; // Also on Windows. Modern Notepad will eat it.
const string HIGHLIGHT_VER = "9.13.1";
#endif
