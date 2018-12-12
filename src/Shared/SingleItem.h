/*
 * blogcxx :: https://www.blogcxx.de
 * Definition of single blog entries.
 */

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "filesystem.h"

enum class ItemType
{
	Page = 0,
	Post
};

inline std::ostream &operator<<(std::ostream &os, const ItemType &type)
{
	switch (type)
	{
		default:
		case ItemType::Page:
			os << "Page";
			break;
		case ItemType::Post:
			os << "Post";
			break;
	}
	return os;
}

class SingleItem
{
	// The SingleItem class holds the meta data of a post or page
	// so it can be used to generate archive pages and/or lists.
public:
	using Ptr = std::shared_ptr<SingleItem>;
	using ConstPtr = std::shared_ptr<const SingleItem>;

	// -------------------------
	// Meta data (from input)
	// -------------------------
	tm time;			  // The creation date/time of the item.
	tm changetime;		  // The date/time of the latest change of the item.
	std::string s_title;  // The contents of the title of the item.
	std::string s_slug;   // The (title/URL) slug of the item.
	std::string s_author; // The author of the item.
	std::string s_text;   // The actual contents of the item.
	bool b_markdown;	  // Could be "Off" if markdownify() is not wanted.
	bool b_emoji;		  // Could be "Off" if this item must not have emojis.
	bool b_sticky;		  // Could be "On" if the article should be sticky.
	bool b_comments;	  // Could be "Off" if this item must not have comments.
	bool b_hardbreaks;	  // Could be "On" if this item should be trated with hardbreaks
	std::string s_ogimage; // An optional OpenGraph image URL.

	std::string
		s_series; // An optional unique name for the series (if !b_isPage).

	int i_position; // An optional position (1..infinity) if b_isPage.
					// Default: 0.

	std::vector<std::string> cats; // Contains the categories of the item.
	std::vector<std::string> tags; // Contains the tags of the item.

	// -------------------------
	// Meta data (internal)
	// -------------------------
	ItemType type;

	fs::path s_filename; // Store the input file name.

	explicit SingleItem(ItemType typ)
	{
		time = {};
		changetime = {};
		s_title = "";
		s_slug = "";
		s_author = "";
		s_text = "";
		b_markdown = true;
		b_emoji = true;
		b_sticky = true;
		b_comments = true;
		b_hardbreaks = false;
		s_ogimage = "";
		s_series = "";
		i_position = 0;

		cats = tags = {};

		type = typ;

		s_filename = "";
	}
};

using Archive = std::vector<std::shared_ptr<SingleItem>>;
using ConstArchive = std::vector<std::shared_ptr<const SingleItem>>;
