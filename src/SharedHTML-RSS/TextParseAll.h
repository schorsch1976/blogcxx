/*
* blogcpp :: https://www.blogcpp.org
*/

#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/SingleItem.h"
#include <string>
#include <vector>

std::pair<std::string, std::vector<std::string>>
TextParseAll(const SingleItem &si, const ConfigCollection &cfgs,
			 bool exceprt = false);
