/*
 * blogcxx :: https://www.blogcxx.de
 * Class to add embedded media to the output stream [header].
 */

#pragma once

#include "Shared/regex.h"
#include <iomanip>
#include <sstream>
#include <string>

class MediaEmbedder
{
public:
	bool addEmbeds(std::string &inputline);

	MediaEmbedder();
};
