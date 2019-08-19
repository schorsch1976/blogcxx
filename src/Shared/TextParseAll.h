/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/SingleItem.h"
#include <string>
#include <vector>

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt = false, bool markdown = true);
