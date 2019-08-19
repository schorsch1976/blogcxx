/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CmdNewPage.h"

#include <string>

#include "ContentsGenerator.h"
#include "Log/Log.h"

#include "Shared/Slug.h"

void CmdNewPage(const ConfigCollection &cfgs)
{
	std::string title;
	PRINT("Looks like you're planning to start a new page.");
	while (title.empty())
	{
		PRINT("Please type the prospected title to start, press Ctrl+C to "
			  "abort: ");
		std::getline(std::cin, title);
	}
	PRINT("The magic begins.");

	title = trim(title);
	ContentsGenerator(ItemType::Page, cfgs.indir_pages(), cfgs.author(), title,
					  hyphenise(title))
		.generate();

	PRINT("Your page has been created in '%1%'", cfgs.indir_pages().string());
	PRINT("Run %1% again to generate your updated website.", cfgs.executable());
}
