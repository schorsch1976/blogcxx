/*
 * blogcxx :: https://www.blogcxx.de
 * RSS class [header].
 */
#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "Shared/SingleItem.h"
#include "Shared/filesystem.h"

#include "XML/tinyxml2.h"

class RSSGenerator
{
public:
	RSSGenerator(fs::path outfile, std::string title,
				 const ConfigCollection &cfgs);
	void createRSS(const ConstArchive &vec);

private:
	std::string title;
	fs::path outfile;
	const ConfigCollection &own_cfgs;
};
