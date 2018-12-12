/*
 * blogcpp :: https://www.blogcpp.org
 */


// clang-format off
/*
                   Flow of operation


                   +-----------------------------------------------------------+
                   |                                                           |
FilesToMetadata    |  CollectPostData                                          |   1 Thread
                   |                                                           |
                   +-----------------------------------------------------------+
                   |                                                           |
                   |  GatherAllFiles                                           |   1 Thread
                   |                                                           |
                   +-------------------------+---------------------------------+
                   |                         |                                 |
                   |  Pages                  |             Posts               |   2 Threads
                   |                         |                                 |
                   +-------------------------+---------------------------------+
                   |                                                           |
                   |                 Check for Files                           |
                   |                 CheckMetadata                             |   1 Thread
                   |                 CheckArchives                             |
                   |                                                           |
                   +--------------------------+--------------------------------+
                                              |
                     (Here flows Metadata)    |
                                              |
                   +--------------------------v--------------------------------+
                   |                                                           |
MetadataToHTML     |  CmdGenerateMetadata                                      |   1 Thread
                   |                                                           |
                   +------------+-------------+----------------+---------------+
                   |            |             |                |               |
                   | CreatePost | CreatePages | CreateArchives |  CreateIndex  |   num-threads
                   |            |             |                |               |
                   |            |             |                |               |
                   +------------+-------------+----------------+---------------+
                                              |
                    (Here flows Metadata)     |
                                              |
                   +--------------------------v--------------------------------+
MetadataToRSS      |                                                           |
                   |  CreateRSS                                                |   1 Thread
                   |                                                           |
                   +-----------------------------------------------------------+
                   |                                                           |
                   |  RSSGenerator (n times in parallel)                       |   num-threads
                   |                                                           |
                   +-----------------------------------------------------------+

*/
// clang-format on


#include <system_error>
#include <fstream>

#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
namespace po = boost::program_options;

#include <boost/locale.hpp>

#include "Shared/Debug.h"

#include "Shared/ConfigCollection.h"
#include "Shared/Helpers.h"
#include "Shared/constants.h"

#include "CmdGenerateMetadata.h"
#include "CmdNewArticle.h"
#include "CmdNewPage.h"

#include "MetadataToHTML/CreateHTML.h"
#include "MetadataToRSS/CreateRSS.h"

// include the generated h files from the GENERATE_CXX stage
// this is the default template and the HOWTO
#include "template-templates-default-archives-txt.h"
#include "template-templates-default-footer-txt.h"
#include "template-templates-default-header-txt.h"
#include "template-templates-default-index-txt.h"
#include "template-templates-default-page-txt.h"
#include "template-templates-default-post-txt.h"
#include "template-templates-default-sidebar-txt.h"

#include "template-templates-default-static-blogcpp-js.h"
#include "template-templates-default-static-favicon-ico.h"
#include "template-templates-default-static-logo-svg.h"
#include "template-templates-default-static-madeincpp-png.h"
#include "template-templates-default-static-manifest-json.h"
#include "template-templates-default-static-styles-css.h"
#include "template-templates-default-static-vanillajs-png.h"

#include "template-README-md.h"

// this is awrapper function to generate the blog or the default
// blog with the default template
void GenerateData(const ConfigCollection &cfgs)
{
	bool run = true;
	while (run)
	{
		try
		{
			// transform the metadata to total const metadata
			ConstMetadata metadata(CmdGenerateMetadata(cfgs));

			// now create the HTMLs and RSS
			CreateHTML(metadata, cfgs);
			CreateRSS(metadata, cfgs);
			run = false;
		}
		catch (const NoFilesFound &)
		{
			PRINT("Could not read files. Create a new blog at '%1%'? "
				  "(y/N)",
				  fs::current_path().string());

			std::string answer;
			std::cin >> answer;
			if (lowercase(answer) != translate("y"))
			{
				return;
			}

			// generate the howto
			fs::create_directories(cfgs.indir_posts());
			fs::create_directories(cfgs.indir_pages());
			std::string data = {{
				"Author: CreateHowto\r\n"
				"Title: Howto use blogcpp\r\n"
				"Slug: howto-use-blogcpp\r\n"
				"Date: 2018-11-22 19:10:04\r\n"
				"Tags: howto\r\n"
				"Categories: blogcpp;howto\r\n"
				"Series: \r\n"
			}};
			data += HEADER_DIVIDER;
			data += "\r\n\r\n";

			std::copy(std::begin(bin2cxx_README), std::end(bin2cxx_README),
				std::back_inserter<>(data));

			write_file(cfgs.indir_posts() / "2018-12-01 HOWTO.txt",
				reinterpret_cast<const unsigned char*>(data.data()), data.size());

			// check template
			if (!fs::exists(cfgs.tpldir()))
			{
				PRINT("No template directory found at '%1%. Create a default "
					  "template? "
					  "(y/N)",
					  cfgs.tpldir().string());

				std::cin >> answer;
				if (lowercase(answer) == translate("y"))
				{
					PRINT("Generating default template at '%1%.",
						  cfgs.tpldir().string());
					fs::create_directories(cfgs.tpldir());
					fs::create_directories(cfgs.tpldir() /
										   cfgs.rel_path_static());

					// clang-format off
					write_file(cfgs.tpldir() / "archives.txt", bin2cxx_archives.data(), bin2cxx_archives.size());
					write_file(cfgs.tpldir() / "footer.txt", bin2cxx_footer.data(), bin2cxx_footer.size());
					write_file(cfgs.tpldir() / "header.txt", bin2cxx_header.data(), bin2cxx_header.size());
					write_file(cfgs.tpldir() / "index.txt", bin2cxx_index.data(), bin2cxx_index.size());
					write_file(cfgs.tpldir() / "page.txt", bin2cxx_page.data(), bin2cxx_page.size());
					write_file(cfgs.tpldir() / "post.txt", bin2cxx_post.data(), bin2cxx_post.size());

					fs::path stat = cfgs.tpldir() / "static";
					write_file(stat / "blogcpp.js", bin2cxx_blogcpp.data(), bin2cxx_blogcpp.size());
					write_file(stat / "favicon.ico", bin2cxx_favicon.data(), bin2cxx_favicon.size());
					write_file(stat / "logo.svg", bin2cxx_logo.data(), bin2cxx_logo.size());
					write_file(stat / "madeincpp.png", bin2cxx_madeincpp.data(), bin2cxx_madeincpp.size());
					write_file(stat / "manifest.json", bin2cxx_manifest.data(), bin2cxx_manifest.size());
					write_file(stat / "styles.css", bin2cxx_styles.data(), bin2cxx_styles.size());
					write_file(stat / "vanillajs.png", bin2cxx_vanillajs.data(), bin2cxx_vanillajs.size());
					// clang-format on
				}
			}

			// ask for generation
			PRINT("Ready to generate the newly created blog at '%1%'. Start "
				  "generation? "
				  "(y/N)",
				  fs::current_path().string());

			std::cin >> answer;
			if (lowercase(answer) != translate("y"))
			{
				return;
			}
		}
	}
}

int main(int argc, char **argv)
{
	Debug::Status debug_status;

	try
	{
		// the complete configuration
		ConfigCollectionFile cfgs;
		cfgs.executable = argv[0];

		// clang-format off
		// the config file name
		std::string cfg_file_name;
		po::options_description cfg_file("Configuration file");
		cfg_file.add_options()
			("config", 			po::value<std::string>(&cfg_file_name)->default_value("blog.cfg"),
								"Uses a different config file than blog.cfg.")
		;

		// the options to get from config file
		po::options_description config_file_visible("Config file options. Just from the file");
		config_file_visible.add_options()
			("sitetitle", 		po::value<std::string>(&cfgs.cfg_sitetitle)->default_value("Just Another Blog"),
								"The title of the site")

			("subtitle",		po::value<boost::optional<std::string>>(&cfgs.cfg_subtitle),
								"The subtitle is entirely optional - you can comment "
								"it out or completely remove the line if you wish.")

			("siteurl",			po::value<std::string>(&cfgs.cfg_siteurl)->default_value(""),
								"Your 'index' URL. Please do NOT add a trailing slash. "
								"The URL can be absolute (e.g. http://example.com/yoursite) "
								"or relative to your domain path (e.g. /yoursite).")

			("author", 			po::value<std::string>(&cfgs.cfg_author),
								"When using the --new-page and --new-article options, "
								"blogcpp will automatically fill in the 'Author:' field "
								"with the following name if enabled.")

			("indir", 			po::value<std::string>(&cfgs.cfg_indir)->default_value("site"),
								"Your posts and pages must reside in {indir}/posts/ and "
								"{indir}/pages.")

			("outdir",			po::value<std::string>(&cfgs.cfg_outdir)->default_value("outdir"),
								"blogcpp will use the {outdir} to generate your HTML files.")

			("series",			po::value<bool>(&cfgs.cfg_series)->default_value(true),
								"blogcpp supports article series, to be added per input file. "
								"You can turn this feature off by setting this variable to "
								"'false' otherwise, blogcpp will respect the per-file settings.")

			("emojis",			po::value<bool>(&cfgs.cfg_emojis)->default_value(true),
								"blogcpp supports automatic Emoji conversion. "
								"If you don't want that, you can set this feature 'false' here.")

			("embeds",			po::value<bool>(&cfgs.cfg_embeds)->default_value(true),
								"blogcpp can automatically embed media from certain websites "
								"(like YouTube) if it finds a media link on a separate line. "
								"If blogcpp was compiled with WITH_OEMBED, this option tries "
								"to find and process OEmbeds behind all of those links; if "
								"WITH_OEMBED is not set, only basic replacements are made. "
								"Set this to 'false' if you want to disable this behavior.")

			("excerptlength",	po::value<int>(&cfgs.cfg_excerpts)->default_value(2),
								"By default, the index page will have a shortened preview "
								"of your blog contents, e.g. a value of 2 will show only "
								"the first two sentences. Set this to 0 to show the full "
								"text on all pages. This can still be overridden by adding "
								"the '<!--more-->' tag where you want to shorten the preview.")


			("excerpttext",		po::value<std::string>(&cfgs.cfg_excerpttext)->default_value("..."),
								"You can, of course, also modify the link text for the "
								"'more...' link.")

			("permalinks", 		po::value<std::string>(&cfgs.cfg_permalinks)->default_value("%Y/%m"),
								"By default, blogcpp parses single article links into "
								"a path structure like 'example.com/year/month/slug'. "
								"You can change this behavior below, replacing the "
								"'year/month' part by a date format of your choice "
								"(e.g. '%Y/%m/%d' to add a 'day' part). blogcpp will read "
								"this variable as: <siteurl>/<subdir>/<permalinks>/<slug>. "
								"Please don't end this variable with a slash or hell "
								"will break loose.\n\n"
								"A list of possible specifiers (the character after '%')"
								"is available here: http://en.cppreference.com/w/cpp/chrono/c/strftime")

			("articlepath", 	po::value<std::string>(&cfgs.cfg_article_subdir)->default_value("posts"),
								"Pages and articles are usually prefixed by /pages and "
								"/posts. The following two settings can be used to change "
								"this behavior. "
								"If you do not want to have separate subdirectories, please "
								"just set the variables to a single slash ('/').")

			("pagepath", 		po::value<std::string>(&cfgs.cfg_page_subdir)->default_value("pages"),
								"Pages and articles are usually prefixed by /pages and "
								"/posts. The following two settings can be used to change "
								"this behavior. "
								"If you do not want to have separate subdirectories, please "
								"just set the variables to a single slash ('/').")

			("maxitems", 		po::value<int>(&cfgs.cfg_maxitems)->default_value(25),
								"By default, blogcpp shows 25 items per page. "
								"This value is also used for the number of items in your RSS feed.")

			("fullfeed",		po::value<bool>(&cfgs.cfg_fullfeed)->default_value(false),
								"If you want to enforce your RSS feed's readers to visit "
								"your site more often, you can turn the 'full feed' off "
								"so blogcpp will use the 'excerptlength' for article "
								"previews. If this is set to 'true', the RSS feed will "
								"contain the full article text.")

			("rssdir",			po::value<std::string>(&cfgs.cfg_feeddir)->default_value("RSS"),
								"By default, RSS feeds will be placed in the RSS/ subfolder. "
								"You can modify this behavior with this setting")

			("commenttype",		po::value<boost::optional<std::string>>(&cfgs.cfg_commenttype),
								"If it is not set, comments are off.\n"
								"Choose one of these:\n\n"
								"\tdisqus, isso, hashover, google, discourse, commento.")

			("commentdata",		po::value<boost::optional<std::string>>(&cfgs.cfg_commentdata),
								"If you want to use comments, the following parameter "
								"determines parameters for certain commenting systems.\n\n"
								"\tFor Disqus: Enter your Disqus shortname here.\n\n"
								"\tFor Discourse: Enter your Discourse URL here.\n\n"
								"\tFor isso: Enter your isso installation URL here.\n\n"
								"\tFor Hashover: Enter your Hashover installation URL here.\n\n"
								"\tFor Commento: Enter your Commento server URL here.\n\n"
								"Note that these URLs must not end in a slash.")

			("maxhistory",		po::value<int>(&cfgs.cfg_maxhistory)->default_value(5),
								"The maxhistory setting sets the number of latest posts "
								"in e.g. your sidebar.")


			("template",		po::value<std::string>(&cfgs.cfg_template)->default_value("default"),
								"Templates must reside in templates/{template}/")

			("locale",			po::value<std::string>(&cfgs.cfg_locale)->default_value("en_US.utf8"),
								"You might want to set your blog's locale here. "
								"This mainly affects the displayed date format.")

			("opengraphimg", po::value<std::string>(&cfgs.cfg_opengraphimg)->default_value("http://www.example.com/opengraph.png"),
								"blogcpp supports OpenGraph meta data. However, it requires "
								"to know the absolute URL to an image file of your choice "
								"to be displayed. If you want to enable support for the "
								"OpenGraph technology on all pages, please set the URL "
								"of your desired image file below. "
								"Note that single posts/pages can have separate OpenGraph "
								"image URLs inside their meta data.")

#ifdef WITH_PLUGINS
			("plugins", 			po::value<bool>(&cfgs.cfg_plugins)->default_value(false),
									"You can en- or disable plug-in support per site: Just "
									"switch this flag from 'false' to 'true'.")

			("plugindir", 			po::value<std::string>(&cfgs.cfg_plugindir)->default_value("plugins"),
									"If compiled with WITH_PLUGINS, blogcpp will search your "
									"plug-in files in the {plugindir} directory.")
#endif
		;

		po::options_description config_file_sidebar_visible("Sidebar Links");
		config_file_sidebar_visible.add_options()
			("link0_url",		po::value<boost::optional<std::string>>(&cfgs.link_url[0])->default_value(std::string("https://www.blogcpp.org")),
								"URLs for your sidebar can be specified as follows. "
								"(blogcpp supports optional hover titles, too.) "
								"Note that currently max. 100 URLs are supported, but "
								"I guess you can live with that. :-)")

			("link0_text",		po::value<boost::optional<std::string>>(&cfgs.link_text[0])->default_value(std::string("blogcpp.org")))
			("link0_title",		po::value<boost::optional<std::string>>(&cfgs.link_title[0])->default_value(std::string("Just another blog software.")))


			("link1_url",		po::value<boost::optional<std::string>>(&cfgs.link_url[1])->default_value(std::string("https://bitbucket.org/tux_/blogcpp")),
								"URLs for your sidebar can be specified as follows. "
								"(blogcpp supports optional hover titles, too.) "
								"Note that currently max. 100 URLs are supported, but "
								"I guess you can live with that. :-)")

			("link1_text",		po::value<boost::optional<std::string>>(&cfgs.link_text[1])->default_value(std::string("blogcpp on Bitbucket")))
			("link1_title",		po::value<boost::optional<std::string>>(&cfgs.link_title[1])->default_value(std::string("")))

			;

		po::options_description config_file_sidebar_invisible;
		for (size_t i = 2; i < MAX_CONFIG_SIDEBAR_LINKS; ++i)
		{
			std::string url = "link" + std::to_string(i) + "_url";
			std::string text = "link" + std::to_string(i) + "_text";
			std::string title = "link" + std::to_string(i) + "_title";
			config_file_sidebar_invisible.add_options()
				(url.c_str(),		po::value<boost::optional<std::string>>(&cfgs.link_url[i]))
				(text.c_str(),		po::value<boost::optional<std::string>>(&cfgs.link_text[i]))
				(title.c_str(),		po::value<boost::optional<std::string>>(&cfgs.link_title[i]))
			;
		}

		// Declare the supported commands.
		// clang-format off
		po::options_description commands("Available commands");
		commands.add_options()
			("help,h", 			"Prints this help")
			("version,v",		"Prints version information.")
			("verbosity",		po::value<int>(&cfgs.verbosity)->default_value(2),
								"Quietly generates your site.")
			("new-article", 	"Creates a new article with your $EDITOR.")
			("new-page",		"Creates a new page with your $EDITOR.")
#ifdef WITH_PLUGINS
			("list-plugins",	"Lists all available plug-ins in your plug-in directory.")
			("enable-plugin",	po::value<std::string>(), "Enables the given plug-in if disabled.")
			("disable-plugin",	po::value<std::string>(), "Disables the given plug-in if enabled.")
#endif
			("num-threads",		po::value<int>(&cfgs.num_threads)->default_value(0),
								"The number of threads to compute. (0 = hardware concrrency)")
		;
		// clang-format on

		// combine them all up
		po::options_description cmdline_options("Commandline Options");
		cmdline_options.add(commands).add(cfg_file);

		po::options_description config_file_options_visible;
		config_file_options_visible.add(config_file_visible)
			.add(config_file_sidebar_visible);

		// now get the value of the command line to get the config file
		po::variables_map vm;
		po::store(
			po::command_line_parser(argc, argv).options(cmdline_options).run(),
			vm);
		po::notify(vm);

		// check for help and version
		if (vm.count("help"))
		{
			std::cout << std::boolalpha << cmdline_options << std::endl
					  << config_file_options_visible << std::endl;
			return EXIT_FAILURE;
		}

		else if (vm.count("version"))
		{
			// Prints version information.
			PRINT("This is blogcpp version %1%.\n"
				  "Url: %2%\n\n"
				  "Licensed under the terms of the WTFPL v2.\n"
				  "See http://wtfpl.net/txt/copying for relevant information.",
				  APP_VERSION, APP_URL);

			return EXIT_FAILURE;
		}

		// read the config file
		if (fs::exists(cfg_file_name) && fs::is_regular_file(cfg_file_name))
		{
			LOG_WARN("Reading file %1%", cfg_file_name);
			{
				std::ifstream ifs(cfg_file_name);
				if (!ifs.is_open())
				{
					THROW_FATAL("Could not open existing file '%1%'.", cfg_file_name);
				}
				po::store(po::parse_config_file(ifs,
												config_file_options_visible,
												true /* allow unregistered */),
					  vm);
			}
			{
				std::ifstream ifs(cfg_file_name);
				if (!ifs.is_open())
				{
					THROW_FATAL("Could not open existing file '%1%'.", cfg_file_name);
				}
				po::store(po::parse_config_file(ifs,
												config_file_sidebar_invisible,
												true /* allow unregistered */),
						vm);
			}
		}
		else
		{
			PRINT("Could not read '%1%', using defaults.", cfg_file_name);
			po::store(po::command_line_parser(argc, argv)
						  .options(config_file_options_visible)
						  .allow_unregistered()
						  .run(),
					  vm);

			po::store(po::command_line_parser(argc, argv)
						  .options(config_file_options_visible)
						  .allow_unregistered()
						  .run(),
					  vm);
		}
		po::notify(vm);

		// this checks the configuration
		ConfigCollection work_cfg(cfgs);

		// Log Verbosity
		debug_status.SetVerbosity(work_cfg.verbosity());

		// set the global locale
		boost::locale::generator gen;
		std::locale::global(gen(cfgs.cfg_locale));

		// now the "real commands"
		if (vm.count("new-article"))
		{
			CmdNewArticle(work_cfg);
		}
		else if (vm.count("new-page"))
		{
			CmdNewPage(work_cfg);
		}

		// no additional command: generate
		else
		{
			PRINT("Using configuration file: %1%", cfg_file_name);
			GenerateData(work_cfg);
		}
		return EXIT_SUCCESS;
	}
	catch (const Debug::THROWN &)
	{
		// already printed
	}
	catch (const boost::program_options::error &ex)
	{
		LOG_FATAL("Sorry: %1%", ex.what());
	}
	catch (const std::exception &ex)
	{
		LOG_FATAL("Sorry: %1%", ex.what());
	}
	catch (const std::error_code &ec)
	{
		LOG_FATAL("Sorry: %1% - %2%", ec, ec.message());
	}
	catch (...)
	{
		LOG_FATAL("Sorry: Unknown error. Please file a Bugreport at %1%",
				  BUGTRACKER);
	}
	return EXIT_FAILURE;
}
