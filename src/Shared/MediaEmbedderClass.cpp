/*
 * blogcxx :: https://www.blogcxx.de
 * Class to add embedded media to the output stream.
 */

#include "MediaEmbedderClass.h"
#include "Log/Log.h"

MediaEmbedder::MediaEmbedder()
{
	LOG_DEBUG("Setting up a media embedder ...");
	LOG_DEBUG("Using OEmbed.");
}

bool MediaEmbedder::addEmbeds(std::string &inputline)
{
	// If the parser finds a YouTube or Vimeo link on a separate line and the
	// user did not disable it in the configuration, it adds media links
	// automatically.
	//
	// This functionality is not actually a part of Markdown, but it might be
	// useful.

	bool ret = false;
	std::string parsedline;
	std::stringstream ss_parsedline;

	LOG_DEBUG("Using standard embeds.");

	rx::regex youtube(
		"^https?://(www\\.)?youtube\\.com/watch\\?v=([A-Za-z0-9-]+)&?$");
	rx::regex vimeo("^https?://(www\\.)?vimeo\\.com/(\\d+)$");

	rx::smatch match;

	parsedline = inputline;

	if (rx::regex_match(inputline, match, youtube))
	{

		LOG_INFO("Found a YouTube link: %1%", match.str(0));

		ss_parsedline
			<< "<iframe frameborder=\"0\" allowfullscreen width=\"640\" "
			   "height=\"360\" src=\"https://www.youtube-nocookie.com/embed/"
			<< match.str(2) << "?autoplay=0\"></iframe>";
		parsedline = ss_parsedline.str();
		ret = true;
	}
	else if (rx::regex_match(inputline, match, vimeo))
	{
		LOG_INFO("Found a Vimeo link: %1%", match.str(0));

		ss_parsedline
			<< "<iframe src=\"https://player.vimeo.com/video/" << match.str(2)
			<< "?title=0&byline=0&portrait=0&badge=0\" width=\"640\" "
			   "height=\"360\" frameborder=\"0\" allowfullscreen></iframe>";
		parsedline = ss_parsedline.str();
		ret = true;
	}

	inputline = parsedline;
	return ret;
}
