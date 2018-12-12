/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "CmdNewArticle.h"

#include <string>

#include "ContentsGenerator.h"
#include "Shared/Debug.h"
#include "Shared/Helpers.h"
#include "Shared/Slug.h"

void CmdNewArticle(const ConfigCollection &cfgs)
{
	std::string title;
	PRINT("Looks like you're planning to start a new blog article.");

	while (title.empty())
	{
		PRINT("Please type the prospected title to start, press Ctrl+C "
			  "to abort: ");
		std::getline(std::cin, title);
	}
	PRINT("The magic begins.");

	title = trim(title);
	ContentsGenerator(ItemType::Post, cfgs.indir_posts(), cfgs.author(), title,
					  createBasicSlug(title))
		.generate();

	PRINT("Your blog post has been created in '%1%'",
		  cfgs.indir_posts().string());
	PRINT("Run %1% again to generate your updated website.", cfgs.executable());
}
