/*
* blogcpp :: https://www.blogcpp.org
*/

#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "TemplateWrapper.h"

void CreateIndex(const ConstArchive &posts, const ConstArchive &pages,
				 const ConfigCollection &cfgs, const TemplateWrapper &engine,
				 TemplateData data);
