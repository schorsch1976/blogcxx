/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "ConfigCollection.h"

#include "Helpers.h"
#include "SingleItem.h"

#include "Shared/regex.h"
#include <iomanip>
#include <sstream>
#include <thread>

ConfigCollection::ConfigCollection(const ConfigCollectionFile &file)
	: m_file(file)
{
	// check the verbosity
	if (m_file.file_verbosity > static_cast<int>(Log::Level::fatal) ||
		m_file.file_verbosity < static_cast<int>(Log::Level::trace))
	{
		THROW_FATAL(
			"File verbosity set to an unsupported value %1%. [Quiet(%2%) - "
			"Verbose(%3%)]",
			m_file.file_verbosity, static_cast<int>(Log::Level::trace),
			static_cast<int>(Log::Level::fatal));
	}

	if (m_file.console_verbosity > static_cast<int>(Log::Level::fatal) ||
		m_file.console_verbosity < static_cast<int>(Log::Level::trace))
	{
		THROW_FATAL(
			"Console verbosity set to an unsupported value %1%. [Quiet(%2%) - "
			"Verbose(%3%)]",
			m_file.console_verbosity, static_cast<int>(Log::Level::trace),
			static_cast<int>(Log::Level::fatal));
	}

	if (m_file.cfg_siteurl.empty())
	{
		m_url = "file://" + fs::current_path().string();
		m_url += "/" + m_file.cfg_outdir;
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

Log::Level ConfigCollection::file_verbosity() const
{
	return static_cast<Log::Level>(m_file.file_verbosity);
}
Log::Level ConfigCollection::console_verbosity() const
{
	return static_cast<Log::Level>(m_file.console_verbosity);
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

fs::path ConfigCollection::rel_path_archive_year(const pt::ptime &time) const
{
	std::ostringstream oss_year;
	oss_year << std::setw(4) << std::setfill('0') << time.date().year();

	fs::path ret{rel_path_archive()};
	ret /= oss_year.str();
	return ret;
}
fs::path
ConfigCollection::rel_path_archive_year_month(const pt::ptime &time) const
{
	std::ostringstream oss_year, oss_month;
	oss_year << std::setw(4) << std::setfill('0') << time.date().year();
	oss_month << std::setw(2) << std::setfill('0') << time.date().month();

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

fs::path ConfigCollection::rel_path_media() const
{
	fs::path ret{"media"};
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
	// rx::regex slash("([^:])/+"); // Do not replace "https://".
	// std::string out = rx::regex_replace(ret, slash, "$1/");
	return url_encode(ret);
}

std::string ConfigCollection::url_encode(std::string in) const
{
	// find the protocol
	const std::string protocol_delimeter = "://";
	auto pos = std::search(std::begin(in), std::end(in),
						   std::begin(protocol_delimeter),
						   std::end(protocol_delimeter));
	std::string protocol =
		std::string(std::begin(in), pos + protocol_delimeter.size());

	std::string value(pos + protocol_delimeter.size(), std::end(in));

	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	const std::string keep = "-._~/";

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n;
		 ++i)
	{
		std::string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || keep.find(c) != std::string::npos)
		{
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << std::uppercase;
		escaped << '%' << std::setw(2) << int((unsigned char)c);
		escaped << std::nouppercase;
	}

	return protocol + escaped.str();
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
