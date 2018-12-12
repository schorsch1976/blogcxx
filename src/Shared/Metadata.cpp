/*
 * blogcxx :: https://www.blogcxx.de
 */

#include "Metadata.h"

namespace
{
ConstArchive Convert(const Archive &rhs)
{
	ConstArchive tmp;
	for (auto p : rhs)
	{
		tmp.push_back(p);
	}
	return tmp;
}

template <typename T>
std::map<T, const ConstArchive> ConvertMap(const std::map<T, Archive> &rhs)
{
	std::map<T, const ConstArchive> ret;

	for (auto &c : rhs)
	{
		ret.emplace(c.first, std::move(Convert(c.second)));
	}
	return ret;
}

} // namespace

ConstMetadata::ConstMetadata(const Metadata &rhs)
	: all_pages(Convert(rhs.all_pages)), all_posts(Convert(rhs.all_posts)),

	  authors(ConvertMap(rhs.authors)), categories(ConvertMap(rhs.categories)),
	  tags(ConvertMap(rhs.tags)), series(ConvertMap(rhs.series)),

	  archives(ConvertMap(rhs.archives))
{
}
