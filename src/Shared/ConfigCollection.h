/*
 * blogcxx :: https://www.blogcxx.de
 * Collection of configuration entries to pass around.
 */

#pragma once

#include <array>
#include <boost/optional.hpp>
#include <string>

#include "Debug.h"
#include "Metadata.h"
#include "filesystem.h"

#if 0
static constexpr size_t MAX_CONFIG_SIDEBAR_LINKS = 100;
#else
#define MAX_CONFIG_SIDEBAR_LINKS 100
#endif

struct ConfigCollectionFile
{
	std::string executable;
	int num_threads;
	int verbosity;

	std::string cfg_sitetitle;
	boost::optional<std::string> cfg_subtitle;
	std::string cfg_siteurl;
	std::string cfg_author;
	std::string cfg_indir;
	std::string cfg_outdir;
	std::string cfg_permalinks;
	std::string cfg_article_subdir;
	std::string cfg_page_subdir;
	std::string cfg_locale;
	bool cfg_series;
	int cfg_maxitems;
	int cfg_maxhistory;
	std::string cfg_template;
	bool cfg_embeds;
	int cfg_excerpts;
	std::string cfg_excerpttext;
	bool cfg_emojis;
	bool cfg_fullfeed;
	std::string cfg_feeddir;
	boost::optional<std::string> cfg_commenttype;
	boost::optional<std::string> cfg_commentdata;
	std::string cfg_opengraphimg;

	std::array<boost::optional<std::string>, MAX_CONFIG_SIDEBAR_LINKS> link_url;
	std::array<boost::optional<std::string>, MAX_CONFIG_SIDEBAR_LINKS>
		link_text;
	std::array<boost::optional<std::string>, MAX_CONFIG_SIDEBAR_LINKS>
		link_title;
};

class ConfigCollection
{
public:
	explicit ConfigCollection(const ConfigCollectionFile &file);

	int num_threads() const;
	std::string executable() const;

	std::string sitetitle() const;
	const boost::optional<std::string> subtitle() const;

	Debug::Level verbosity() const;

	std::string author() const;

	std::string permalink_format() const;
	std::string locale() const;
	bool series() const;
	int maxitems() const;
	int maxhistory() const;

	bool embeds() const;
	int excerpts() const;
	std::string excerpttext() const;

	bool emojis() const;
	bool fullfeed() const;

	const boost::optional<std::string> commenttype() const;
	const boost::optional<std::string> commentdata() const;

	std::string opengraphimg() const;

	const boost::optional<std::string> link_url(int idx) const;
	const boost::optional<std::string> link_text(int idx) const;
	const boost::optional<std::string> link_title(int idx) const;

	// convert the configuration to paths
	fs::path tpldir() const;
	fs::path tpl_index() const;
	fs::path tpl_archiv() const;
	fs::path tpl_post() const;
	fs::path tpl_page() const;
	fs::path tpl_RSS() const;

	fs::path indir() const;
	fs::path indir_posts() const;
	fs::path indir_pages() const;

	fs::path outdir_root() const;

	fs::path commentdir() const;

	fs::path rel_path_archive() const;
	fs::path rel_path_archive_year(const tm &time) const;
	fs::path rel_path_archive_year_month(const tm &time) const;
	fs::path rel_path_posts(const std::string &slug) const;
	fs::path rel_path_pages(const std::string &slug) const;
	fs::path rel_path_authors(const std::string &author) const;
	fs::path rel_path_categories(const std::string &cat) const;
	fs::path rel_path_tags(const std::string &cat) const;
	fs::path rel_path_static() const;
	fs::path rel_path_series(const std::string &series) const;
	fs::path rel_path_images() const;
	fs::path rel_path_feed() const;

	fs::path feed_file(const ArchiveData &ad) const;

	// index: -1 -> no index.html added
	// indes:  0 -> relpath/index.html
	// indes:  n -> relpath/index-n.html
	// url("", int index = -1) -> returns the base url
	std::string url(const fs::path rel_path, int index = -1) const;

private:
	const ConfigCollectionFile &m_file;
	std::string m_url;
};
