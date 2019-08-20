/*
 * blogcxx :: https://www.blogcxx.de
 * Generator class for new posts and pages [header].
 */

#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "Shared/Helpers.h"
#include "Shared/SingleItem.h"
#include "Shared/filesystem.h"

class ContentsGenerator
{
public:
	ContentsGenerator(ItemType type, fs::path input_folder, std::string author,
					  std::string title, std::string slug,
					  std::string first_text = "");

	void generate();

private:
	ItemType type;

	std::string createDummyPage();
	std::string createDummyArticle();

	fs::path input_folder;
	std::string author;
	std::string title;
	std::string slug;
	std::string first_text;
};
