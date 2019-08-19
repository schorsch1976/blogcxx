#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "TemplateWrapper.h"

void CreateArchive(const fs::path rel_path, const ConstArchive &ar,
				   const ConfigCollection &cfgs, const TemplateWrapper &engine,
				   std::string feed_url, TemplateData data);
