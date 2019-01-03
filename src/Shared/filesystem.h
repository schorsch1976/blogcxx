/*
 * blogcxx :: https://www.blogcxx.de
 * Filesystem class detector.
 */

#pragma once

#if !defined(USE_FILESYSTEM_API) || USE_FILESYSTEM_API < 1 ||                  \
	USE_FILESYSTEM_API > 2
#error "Filesystem API not defined"
#endif

#if USE_FILESYSTEM_API == 1
#include <filesystem>
namespace fs = std::filesystem;
#endif

#if USE_FILESYSTEM_API == 2
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif
