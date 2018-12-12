/*
 * blogcxx :: https://www.blogcxx.de
 * RSS class.
 */

#include "RSSGeneratorClass.h"
#include "Shared/Debug.h"
#include "Shared/Helpers.h"
#include "SharedHTML-RSS/ExcerptExtractorClass.h"
#include "SharedHTML-RSS/TextParseAll.h"

#include <iomanip>

RSSGenerator::RSSGenerator(fs::path outfile, std::string title,
						   const ConfigCollection &cfgs)
	: own_cfgs(cfgs)
{
	this->title = title;
	this->outfile = outfile;
}

void RSSGenerator::createRSS(const ConstArchive &vec)
{
	// Creates the site's RSS.xml file.
	// Requires vec to be reverse-sorted (which is probably the case here).
	PRINT("Creating the RSS feed '%1%.", outfile.string());

	tinyxml2::XMLDocument rss;
	std::stringstream ss_item_time, ss_text, ss_url;
	int counter = 0;

	tinyxml2::XMLDeclaration *pDeclaration = rss.NewDeclaration();
	pDeclaration->SetValue("xml version=\"1.0\" encoding=\"utf-8\"");
	rss.InsertFirstChild(pDeclaration);

	tinyxml2::XMLElement *pRoot = rss.NewElement("rss");
	pRoot->SetAttribute("version", "2.0");
	rss.InsertEndChild(pRoot);

	tinyxml2::XMLNode *pChannel = rss.NewElement("channel");
	pRoot->InsertFirstChild(pChannel);

	tinyxml2::XMLElement *pTitle = rss.NewElement("title");
	pTitle->SetText(title.c_str());
	pChannel->InsertFirstChild(pTitle);

	tinyxml2::XMLElement *pLink = rss.NewElement("link");
	pLink->SetText(own_cfgs.url("").c_str());
	pChannel->InsertEndChild(pLink);

	if (own_cfgs.subtitle())
	{
		tinyxml2::XMLElement *pDescription = rss.NewElement("description");
		pDescription->SetText(own_cfgs.subtitle()->c_str());
		pChannel->InsertEndChild(pDescription);
	}

	tinyxml2::XMLElement *pUpdated = rss.NewElement("lastBuildDate");
	pUpdated->SetText(timeNow().c_str());
	pChannel->InsertEndChild(pUpdated);

	// Traverse through the latest <maxitems> posts and add them to the feed..
	ConstArchive v_newest_x{
		vec.begin(),
		next(vec.begin(),
			 std::min(vec.size(), static_cast<size_t>(own_cfgs.maxitems())))};
	for (auto s : v_newest_x)
	{
		const SingleItem &p = *s;

		if (p.type == ItemType::Page)
		{
			// It makes no sense to have pages in your RSS feed.
			continue;
		}

		tinyxml2::XMLNode *pItem = rss.NewElement("item");
		pChannel->InsertEndChild(pItem);

		tinyxml2::XMLElement *pItemTitle = rss.NewElement("title");
		pItemTitle->SetText(p.s_title.c_str());
		pItem->InsertFirstChild(pItemTitle);

		tinyxml2::XMLElement *pItemDescription = rss.NewElement("description");

		// We probably have processed the Markdown text by this point unless the
		// user has chosen "full feed" but "shortened articles" (or vice versa).
		// In the best case, we don't want to repeat ourselves.
		ss_text.str("");

		ss_text << TextParseAll(p, own_cfgs, !own_cfgs.fullfeed(),
								false /* no markdown */)
					   .first;

		tinyxml2::XMLText *pItemDescText = rss.NewText(ss_text.str().c_str());
		pItemDescText->SetCData(true);

		pItemDescription->InsertEndChild(pItemDescText);
		pItem->InsertEndChild(pItemDescription);

		tinyxml2::XMLElement *pItemLink = rss.NewElement("link");
		std::string outputdate =
			parseTmToPath(p.time, own_cfgs.permalink_format());
		ss_url.str("");
		ss_url << own_cfgs.url("") << "/";
		if (own_cfgs.indir_posts().string() != "/")
		{
			ss_url << own_cfgs.indir_posts().string() << "/";
		}
		ss_url << outputdate << "/" << p.s_slug;
		pItemLink->SetText(ss_url.str().c_str());
		pItem->InsertEndChild(pItemLink);

		tinyxml2::XMLElement *pItemPubDate = rss.NewElement("pubDate");
		ss_item_time.str("");
		ss_item_time << std::put_time(&p.time, "%a, %d %b %Y %T %z");

		pItemPubDate->SetText(ss_item_time.str().c_str());
		pItem->InsertEndChild(pItemPubDate);

		counter++;
	}

	if (counter > 0)
	{
		// Don't write empty feeds.
		LOG_DEBUG("Writing the RSS file into %1%", outfile.string());
		rss.SaveFile(outfile.string().c_str());
	}
	else
	{
		LOG_INFO("There are no items for this feed. Skipping.");
	}
}
