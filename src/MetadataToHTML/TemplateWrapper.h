/*
 * blogcpp :: https://www.blogcpp.org
 * Class to make using the inja template engine easier [header].
 */

#pragma once

#include <memory>
#include <string>

#include "Shared/variant.h"
#include "Shared/filesystem.h"

// this is a wrapper to keep c++ from converting everything to a bool
struct boolean
{
	explicit boolean(bool v) : value(v) {}
	bool value;
};

using TemplateKey = variant_ns::variant<std::string, int>;
using ValueKey = variant_ns::variant<std::string, boolean>;

class TemplateWrapper;
class TemplateData
{
public:
	TemplateData();
	~TemplateData(); // needed because the compiler fire wall

	// move, copy
	TemplateData(TemplateData &&rhs);
	TemplateData &operator=(TemplateData &&rhs);

	TemplateData(const TemplateData &rhs);
	TemplateData &operator=(const TemplateData &rhs);

	void Set(std::vector<TemplateKey> path, const ValueKey &value);
	void InsertEmptyArray(std::vector<TemplateKey> path);

private:
	friend class TemplateWrapper;

	// compiler firewall: contain the inja the TemplateWrapper
	struct impl;
	std::unique_ptr<impl> mp_impl;
};

class TemplateWrapper
{
public:
	// store all templates in memory
	explicit TemplateWrapper(fs::path templ_directory,
							 fs::path tmpl_ext = ".txt");
	~TemplateWrapper(); // needed because the compiler fire wall

	std::string Render(const fs::path &tpl_file,
					   const TemplateData &data) const;

private:
	// compiler firewall: contain the inja the TemplateWrapper
	struct impl;
	std::unique_ptr<impl> mp_impl;
};