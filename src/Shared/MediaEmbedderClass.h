/*
 * blogcxx :: https://www.blogcxx.de
 * Class to add embedded media to the output stream [header].
 */

#pragma once

#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

#ifdef WITH_OEMBED
#include "XML/tinyxml2.h"
#include "curl/HTTPDownloader.hpp"
#include "json.hpp"
#endif

class MediaEmbedder
{
public:
	bool addEmbeds(std::string &inputline);

	MediaEmbedder();

#ifdef WITH_OEMBED
private:
	bool addOEmbeds(std::string &inputline);
#endif
};
