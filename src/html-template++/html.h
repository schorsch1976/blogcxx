#ifndef __html_template_h__
#define __html_template_h__

#include <string>
#include <vector>
#include <istream>
#include <stdexcept>

#include "Shared/variant.h"
#include "Shared/filesystem.h"

namespace html
{
	// ----------------------------------------
	// our exceptions
	// ----------------------------------------
	struct exception : std::runtime_error
	{
		explicit exception(const std::string& m);
	};
	struct tokenize_error : exception
	{
		explicit tokenize_error(const std::string& m);
	};
	struct token_stream_error : exception
	{
		explicit token_stream_error(const std::string& m);
	};
	struct data_map_error : exception
	{
		explicit data_map_error(const std::string& m);
	};
	struct file_not_found_error : exception
	{
		explicit file_not_found_error(const std::string& m);
	};
	struct recursive_include_error : exception
	{
		explicit recursive_include_error(const std::string& m);
	};
	// ----------------------------------------
	// the data we provide for the HTML Template
	// subvalues are like "key1.table1.b"
	// ----------------------------------------
	class item
	{
	public:
		using interal_t = std::vector<item>;
		using iterator = interal_t::iterator;
		using const_iterator = interal_t::const_iterator;

		item(std::string name);
		item(const item& rhs);
		item(item&& rhs);
		item& operator=(const item& rhs);
		item& operator==(item&& rhs);

		iterator begin();
		iterator end();

		const_iterator cbegin() const;
		const_iterator cend() const;

		bool is_array() const;
		bool is_string() const;
		bool is_bool() const;
		bool is_null() const;

		std::string& get_string();
		bool& get_bool();
		std::vector<item>& get_array();

		const std::string& get_string() const;
		const bool& get_bool() const;
		const std::vector<item>& get_array() const;

		std::string name() const;

		// used in the html parser
		item& child(const std::string& key);
		const item& child(const std::string& key) const;
		bool childexists(const std::string& key) const;

		std::string dump(int ident = 4) const;
	private:
		int index() const;
		std::string m_name;
		var::variant<std::string, bool, std::vector<item>> m_data;
	};

	// ----------------------------------------
	// main function
	// parse in the file and get out the generated document
	// ----------------------------------------
	std::string parse(fs::path file, const item& data);

} // ns html


#endif // __html_template_h__
