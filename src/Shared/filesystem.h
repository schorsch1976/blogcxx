/*
 * blogcpp :: https://www.blogcpp.org
 * Filesystem class detector.
 */

#pragma once

#if !defined(USE_FILESYSTEM_API) || USE_FILESYSTEM_API <1 || USE_FILESYSTEM_API >3
	#error "Filesystem API not defined"
#endif

#if USE_FILESYSTEM_API==1
#include <filesystem>
namespace fs = std::filesystem;
#endif


#if USE_FILESYSTEM_API==2
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#if USE_FILESYSTEM_API==3
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif
