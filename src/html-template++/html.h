#ifndef __html_template_h__
#define __html_template_h__

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <istream>
#include <stdexcept>
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
	class map
	{
	public:
		using interal_t = std::map<std::string, std::tuple<std::string, std::vector<map>>>;
		using iterator = interal_t::iterator;
		using const_iterator = interal_t::const_iterator;

		bool has_children(const std::string& v) const;
		std::string& operator[](const std::string& v);
		const std::string&  operator[](const std::string& v) const;

		iterator begin();
		iterator end();

		const_iterator cbegin() const;
		const_iterator cend() const;

		std::vector<map>& get_children(const std::string& v);
		const std::vector<map>& get_children(const std::string& v) const;

	private:
		interal_t m_data;
	};

	// ----------------------------------------
	// main function
	// parse in the file and get out the generated document
	// ----------------------------------------
	std::string parse(fs::path file, const map& data);

} // ns html


#endif // __html_template_h__
