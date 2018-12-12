/*
* blogcpp :: https://www.blogcpp.org
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
	: all_items(Convert(rhs.all_items)),

	  authors(ConvertMap(rhs.authors)), categories(ConvertMap(rhs.categories)),
	  tags(ConvertMap(rhs.tags)), series(ConvertMap(rhs.series)),

	  archives(ConvertMap(rhs.archives))
{
}