/*
* blogcpp :: https://www.blogcpp.org
*/

#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"

Metadata CollectPostData(const ConfigCollection &cfgs,
						 const AllFilePaths &all_in_files);
