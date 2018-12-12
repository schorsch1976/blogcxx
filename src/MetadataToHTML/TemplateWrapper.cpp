/*
 * blogcxx :: https://www.blogcxx.de
 * Class to make using the inja template engine easier.
 */

#include "TemplateWrapper.h"
#include "Shared/Debug.h"
#include "Shared/constants.h"

#ifndef TEST_OLDER_COMPILERS
#include "inja.hpp"
#include "json.hpp"

using json = nlohmann::json;
#else
// dummy json
struct json
{
	void operator=(const std::string &v) {}
	void operator=(const bool &v) {}
	template <typename T>
	json operator[](T v)
	{
		return json();
	}
	bool is_array() const
	{
		return false;
	}
	static json array()
	{
		return json();
	}
	json& operator=(const json&)
	{
		return *this;
	}
	std::string dump(int) const
	{
		return "Dummy";
	}
};

#endif

#include <map>

// ----------------------------------------------------------------------------
// the json wrapper
// ----------------------------------------------------------------------------
struct TemplateData::impl
{
	json data;

	void Set(json &d, std::vector<TemplateKey> path, const ValueKey &value)
	{
#if USE_VARIANT_API == 1
		// std::variant
		// break the recursion
		if (path.empty())
		{
			size_t index = value.index();
			switch (index)
			{
				// string
				case 0:
					d = std::get<0>(value);
					break;

				// bool
				case 1:
					d = std::get<1>(value).value;
					break;
				default:
					THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
								"This is a Bug. Please report it at %1%.",
								BUGTRACKER);
			}
			return;
		}
		std::vector<TemplateKey> reduced{path};
		reduced.erase(reduced.begin());

		size_t idx = path.front().index();
		switch (idx)
		{
			case 0:
				Set(d[std::get<std::string>(path.front())], reduced, value);
				break;
			case 1:
				Set(d[std::get<int>(path.front())], reduced, value);
				break;
			default:
				THROW_FATAL("TemplateData::impl: Unhandled Variant value. "
							"This is a Bug. Please report it at %1%.",
							BUGTRACKER);
		}
#endif
#if USE_VARIANT_API == 2
		// boost
		// break the recursion
		if (path.empty())
		{
			if (const std::string *p = boost::get<std::string>(&value))
			{
				d = *p;
			}
			else if (const boolean *p = boost::get<boolean>(&value))
			{
				d = p->value;
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

		const auto &switchdata = path.front();
		if (const std::string *p = boost::get<std::string>(&switchdata))
		{
			Set(d[*p], reduced, value);
		}
		else if (const int *p = boost::get<int>(&switchdata))
		{
			// this must be an array
			if (!d.is_array())
			{
				d = json::array();
			}
			Set(d[*p], reduced, value);
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
	mp_impl->Set(mp_impl->data, path, value);
}
std::string TemplateData::to_string() const
{
	std::ostringstream oss;
	oss << mp_impl->data.dump(4);
	return oss.str();
}


// ----------------------------------------------------------------------------
// the inja wrapper
// ----------------------------------------------------------------------------
struct TemplateWrapper::impl
{
#ifndef TEST_OLDER_COMPILERS
	// sadly, as the environment is not const for render, we
	// need to use an own environment for all CreateXYZ functions
	inja::Environment m_env;
	std::map<fs::path, inja::Template> m_tpl;
#endif
};

TemplateWrapper::TemplateWrapper(std::vector<fs::path> templ_directories, fs::path tmpl_ext)
{
	mp_impl = std::make_unique<impl>();

	// read all templates into the environment
	for (auto& templ_directory : templ_directories)
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

#ifndef TEST_OLDER_COMPILERS
			try
			{
				inja::Template tpl = mp_impl->m_env.parse_template(path.string());
				mp_impl->m_tpl[path] = tpl;
				mp_impl->m_env.include_template(path.filename().string(), tpl);
			}
			catch (const std::exception &ex)
			{
				THROW_FATAL("TemplateWrapper: File: '%1%' - Error: %2%",
					path.string(), ex.what());
			}
#endif
		}
	}
}

TemplateWrapper::~TemplateWrapper() {}
std::string TemplateWrapper::Render(const fs::path &tpl_file,
									const TemplateData &data) const
{
#ifndef TEST_OLDER_COMPILERS
	auto pos = mp_impl->m_tpl.find(tpl_file);
	if (pos == mp_impl->m_tpl.end())
	{
		THROW_FATAL("TemplateWrapper: template '%1% 'not found in environment.",
					tpl_file.string());
	}

	std::string rendered;
	try
	{
		/*
		std::ostringstream oss;
		oss << data.mp_impl->data;
		LOG_DEBUG("Try to render: %1%", oss.str());
		*/
		// making a copy preserves the outer data structure in any case
		inja::Environment render_env = mp_impl->m_env;
		json render_data = data.mp_impl->data;

		rendered = render_env.render_template(pos->second, render_data);
	}
	catch (std::exception &e)
	{
		THROW_FATAL("inja error trying to render '%1%' - %2%.\n   json=%3%",
					tpl_file.string(), e.what(), data.to_string());
	}
	return rendered;
#else
	// return dummy data
	return "TEST_OLDER_COMPILERS";
#endif
}
