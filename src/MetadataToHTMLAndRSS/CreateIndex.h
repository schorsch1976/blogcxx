/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "TemplateWrapper.h"

void CreateIndex(size_t page, size_t page_count, const ConstArchive &posts,
				 const ConstArchive &pages, const ConfigCollection &cfgs,
				 const TemplateWrapper &engine, TemplateData data);
