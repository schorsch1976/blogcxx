/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "ConfigCollection.h"

#include "Debug.h"
#include "Helpers.h"
#include "SingleItem.h"
#include "constants.h"

#include <iomanip>
#include <regex>
#include <sstream>
#include <thread>

ConfigCollection::ConfigCollection(const ConfigCollectionFile &file)
	: m_file(file)
{
	// check the verbosity
	if (m_file.verbosity > static_cast<int>(Debug::Level::fatal) ||
		m_file.verbosity < static_cast<int>(Debug::Level::trace))
	{
		THROW_FATAL("Verbosity set to an unsupported value %1%. [Quiet(%2%) - "
					"Verbose(%3%)]",
					m_file.verbosity, static_cast<int>(Debug::Level::trace),
					static_cast<int>(Debug::Level::fatal));
	}

	if (m_file.cfg_siteurl.empty())
	{
		m_url = "file:///" + fs::current_path().string();
		m_url += "/" + m_file.cfg_outdir + "/";
	}
	else
	{
		m_url = m_file.cfg_siteurl;
	}
	// TODO check the pathes if they are unique
}

int ConfigCollection::num_threads() const
{
	return m_file.num_threads <= 0 ? std::thread::hardware_concurrency()
								   : m_file.num_threads;
}
std::string ConfigCollection::executable() const { return m_file.executable; }

std::string ConfigCollection::sitetitle() const { return m_file.cfg_sitetitle; }
const boost::optional<std::string> ConfigCollection::subtitle() const
{
	return m_file.cfg_subtitle;
}

Debug::Level ConfigCollection::verbosity() const
{

	return static_cast<Debug::Level>(m_file.verbosity);
}

std::string ConfigCollection::author() const { return m_file.cfg_author; }

std::string ConfigCollection::permalink_format() const
{
	return m_file.cfg_permalinks;
}
std::string ConfigCollection::locale() const { return m_file.cfg_locale; }
bool ConfigCollection::series() const { return m_file.cfg_series; }
int ConfigCollection::maxitems() const { return m_file.cfg_maxitems; }
int ConfigCollection::maxhistory() const { return m_file.cfg_maxhistory; }

bool ConfigCollection::embeds() const { return m_file.cfg_embeds; }
int ConfigCollection::excerpts() const { return m_file.cfg_excerpts; }
std::string ConfigCollection::excerpttext() const
{
	return m_file.cfg_excerpttext;
}

bool ConfigCollection::emojis() const { return m_file.cfg_emojis; }
bool ConfigCollection::fullfeed() const { return m_file.cfg_fullfeed; }

const boost::optional<std::string> ConfigCollection::commenttype() const
{
	return m_file.cfg_commenttype;
}
const boost::optional<std::string> ConfigCollection::commentdata() const
{
	return m_file.cfg_commentdata;
}

std::string ConfigCollection::opengraphimg() const
{
	return m_file.cfg_opengraphimg;
}

const boost::optional<std::string> ConfigCollection::link_url(int idx) const
{
	if (idx < 0 || idx >= MAX_CONFIG_SIDEBAR_LINKS)
	{
		THROW_FATAL(
			"Requested unsuported link_url index (%1%) allowed: [%2% to %3%]",
			idx, 0, MAX_CONFIG_SIDEBAR_LINKS - 1);
	}
	return m_file.link_url[idx];
}
const boost::optional<std::string> ConfigCollection::link_text(int idx) const
{
	if (idx < 0 || idx >= MAX_CONFIG_SIDEBAR_LINKS)
	{
		THROW_FATAL(
			"Requested unsuported link_text index (%1%) allowed: [%2% to %3%]",
			idx, 0, MAX_CONFIG_SIDEBAR_LINKS - 1);
	}
	return m_file.link_text[idx];
}
const boost::optional<std::string> ConfigCollection::link_title(int idx) const
{
	if (idx < 0 || idx >= MAX_CONFIG_SIDEBAR_LINKS)
	{
		THROW_FATAL(
			"Requested unsuported link_title index (%1%) allowed: [%2% to %3%]",
			idx, 0, MAX_CONFIG_SIDEBAR_LINKS - 1);
	}
	return m_file.link_title[idx];
}

// convert the configuration to paths
fs::path ConfigCollection::tpldir() const
{
	fs::path ret = {"templates"};
	ret /= m_file.cfg_template;
	return ret;
}

fs::path ConfigCollection::tpl_index() const { return tpldir() / "index.txt"; }
fs::path ConfigCollection::tpl_archiv() const
{
	return tpldir() / "archives.txt";
}
fs::path ConfigCollection::tpl_post() const { return tpldir() / "post.txt"; }
fs::path ConfigCollection::tpl_page() const { return tpldir() / "page.txt"; }
fs::path ConfigCollection::tpl_RSS() const
{
	return fs::path("templates") / "RSS.txt";
}

fs::path ConfigCollection::indir() const
{
	fs::path ret = {m_file.cfg_indir};
	return ret;
}
fs::path ConfigCollection::indir_posts() const
{
	return indir() / m_file.cfg_article_subdir;
}
fs::path ConfigCollection::indir_pages() const
{
	return indir() / m_file.cfg_page_subdir;
}

fs::path ConfigCollection::outdir_root() const
{
	fs::path ret = {m_file.cfg_outdir};
	ret += "/";
	return ret;
}

fs::path ConfigCollection::commentdir() const
{
	fs::path ret = {"comments"};
	return ret;
}

fs::path ConfigCollection::rel_path_archive() const
{
	return fs::path("archives");
}

fs::path ConfigCollection::rel_path_archive_year(const tm &time) const
{
	std::ostringstream oss_year;
	oss_year << std::setw(4) << std::setfill('0') << time.tm_year + 1900;

	fs::path ret{rel_path_archive()};
	ret /= oss_year.str();
	return ret;
}
fs::path ConfigCollection::rel_path_archive_year_month(const tm &time) const
{
	std::ostringstream oss_year, oss_month;
	oss_year << std::setw(4) << std::setfill('0') << time.tm_year + 1900;
	oss_month << std::setw(2) << std::setfill('0') << time.tm_mon + 1;

	fs::path ret{rel_path_archive()};
	ret /= oss_year.str();
	ret /= oss_month.str();
	return ret;
}
fs::path ConfigCollection::rel_path_posts(const std::string &slug) const
{
	fs::path p{m_file.cfg_article_subdir};
	p /= slug;
	return p;
}
fs::path ConfigCollection::rel_path_pages(const std::string &slug) const
{
	fs::path p{m_file.cfg_page_subdir};
	p /= slug;
	return p;
}
fs::path ConfigCollection::rel_path_authors(const std::string &author) const
{
	fs::path p{"author"};
	p /= author;
	return p;
}
fs::path ConfigCollection::rel_path_categories(const std::string &cat) const
{
	fs::path p{"categories"};
	p /= cat;
	return p;
}
fs::path ConfigCollection::rel_path_tags(const std::string &cat) const
{
	fs::path p{"tags"};
	p /= cat;
	return p;
}

fs::path ConfigCollection::rel_path_static() const
{
	fs::path p{"static"};
	return p;
}

fs::path ConfigCollection::rel_path_series(const std::string &series) const
{
	fs::path p{"series"};
	p /= series;
	return p;
}

fs::path ConfigCollection::rel_path_images() const
{
	fs::path ret{"images"};
	return ret;
}

fs::path ConfigCollection::rel_path_feed() const
{
	fs::path ret{m_file.cfg_feeddir};
	return ret;
}

// index: -1 -> no index.html added
// indes:  0 -> relpath/index.html
// indes:  n -> relpath/index-n.html
// url("", int index = -1) -> returns the base url
std::string ConfigCollection::url(const fs::path rel_path, int index) const
{
	fs::path total{"/"};
	total /= rel_path;

	if (index <= -1)
	{
		// nothing todo
	}
	else if (index == 0)
	{
		total /= "index.html";
	}
	else
	{
		total /= "index-" + std::to_string(index) + ".html";
	}
	std::string ret = m_url;
	ret += total.string();

	// substitute \ to /
	for (auto &c : ret)
	{
		if (c == '\\')
		{
			c = '/';
		}
	}

	if (*ret.rbegin() == '/')
	{
		ret.pop_back();
	}

	// Makes "a/b/c" out of "a//b//////c".
	std::regex slash("([^:])/+"); // Do not replace "https://".
	std::string out = std::regex_replace(ret, slash, "$1/");
	return out;
}

fs::path ConfigCollection::feed_file(const ArchiveData &ad) const
{
	fs::path outfile;

	switch (ad.type)
	{
		case ArchiveType::Author:
			outfile = "author-";
			outfile += lowercase(ad.path.stem().string()) + ".xml";
			break;
		case ArchiveType::Categories:
			outfile = "category-";
			outfile += lowercase(ad.path.stem().string()) + ".xml";
			break;
		case ArchiveType::Tags:
			outfile = "tag-";
			outfile += lowercase(ad.path.stem().string()) + ".xml";
			break;
		case ArchiveType::Series:
			outfile = "series-";
			outfile += lowercase(ad.path.stem().string()) + ".xml";
			break;
		default:
			outfile = "RSS.xml";
	}

	return outfile;
}
