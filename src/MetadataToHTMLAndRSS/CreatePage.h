#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"
#include "TemplateWrapper.h"

void CreatePage(SingleItem::ConstPtr si, int page_index,
				const ConfigCollection &cfgs, const TemplateWrapper &engine,
				TemplateData data);
