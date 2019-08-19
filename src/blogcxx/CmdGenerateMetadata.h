#pragma once

#include "Shared/ConfigCollection.h"
#include "Shared/Metadata.h"

// used to give main the information that there are no files found
struct NoFilesFound
{
};

Metadata CmdGenerateMetadata(const ConfigCollection &cfgs);
