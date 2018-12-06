/*
* blogcpp :: https://www.blogcpp.org
*/

#include "CollectPostData.h"

#include "Shared/AsyncWorker.h"
#include "Shared/Helpers.h"
#include "Shared/constants.h"

#include "SharedHTML-RSS/Slug.h"

#include <regex>

namespace
{
template <typename T>
void MakeUnique(std::vector<T> &v)
{
	std::sort(std::begin(v), std::end(v));
	auto last = std::unique(std::begin(v), std::end(v));
	v.erase(last, std::end(v));
}
void DetectSeries(Metadata &merged)
{
	LOG_DEBUG("DetectSeries: Collecting series...");
	for (auto &si : merged.all_items)
	{
		if (si->s_series.empty() || si->type != ItemType::Post)
		{
			continue;
		}

		merged.series[si->s_series].push_back(si);
	}

	LOG_DEBUG("Sorting series.");
	for (auto &single_series_data : merged.series)
	{
		auto &items_for_this_series = single_series_data.second;

		std::sort(std::begin(items_for_this_series),
				  std::end(items_for_this_series), time_smaller);
	}

	LOG_DEBUG("DetectSeries: Done.");
}

void DetectArchives(Metadata &merged, const ConfigCollection &cfgs)
{
	LOG_DEBUG("DetectArchives...");

	std::map<int, Archive> years;
	std::map<std::pair<int, int>, Archive> years_months;

	std::map<std::string, Archive> cats;
	std::map<std::string, Archive> tags;

	std::map<std::string, Archive> authors;

	for (auto &si : merged.all_items)
	{
		years[si->time.tm_year].push_back(si);
		years_months[std::make_pair(si->time.tm_year, si->time.tm_mon)]
			.push_back(si);

		for (auto &c : si->cats)
		{
			cats[c].push_back(si);
		}
		for (auto &c : si->tags)
		{
			tags[c].push_back(si);
		}
		authors[si->s_author].push_back(si);
	}

	LOG_DEBUG("Make entries in archives unique and sorted by time...");
	for (auto &ar : years)
	{
		MakeUnique(ar.second);
		std::sort(std::begin(ar.second), std::end(ar.second), time_greater);
	}
	for (auto &ar : years_months)
	{
		MakeUnique(ar.second);
		std::sort(std::begin(ar.second), std::end(ar.second), time_greater);
	}
	for (auto &ar : cats)
	{
		MakeUnique(ar.second);
		std::sort(std::begin(ar.second), std::end(ar.second), time_greater);
	}
	for (auto &ar : tags)
	{
		MakeUnique(ar.second);
		std::sort(std::begin(ar.second), std::end(ar.second), time_greater);
	}
	for (auto &ar : authors)
	{
		MakeUnique(ar.second);
		std::sort(std::begin(ar.second), std::end(ar.second), time_greater);
	}

	LOG_DEBUG("Add archives to the merged data");
	for (auto &c : years)
	{
		// we know all items are years, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::Year;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"year (%1%) archive. Please fill a bugreport at %2%.",
				c.first, BUGTRACKER);
		}
		ad.time = c.second.front()->time;
		ad.path = cfgs.rel_path_archive_year(c.second.front()->time);
		merged.archives[ad] = c.second;
	}
	for (auto &c : years_months)
	{
		// we know all items are years_months, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::YearMonth;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"year_month (%1%/%2%) archive. Please fill a bugreport at %3%.",
				c.first.first, c.first.second, BUGTRACKER);
		}

		ad.time = c.second.front()->time;
		ad.path = cfgs.rel_path_archive_year_month(c.second.front()->time);

		merged.archives[ad] = c.second;
	}
	for (auto &c : cats)
	{
		// we know all items are categories, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::Categories;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"category (%1%) archive. Please fill a bugreport at %2%.",
				c.first, BUGTRACKER);
		}

		ad.path = cfgs.rel_path_categories(c.first);

		merged.archives[ad] = c.second;
	}
	for (auto &c : tags)
	{
		// we know all items are tags, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::Tags;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"tag (%1%) archive. Please fill a bugreport at %2%.",
				c.first, BUGTRACKER);
		}

		ad.path = cfgs.rel_path_tags(c.first);

		merged.archives[ad] = c.second;
	}
	for (auto &c : authors)
	{
		// we know all items are tags, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::Author;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"author (%1%) archive. Please fill a bugreport at %2%.",
				c.first, BUGTRACKER);
		}

		ad.path = cfgs.rel_path_authors(c.first);

		merged.archives[ad] = c.second;
	}
	for (auto &c : merged.series)
	{
		// we know all items are tags, so take the first one
		ArchiveData ad;
		ad.type = ArchiveType::Series;
		if (c.second.empty())
		{
			THROW_FATAL(
				"This is a Bug. There must be ar least one article in this "
				"series (%1%) archive. Please fill a bugreport at %2%.",
				c.first, BUGTRACKER);
		}

		ad.path = cfgs.rel_path_series(c.first);

		merged.archives[ad] = c.second;
	}

	LOG_DEBUG("DetectArchives: Done.");
}

void Slugify(Metadata &merged)
{
	// slugify it
	std::vector<std::string> all_slug_names;
	std::map<std::string, int> count_doublicate_slugs;

	if (merged.all_items.empty())
	{
		LOG_WARN("No items found.");
		return;
	}

	// count the slug names and collect them
	{
		for (auto &si : merged.all_items)
		{
			if (si->type != ItemType::Post)
			{
				continue;
			}

			if (si->s_slug.empty())
			{
				si->s_slug = createBasicSlug(si->s_filename.string());
			}
			count_doublicate_slugs[si->s_slug] += 1;
			all_slug_names.push_back(si->s_slug);
		}
		MakeUnique(all_slug_names);
	}

	// check if they are unique
	auto pos = std::max_element(std::begin(count_doublicate_slugs),
								std::end(count_doublicate_slugs),
								[](const std::pair<std::string, int> &lhs,
								   const std::pair<std::string, int> &rhs) {
									return lhs.second < rhs.second;
								});

	if (pos->second <= 1)
	{
		LOG_INFO("All slugs are unique.");
		return;
	}

	LOG_INFO("Not all slugs are unique... extending them");

	std::vector<SingleItem::Ptr> all_doublicate_si;

	for (auto &c : count_doublicate_slugs)
	{
		if (c.second <= 1)
		{
			continue;
		}

		// this is a doublicate. fetch the SingleItems
		std::vector<SingleItem::Ptr> doublicates;
		for (auto &si : merged.all_items)
		{
			if (si->s_slug == c.first)
			{
				doublicates.push_back(si);
			}
		}

		// order the duplicates by their time
		std::sort(std::begin(doublicates), std::end(doublicates), time_smaller);

		// now add an ID to it
		for (size_t i = 0; i < doublicates.size(); ++i)
		{
			std::string after =
				doublicates[i]->s_slug + "-" + std::to_string(i);
			LOG_INFO("Extending %1% -> %2%", doublicates[i]->s_slug, after);
			doublicates[i]->s_slug = after;
		}
	}

	// do it again
	Slugify(merged);
}
} // namespace

void CheckArchive(const std::string msg, const Metadata &merged,
				  const std::map<std::string, Archive> &ar)
{
	for (const auto &a : ar)
	{
		if (a.first.empty())
		{
			THROW_ERROR("There is an empty %1% record in metadata.", msg);
		}
		for (SingleItem::ConstPtr si : a.second)
		{
			auto pos = std::find_if(
				std::begin(merged.all_items), std::end(merged.all_items),
				[si](SingleItem::ConstPtr item) -> bool { return si == item; });
			if (pos == std::end(merged.all_items))
			{
				THROW_ERROR("This is a bug.\n'%1%' points outside of "
							"all_items. Please report at %2%",
							si->s_filename.string(), BUGTRACKER);
			}
		}
	}
}

void CheckMetadata(const Metadata &merged)
{
	PRINT("Checking the collected Metadata for consistency.");

	// check items
	for (const auto &si : merged.all_items)
	{
		if (si->s_author.empty())
		{
			THROW_ERROR("File '%1%' has an empty author.",
						si->s_filename.string());
		}

		if (!(si->type == ItemType::Page || si->type == ItemType::Post))
		{
			THROW_ERROR("File '%1%' has an undefined type. Please fill a "
						"Bugreport at %2%.",
						si->s_filename.string(), BUGTRACKER);
		}

		if (si->s_title.empty())
		{
			THROW_ERROR("File '%1%' has an empty title.",
						si->s_filename.string());
		}

		if (si->s_series.size() && si->type != ItemType::Post)
		{
			THROW_ERROR("File '%1%' is not a post but has series set.",
						si->s_filename.string());
		}

		if (si->s_slug.empty())
		{
			THROW_ERROR("File '%1%' has an empty slug. Please fill a "
						"Bugreport at %2%.",
						si->s_filename.string(), BUGTRACKER);
		}

		if (si->time.tm_hour > 23 || si->time.tm_hour < 0 ||
			si->time.tm_min > 59 || si->time.tm_hour < 0 ||
			si->time.tm_sec > 60 || si->time.tm_sec < 0 ||

			si->time.tm_wday > 6 || si->time.tm_wday < 0 ||

			si->time.tm_mday > 31 || si->time.tm_mday < 0 ||
			si->time.tm_mon > 11 || si->time.tm_mon < 0 ||
			si->time.tm_yday > 365 || si->time.tm_yday < 0 ||
			si->time.tm_year < 0)
		{
			THROW_ERROR("File '%1%' has an invalid time set.",
						si->s_filename.string());
		}
	}

	// check for empty string in metadata
	LOG_INFO("Checking authors list.");
	CheckArchive("authors", merged, merged.authors);

	LOG_INFO("Checking series list.");
	CheckArchive("series", merged, merged.series);

	LOG_INFO("Checking categories list.");
	CheckArchive("categories", merged, merged.categories);

	LOG_INFO("Checking tags list.");
	CheckArchive("categories", merged, merged.tags);

	PRINT("Metadata is consistent.");
}

SingleItem::Ptr CollectPostDataHelper(ItemType type,
									  const ConfigCollection &cfgs,
									  fs::path inputfile)
{
	SingleItem::Ptr ret = std::make_shared<SingleItem>(type);
	SingleItem &si = *ret;

	// Fills vsi_posts, vsi_pages, vs_cats and vs_tags before processing them
	// entirely.
	std::stringstream ss_output;
	std::string s_cats, s_tags, s_datetime, s_change_datetime, s_author,
		s_title, s_slug, s_markdown, s_emoji, s_sticky, s_comments, s_ogimage,
		s_series;
	int i_position = 0;
#ifdef WITH_PLUGINS
	std::string s_plugins;
#endif
	tm tm_t = {}, change_tm_t = {};

	LOG_DEBUG("Gathering categories and tags from the %1% from '%2%'", type,
			  inputfile);

	std::string file_contents = read_file(inputfile);

	LOG_DEBUG("File contents for '%1%' are set.", inputfile);

	// file_contents contains the input text + meta data now.
	// Let's first read the meta data (until HEADER_DIVIDER):
	std::string line;
	std::istringstream ss_contents(file_contents);

	std::regex re_author("^Author\\s*:[\\s\\t]*(.*?)$",
						 std::regex_constants::icase);
	std::regex re_title("^Title\\s*:[\\s\\t]*(.*?)$",
						std::regex_constants::icase);
	std::regex re_slug("^Slug\\s*:[\\s\\t]*(.*?)$",
					   std::regex_constants::icase);
	std::regex re_datetime("^Date\\s*:[\\s\\t]*(\\d{4}\\-\\d{2}\\-\\d{2} "
						   "\\d{2}:\\d{2}:\\d{2}?)$",
						   std::regex_constants::icase);
	std::regex re_change_datetime("^Changed\\s*:[\\s\\t]*(\\d{4}\\-\\d{2}"
								  "\\-\\d{2} \\d{2}:\\d{2}:\\d{2}?)$",
								  std::regex_constants::icase);
	std::regex re_cats("^Categories\\s*:[\\s\\t]*(.*?)$",
					   std::regex_constants::icase);
	std::regex re_tags("^Tags\\s*:[\\s\\t]*(.*?)$",
					   std::regex_constants::icase);
	std::regex re_markdown("^Markdown\\s*:[\\s\\t]*(.*?)$",
						   std::regex_constants::icase);
	std::regex re_emoji("^Emoji\\s*:[\\s\\t]*(.*?)$",
						std::regex_constants::icase);
	std::regex re_sticky("^Sticky\\s*:[\\s\\t]*(.*?)$",
						 std::regex_constants::icase);
	std::regex re_comments("^Comments\\s*:[\\s\\t]*(.*?)$",
						   std::regex_constants::icase);
	std::regex re_ogimage("^OpenGraphImage\\s*:[\\s\\t]*(.*?)$",
						  std::regex_constants::icase);
#ifdef WITH_PLUGINS
	std::regex re_plugins("^Plugins\\s*:[\\s\\t]*(.*?)$",
						  std::regex_constants::icase);
#endif
	std::regex re_series("^Series\\s*:[\\s\\t]*(.*?)$",
						 std::regex_constants::icase);
	std::regex re_position("^Position\\s*:[\\s\\t]*(\\d*?)$",
						   std::regex_constants::icase);

	std::regex re_url("https?://",
					  std::regex_constants::icase); // Avoid stupidities

	std::smatch match;

	while (getline(ss_contents, line))
	{
		if (line == HEADER_DIVIDER)
		{
			// The end of meta data.
			break;
		}

		using std::regex_match;

		try
		{
			if (regex_match(line, match, re_author) && match.size() > 1)
			{
				s_author = match.str(1);
			}
			else if (regex_match(line, match, re_title) && match.size() > 1)
			{
				s_title = match.str(1);
			}
			else if (regex_match(line, match, re_slug) && match.size() > 1)
			{
				s_slug = match.str(1);
			}
			else if (regex_match(line, match, re_datetime) && match.size() > 1)
			{
				// Get the tm from the date/time in the file.
				parseDatestringToTm(match.str(1), inputfile.string(), tm_t);
			}
			else if (regex_match(line, match, re_change_datetime) &&
					 match.size() > 1)
			{
				// Get the tm from the "latest change" date/time in the file.
				parseDatestringToTm(match.str(1), inputfile.string(),
									change_tm_t);
			}
			else if (type != ItemType::Page &&
					 regex_match(line, match, re_cats) && match.size() > 1)
			{
				// It makes no sense to categorize pages.
				s_cats = match.str(1);
			}
			else if (regex_match(line, match, re_tags) && match.size() > 1)
			{
				s_tags = match.str(1);
			}
			else if (regex_match(line, match, re_markdown) && match.size() > 1)
			{
				s_markdown = match.str(1);
			}
			else if (regex_match(line, match, re_emoji) && match.size() > 1)
			{
				s_emoji = match.str(1);
			}
			else if (regex_match(line, match, re_sticky) && match.size() > 1)
			{
				s_sticky = match.str(1);
			}
			else if (type != ItemType::Page &&
					 regex_match(line, match, re_comments) && match.size() > 1)
			{
				s_comments = match.str(1);
			}
			else if (regex_match(line, match, re_ogimage) &&
					 regex_match(line, match, re_url) && match.size() > 1)
			{
				// Add only valid URLs
				s_ogimage = match.str(1);
			}
#ifdef WITH_PLUGINS
			else if (regex_match(line, match, re_plugins) && match.size() > 1)
			{
				s_plugins = match.str(1);
			}
#endif
			else if (regex_match(line, match, re_series) && match.size() > 1)
			{
				s_series = match.str(1);
			}
			else if (regex_match(line, match, re_position) && match.size() > 1)
			{
				i_position = stoi(match.str(1));
			}
		}
		catch (const std::regex_error &e)
		{
			// Syntax error in the regular expression.
			LOG_FATAL("An error occurred while trying to match a regular "
					  "expression: %1%",
					  e.what());
			LOG_FATAL("Please file a blogcpp bug so we can investigate and "
					  "fix it for you.");
			LOG_FATAL(" --> %1%", BUGTRACKER);
			throw;
		}
		catch (const std::invalid_argument &e)
		{
			// Invalid position argument in stoi(). Skip.
			LOG_ERROR("Failed to set the position in article '%1%'.", s_title);
			LOG_ERROR("You might want to check the parameters...? %1%",
					  e.what());

			continue;
		}
	}

	// Find the content and put it into the template.
	bool input_has_started = false;
	while (getline(ss_contents, line))
	{
		// The stringstream already points below the HEADER_DIVIDER.
		// We can safely continue from here.

		// If the line below the divider is empty, we don't really want to keep
		// it.
		if (!line.empty())
		{
			input_has_started = true;
		}
		if (!input_has_started)
		{
			continue;
		}

		// Everything that follows now is a part of what we need.
		ss_output << line << "\n";
	}

	// Positions have to be >= 0.
	if (i_position < 0 && type == ItemType::Page)
	{
		LOG_DEBUG("Fixing invalid page position.");
		i_position = 0;
	}

	si.s_filename = inputfile;

	si.time = tm_t;
	si.changetime = change_tm_t;
	si.s_title = s_title;
	si.s_slug = s_slug;
	si.s_author = s_author;
	si.s_text = ss_output.str();
	si.b_markdown = (lowercase(s_markdown) != "off");
	si.b_emoji = cfgs.emojis() && (lowercase(s_emoji) != "off");
	si.b_sticky = (lowercase(s_sticky) != "off");
	si.b_comments = (lowercase(s_comments) != "off");
	si.s_ogimage = s_ogimage;
	if (cfgs.series() && type != ItemType::Page)
	{
		si.s_series = s_series;
	}
	else if (type == ItemType::Page)
	{
		si.i_position = i_position;
	}

	si.cats = vectorSplit(s_cats);
	si.tags = vectorSplit(s_tags);

	si.type = type;

	return ret;
}

Metadata CollectPostData(const ConfigCollection &cfgs,
						 const AllFilePaths &all_in_files)
{
	AsyncWorker<fs::path, SingleItem::Ptr> collector(cfgs.num_threads());

	auto adder = [&collector, &cfgs](std::vector<fs::path> paths,
									 ItemType type) {
		for (const auto &c : paths)
		{
			collector.Add(c, std::bind(CollectPostDataHelper, type, cfgs, c));
		}
	};

	adder(all_in_files.pages, ItemType::Page);
	adder(all_in_files.posts, ItemType::Post);

	auto all_post_data = collector.GetResults();

	// merge the data
	Metadata merged;
	for (const auto &c : all_post_data)
	{
		if (isFutureDate(c.second->time))
		{
			LOG_WARN("Skipping item with future date. %1%", c.second->s_filename.string());
			continue;
		}
		merged.all_items.push_back(c.second);

		// !!! find the si name in the all_items to prevent dangling pointer
		auto pos = std::find_if(
			std::begin(merged.all_items), std::end(merged.all_items),
			[&c](SingleItem::ConstPtr item) -> bool {
				return c.second->s_filename == item->s_filename;
			});
		if (pos == std::end(merged.all_items))
		{
			THROW_FATAL("Could not fimd item '%1%' in all items. This is a "
						"Bug. Please report it at %2%",
						c.second->s_filename.string(), BUGTRACKER);
		}

		// !!si points to the item in the all_items!!!
		SingleItem::Ptr si = *pos;

		merged.authors[si->s_author].push_back(si);

		for (const auto &cat : si->cats)
		{
			merged.categories[cat].push_back(si);
		}
		for (const auto &tag : si->tags)
		{
			merged.tags[tag].push_back(si);
		}

		if (si->type == ItemType::Page && si->s_series.size())
		{
			merged.series[si->s_series].push_back(si);
		}
	}

	// this requires all shared state. so we can just do it in a single thread
	LOG_DEBUG("Slugify items.");
	Slugify(merged);

	LOG_DEBUG("DetectSeries.");
	DetectSeries(merged);

	LOG_DEBUG("DetectArchives.");
	DetectArchives(merged, cfgs);

	CheckMetadata(merged);

	return merged;
}