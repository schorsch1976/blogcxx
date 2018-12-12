/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of Duktape plug-ins [header].
 */

#pragma once

#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "Shared/filesystem.h"

#include <dukglue/dukglue.h>


class Exports
{
public:
	Exports();
	std::string getVersion();
};

class PluginFile
{
public:
	bool enabled;
	std::string filename;
	std::string shortname; // usually, <filename> without the extensions.
	std::vector<char> vc_affects;
};

class Plugins
{
private:
	std::string plugindir;
	duk_context *ctx;
	std::mutex mtx;
	bool enabled;
	bool traversePlugins(std::vector<PluginFile> *pfiles, std::string directory,
						 char affects);

public:
	virtual ~Plugins();

	void init(bool use_plugins, std::string in_plugindir);
	std::string enable(std::string filename);
	std::string disable(std::string filename);
	std::string listAll();
	std::string processContentPlugins(std::string input);
	std::string processHeaderPlugins();
	std::string processFooterPlugins();
};
