/*
 * blogcxx :: https://www.blogcxx.de
 * Class to make using the inja template engine easier.
 */

#include "TemplateWrapper.h"
#include "Log/Log.h"

#include "html-template++/html.h"

#include <set>

// ----------------------------------------------------------------------------
// the json wrapper
// ----------------------------------------------------------------------------
struct TemplateData::impl
{
	html::item data;

	impl() : data("root") {}

	void Set(html::item &d, std::vector<TemplateKey> path,
			 const ValueKey &value)
	{
#if USE_VARIANT_API == 1
		// std
		// break the recursion
		if (path.empty())
		{
			size_t index = value.index();
			switch (index)
			{
				case 0:
					d.get_string() = std::get<0>(value);
					break;
				case 1:
					d.get_bool() = std::get<1>(value).value;
					break;
				default:
					THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
								"This is a Bug. Please report it at %1%.",
								BUGTRACKER);
					break;
			}
			return;
		}

		std::vector<TemplateKey> reduced{path};
		reduced.erase(reduced.begin());

		const auto &next_key = path.front();
		size_t index = next_key.index();
		switch (index)
		{
			case 0:
				Set(d.child(std::get<0>(next_key)), reduced, value);
				break;
			case 1:
			{
				// this must be an array
				auto &arr = d.get_array();
				const int p = std::get<1>(next_key);
				if (static_cast<int>(arr.size()) < p + 1)
				{
					for (int i = static_cast<int>(arr.size()); i < p + 1; ++i)
					{
						arr.emplace_back(html::item(std::to_string(i)));
					}
				}
				Set(arr[p], reduced, value);
				break;
			}
			default:
				THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
							"This is a Bug. Please report it at %1%.",
							BUGTRACKER);
				break;
		}

#endif

#if USE_VARIANT_API == 2
		// boost
		// break the recursion
		if (path.empty())
		{
			if (const std::string *p = boost::get<std::string>(&value))
			{
				d.get_string() = *p;
			}
			else if (const boolean *p = boost::get<boolean>(&value))
			{
				d.get_bool() = p->value;
			}
			else
			{
				THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
							"This is a Bug. Please report it at %1%.",
							BUGTRACKER);
			}
			return;
		}

		std::vector<TemplateKey> reduced{path};
		reduced.erase(reduced.begin());

		const auto &next_key = path.front();
		if (const std::string *p = boost::get<std::string>(&next_key))
		{
			Set(d.child(*p), reduced, value);
		}
		else if (const int *p = boost::get<int>(&next_key))
		{
			// this must be an array
			auto &arr = d.get_array();
			if (static_cast<int>(arr.size()) < *p + 1)
			{
				for (int i = static_cast<int>(arr.size()); i < *p + 1; ++i)
				{
					arr.emplace_back(html::item(std::to_string(i)));
				}
			}
			Set(arr[*p], reduced, value);
		}
		else
		{
			THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
						"This is a Bug. Please report it at %1%.",
						BUGTRACKER);
		}
#endif
	}
};

TemplateData::TemplateData() { mp_impl = std::make_unique<impl>(); }
TemplateData::~TemplateData() {}

// just move, no copy
TemplateData::TemplateData(TemplateData &&rhs)
{
	mp_impl = std::move(rhs.mp_impl);
}

TemplateData &TemplateData::operator=(TemplateData &&rhs)
{
	mp_impl = std::move(rhs.mp_impl);
	return *this;
}

TemplateData::TemplateData(const TemplateData &rhs)
{
	mp_impl = std::make_unique<impl>(*rhs.mp_impl);
}
TemplateData &TemplateData::operator=(const TemplateData &rhs)
{
	mp_impl = std::make_unique<impl>(*rhs.mp_impl);
	return *this;
}

void TemplateData::Set(std::vector<TemplateKey> path, const ValueKey &value)
{
	if (path.empty())
	{
		LOG_ERROR("TemplateData::Set: path is empty");
		return;
	}

	mp_impl->Set(mp_impl->data, path, value);
}
std::string TemplateData::to_string() const
{
	std::string s = mp_impl->data.dump();
	return s;
}

// ----------------------------------------------------------------------------
// the inja wrapper
// ----------------------------------------------------------------------------
struct TemplateWrapper::impl
{
	// sadly, as the environment is not const for render, we
	// need to use an own environment for all CreateXYZ functions
	std::set<fs::path> m_tpl;
};

TemplateWrapper::TemplateWrapper(std::vector<fs::path> templ_directories,
								 fs::path tmpl_ext)
{
	mp_impl = std::make_unique<impl>();

	// read all templates into the environment
	for (auto &templ_directory : templ_directories)
	{
		for (fs::directory_iterator end_dir_it, it(templ_directory);
			 it != end_dir_it; ++it)
		{
			const fs::path &path = it->path();
			if (!fs::is_regular_file(path))
			{
				continue;
			}

			auto e = path.extension();
			if (path.extension() != tmpl_ext)
			{
				continue;
			}

			try
			{
				mp_impl->m_tpl.insert(path);
			}
			catch (const std::exception &ex)
			{
				THROW_FATAL("TemplateWrapper: File: '%1%' - Error: %2%",
							path.string(), ex.what());
			}
		}
	}
}

TemplateWrapper::~TemplateWrapper() {}
std::string TemplateWrapper::Render(const fs::path &tpl_file,
									const TemplateData &data) const
{
	auto pos = mp_impl->m_tpl.find(tpl_file);
	if (pos == mp_impl->m_tpl.end())
	{
		THROW_FATAL("TemplateWrapper: template '%1% 'not found in environment.",
					tpl_file.string());
	}

	std::string rendered;
	try
	{
		// making a copy preserves the outer data structure in any case
		std::string s = data.to_string();
		rendered = html::parse(tpl_file, data.mp_impl->data);
	}
	catch (std::exception &e)
	{
		THROW_FATAL("error trying to render '%1%' - %2%.\n   json=%3%",
					tpl_file.string(), e.what(), data.to_string());
	}
	return rendered;
}
