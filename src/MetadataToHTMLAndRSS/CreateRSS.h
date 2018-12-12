#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "TemplateWrapper.h"

void CreateRSS(const fs::path outfile, const std::string title,
			   const ConstArchive ar, const ConfigCollection &cfgs,
			   const TemplateWrapper &engine);
