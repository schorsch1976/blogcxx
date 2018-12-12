/*
 * blogcxx :: https://www.blogcxx.de
 * Generator class for new posts and pages.
 */

#include "ContentsGenerator.h"
#include "Shared/Debug.h"

#include "SharedHTML-RSS/EmojiParserClass.h"

ContentsGenerator::ContentsGenerator(ItemType type, fs::path input_folder,
									 std::string author, std::string title,
									 std::string slug, std::string first_text)
{
	// A new ContentsGenerator has a ContentType passed.
	this->type = type;
	this->input_folder = input_folder;
	this->author = author;
	this->title = title;
	this->slug = slug;
	this->first_text = first_text;
}

void ContentsGenerator::generate()
{
	// Creates a new article or page file, fills the necessary
	// variables and opens it in the system's default text editor.
	EmojiParser eparser;
	std::ostringstream contents;
	fs::path filename{input_folder};

	switch (type)
	{
		case ItemType::Post:
			filename /= timeNow("%Y-%m-%d") + " " +
						trim(hyphenise(eparser.clear(title))) + ".txt";
			contents << createDummyArticle() << first_text;
			break;
		case ItemType::Page:
			filename /= trim(hyphenise(eparser.clear(title))) + ".txt";
			contents << createDummyPage() << first_text;
			break;
		default:
			THROW_FATAL("ContentsGenerator: I can not generate an undefined "
						"type. Please fill a bugreport at %1%'",
						BUGTRACKER);
			break;
	}

	// If this file already exists, we shall not pass.
	if (fs::exists(filename))
	{
		THROW_ERROR("Duplicate contents found - you might want to choose a "
					"new title first.");
	}

	// Write into the file. (Will return false on error.)
	std::ofstream os_file(filename.string());
	if (os_file.is_open())
	{
		LOG_DEBUG("Writing to the file '%1%'.", filename.string());

		os_file << contents.str();
		os_file.close();
	}
	else
	{
		THROW_ERROR("Whoops - I couldn't write into '%1%'", filename.string());
	}

	LOG_DEBUG("File successfully created.");

	LOG_DEBUG("Your stub file '%1%' has been generated.", filename.string());

	if (first_text.empty())
	{
		PRINT("Trying to launch your default editor with it...");

		openWithEditor(filename);
	}
}

std::string ContentsGenerator::createDummyPage()
{
	// Returns the dummy text for a new page.
	std::ostringstream ret;

	ret << "Author: " << author << NEWLINE;
	ret << "Title: " << title << NEWLINE;
	ret << "Slug: " << slug << NEWLINE;
	ret << "Date: " << timeNow("%Y-%m-%d %H:%M:%S") << NEWLINE;
	ret << "Tags: " << NEWLINE;
	ret << NEWLINE;
	ret << HEADER_DIVIDER << NEWLINE;
	ret << NEWLINE;

	return ret.str();
}

std::string ContentsGenerator::createDummyArticle()
{
	// Returns the dummy text for a new page.
	std::ostringstream ret;

	ret << "Author: " << author << NEWLINE;
	ret << "Title: " << title << NEWLINE;
	ret << "Slug: " << slug << NEWLINE;
	ret << "Date: " << timeNow("%Y-%m-%d %H:%M:%S") << NEWLINE;
	ret << "Tags: " << NEWLINE;
	ret << "Categories: " << NEWLINE;
	ret << NEWLINE;
	ret << HEADER_DIVIDER << NEWLINE;
	ret << NEWLINE;

	return ret.str();
}
