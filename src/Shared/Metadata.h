/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include "SingleItem.h"

#include "filesystem.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

struct AllFilePaths
{
	std::vector<fs::path> posts;
	std::vector<fs::path> pages;
};

enum class ArchiveType
{
	Year = 0,
	YearMonth,
	Author,
	Categories,
	Tags,
	Series
};

struct ArchiveData
{
	fs::path path;
	ArchiveType type;
	tm time;

	inline bool operator<(const ArchiveData &rhs) const
	{
		return path < rhs.path;
	}
	inline bool operator>(const ArchiveData &rhs) const
	{
		return path > rhs.path;
	}
	inline bool operator==(const ArchiveData &rhs) const
	{
		return path < rhs.path;
	}
};

struct Metadata
{
	// all pointers point into this container
	Archive all_pages;
	Archive all_posts;

	// these are the cross references
	std::map<std::string, Archive> authors;

	std::map<std::string, Archive> categories;
	std::map<std::string, Archive> tags;
	std::map<std::string, Archive> series;

	std::map<ArchiveData, Archive> archives;
};

// the reason for this big constiness is
// auto pos = metadata.authors.find("Georg");
// if (pos != metadata.authors.end())
// {
// 	pos->second[0]->b_sticky = false; // this must fail
// }

struct ConstMetadata
{
	explicit ConstMetadata(const Metadata &rhs);

	ConstMetadata(const ConstMetadata &) = delete;
	ConstMetadata(ConstMetadata &&) = delete;
	ConstMetadata &operator=(const ConstMetadata &) = delete;
	ConstMetadata &operator=(ConstMetadata &&) = delete;

	// all pointers point into this container
	const ConstArchive all_pages;
	const ConstArchive all_posts;

	// these are the cross references
	const std::map<std::string, const ConstArchive> authors;

	const std::map<std::string, const ConstArchive> categories;
	const std::map<std::string, const ConstArchive> tags;
	const std::map<std::string, const ConstArchive> series;

	const std::map<ArchiveData, const ConstArchive> archives;
};