#include "html.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

namespace html
{
// ----------------------------------------
// our exceptions
// ----------------------------------------
exception::exception(const std::string &m) : std::runtime_error(m.c_str()) {}
tokenize_error::tokenize_error(const std::string &m) : exception(m.c_str()) {}
token_stream_error::token_stream_error(const std::string &m)
	: exception(m.c_str())
{
}
data_map_error::data_map_error(const std::string &m) : exception(m.c_str()) {}
file_not_found_error::file_not_found_error(const std::string &m)
	: exception(m.c_str())
{
}
recursive_include_error::recursive_include_error(const std::string &m)
	: exception(m.c_str())
{
}

// ----------------------------------------
// an item in the map:
// ----------------------------------------
item::item(std::string name) : m_name(name) {}
item::item(const item &rhs) : m_name(rhs.m_name), m_data(rhs.m_data) {}
item::item(item &&rhs) : m_name(rhs.m_name), m_data(std::move(rhs.m_data)) {}
item &item::operator=(const item &rhs)
{
	m_name = rhs.m_name;
	m_data = rhs.m_data;
	return *this;
}
item &item::operator==(item &&rhs)
{
	m_name = std::move(rhs.m_name);
	m_data = std::move(rhs.m_data);
	return *this;
}

item::iterator item::begin()
{
	if (is_array())
	{
		return get_array().begin();
	}
	return item::iterator();
}
item::iterator item::end()
{
	if (is_array())
	{
		return get_array().end();
	}
	return item::iterator();
}

item::const_iterator item::cbegin() const
{
	if (is_array())
	{
		return get_array().cbegin();
	}
	return item::const_iterator();
}
item::const_iterator item::cend() const
{
	if (is_array())
	{
		return get_array().cend();
	}
	return item::const_iterator();
}

bool item::is_array() const { return index() == 2; }
bool item::is_string() const { return index() == 0; }
bool item::is_bool() const { return index() == 1; }
bool item::is_null() const { return !is_array() && !is_string() && !is_bool(); }

std::string &item::get_string()
{
	if (!is_string())
	{
		m_data = std::string();
	}
#if USE_VARIANT_API == 1
	return std::get<0>(m_data);
#endif
#if USE_VARIANT_API == 2
	return *boost::get<std::string>(&m_data);
#endif
}
bool &item::get_bool()
{
	if (!is_bool())
	{
		m_data = false;
	}
#if USE_VARIANT_API == 1
	return std::get<1>(m_data);
#endif
#if USE_VARIANT_API == 2
	return *boost::get<bool>(&m_data);
#endif
}
std::vector<item> &item::get_array()
{
	if (!is_array())
	{
		m_data = std::vector<item>();
	}
#if USE_VARIANT_API == 1
	return std::get<2>(m_data);
#endif
#if USE_VARIANT_API == 2

	return *boost::get<std::vector<item>>(&m_data);
#endif
}

const std::string &item::get_string() const
{
#if USE_VARIANT_API == 1
	return std::get<0>(m_data);
#endif
#if USE_VARIANT_API == 2
	return *boost::get<std::string>(&m_data);
#endif
}
const bool &item::get_bool() const
{
#if USE_VARIANT_API == 1
	return std::get<1>(m_data);
#endif
#if USE_VARIANT_API == 2
	return *boost::get<bool>(&m_data);
#endif
}
const std::vector<item> &item::get_array() const
{
#if USE_VARIANT_API == 1
	return std::get<2>(m_data);
#endif
#if USE_VARIANT_API == 2
	return *boost::get<std::vector<item>>(&m_data);
#endif
}

std::string item::name() const { return m_name; }

// used in the html parser
item &item::child(const std::string &key)
{
	auto &arr = get_array();
	auto pos = std::find_if(std::begin(arr), std::end(arr),
							[key](const item &i) { return i.name() == key; });
	if (pos == std::end(arr))
	{
		arr.emplace_back(item(key));
		return *arr.rbegin();
	}
	return *pos;
}

const item &item::child(const std::string &key) const
{
	const interal_t &arr = get_array();
	auto pos = std::find_if(std::begin(arr), std::end(arr),
							[key](const item &i) { return i.name() == key; });
	if (pos == std::end(arr))
	{
		throw data_map_error("child " + key + " not found");
	}

	return *pos;
}

bool item::childexists(const std::string &key) const
{
	const interal_t &arr = get_array();
	auto pos = std::find_if(std::begin(arr), std::end(arr),
							[key](const item &i) { return i.name() == key; });
	return (pos != std::end(arr));
}

std::string item::dump(int ident) const
{
	struct sub
	{
		sub(std::ostream &s) : oss(s) {}

		void operator()(const html::item &m, const int ident, const int lvl)
		{
			indent(lvl * ident);
			oss << m.name() << "=";
			if (m.is_null())
			{
				oss << "{null}";
			}
			else if (m.is_string())
			{
				std::string msg = m.get_string();
				msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
				msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
				oss << "{\"" << msg << "\"}";
			}
			else if (m.is_bool())
			{
				oss << std::boolalpha << m.get_bool();
			}
			else if (m.is_array())
			{
				oss << std::endl;
				indent(lvl * ident);
				oss << "{" << std::endl;
				sub s(oss);
				const auto &sub_items = m.get_array();
				for (auto &item : sub_items)
				{
					s(item, ident, lvl + 1);
				}
				indent(lvl * ident);
				oss << "}";
			}
			oss << std::endl;
		}
		void indent(int indent)
		{
			for (int i = 0; i < indent; ++i)
			{
				oss << " ";
			}
		}

		std::ostream &oss;
	};

	std::ostringstream oss;

	sub s(oss);
	s(*this, ident, 0);

	return oss.str();
}

int item::index() const
{
#if USE_VARIANT_API == 1
	size_t pos = m_data.index();
	if (pos != std::variant_npos)
	{
		return static_cast<int>(m_data.index());
	}
	else
	{
		return -1;
	}
#endif
#if USE_VARIANT_API == 2
	if (const std::string *p = boost::get<std::string>(&m_data))
	{
		return 0;
	}
	else if (const bool *p = boost::get<bool>(&m_data))
	{
		return 1;
	}
	else if (const std::vector<item> *p =
				 boost::get<std::vector<item>>(&m_data))
	{
		return 2;
	}
	else
	{
		return -1;
	}
#endif
}

// ----------------------------------------
// the tokenization of the TMPL file
// ----------------------------------------
namespace token
{
enum class type_t
{
	TEXT,
	VAR,
	FOR,
	ENDFOR,
	IF,
	ELSE,
	ENDIF,
	INCLUDE,
	EXISTS,
	ENDEXISTS,
	END
};

struct tok
{
	tok() { clear(); }

	type_t type;
	fs::path filename;
	std::string name;
	size_t line;

	void clear()
	{
		type = type_t::END;
		name = "";
		line = 0;
	}
};

// the parsed stream of tokens
using stream = std::vector<tok>;

void remove_spaces(std::string &msg)
{
	msg.erase(std::remove(msg.begin(), msg.end(), ' '), msg.end());
}
void tokenize(const fs::path file, std::istream &is, size_t &line,
			  token::tok &next_token)
{
	// clang-format off
			/*
			[TMPL_VAR xyz] : token::VAR
			[TMPL_FOR xyz] : token::FOR
			[/TMPL_FOR]    : token::ENDFOR
			[TMPL_IF xyz]  : token::IF
			[TMPL_ELSE]    : token::ELSE
			[/TMPL_IF]     : token::ENDIF
			[TMPL_INCLUDE xyz]  : token::INCLUDE
			[TMPL_EXISTS xyz]   : token::EXISTS
			[/TMPL_EXISTS]      : token::ENDEXISTS

			End of stream  : token::END
			*/
	// clang-format on

	next_token.clear();
	next_token.filename = file;
	char c = 0;

	is.get(c);
	if (!is)
	{
		next_token.type = token::type_t::END;
		next_token.line = line;
		return;
	}

	if (c == '[')
	{
		// start of an token : grab until ']'
		std::string tok_data;
		tok_data += c;
		while (is.get(c) && c != ']')
		{
			if (c == '\n')
				++line;

			tok_data += c;
		}
		if (c == '\n')
			++line;
		tok_data += c;

		// now check the value
		if (tok_data.find("[TMPL_VAR ") != std::string::npos)
		{
			next_token.type = type_t::VAR;
			next_token.line = line;

			// extract the var name
			auto start = tok_data.find(" ");
			std::copy(tok_data.begin() + start + 1,
					  tok_data.begin() + tok_data.size() - 1,
					  std::back_inserter(next_token.name));

			remove_spaces(next_token.name);
		}
		else if (tok_data.find("[TMPL_FOR ") != std::string::npos)
		{
			next_token.type = type_t::FOR;
			next_token.line = line;

			// extract the var name
			auto start = tok_data.find(" ");
			std::copy(tok_data.begin() + start + 1,
					  tok_data.begin() + tok_data.size() - 1,
					  std::back_inserter(next_token.name));

			remove_spaces(next_token.name);
		}
		else if (tok_data.find("[/TMPL_FOR]") != std::string::npos)
		{
			next_token.type = type_t::ENDFOR;
			next_token.line = line;
		}
		else if (tok_data.find("[TMPL_IF ") != std::string::npos)
		{
			next_token.type = type_t::IF;
			next_token.line = line;

			// extract the var name and the value
			auto start = tok_data.find(" ");

			// check the positions
			if (start == std::string::npos)
				throw tokenize_error("TMPL_IF error at line " +
									 std::to_string(line));

			std::copy(tok_data.begin() + start + 1,
					  tok_data.begin() + tok_data.size() - 1,
					  std::back_inserter(next_token.name));

			remove_spaces(next_token.name);
		}
		else if (tok_data.find("[TMPL_ELSE]") != std::string::npos)
		{
			next_token.type = type_t::ELSE;
			next_token.line = line;
		}
		else if (tok_data.find("[/TMPL_IF]") != std::string::npos)
		{
			next_token.type = type_t::ENDIF;
			next_token.line = line;
		}
		else if (tok_data.find("[TMPL_INCLUDE ") != std::string::npos)
		{
			next_token.type = type_t::INCLUDE;
			next_token.line = line;

			// extract the include name
			auto start = tok_data.find(" ");
			std::copy(tok_data.begin() + start + 1,
					  tok_data.begin() + tok_data.size() - 1,
					  std::back_inserter(next_token.name));

			remove_spaces(next_token.name);
		}
		else if (tok_data.find("[TMPL_EXISTS ") != std::string::npos)
		{
			next_token.type = type_t::EXISTS;
			next_token.line = line;

			// extract the exists name
			auto start = tok_data.find(" ");
			std::copy(tok_data.begin() + start + 1,
					  tok_data.begin() + tok_data.size() - 1,
					  std::back_inserter(next_token.name));

			remove_spaces(next_token.name);
		}
		else if (tok_data.find("[/TMPL_EXISTS]") != std::string::npos)
		{
			next_token.type = type_t::ENDEXISTS;
			next_token.line = line;
		}
		else
		{
			// unknown token
			std::string msg{"Unknown token at line " + std::to_string(line)};
			throw tokenize_error(msg);
		}
	}
	else
	{
		// regular text : grab until [
		std::string tok_data;
		tok_data += c;
		if (c == '\n')
			++line;

		while (is.get(c) && c != '[')
		{
			if (c == '\n')
				++line;

			tok_data += c;
		}
		// putback the character which ended the grab ('[')
		if (is)
			is.putback(c);

		next_token.type = type_t::TEXT;
		next_token.line = line;
		next_token.name = tok_data;
	}
}

// these two functions call each other
void generate_token_stream(fs::path file, std::istream &is,
						   token::stream &token_stream);
void generate_included_token_stream(fs::path file, token::stream &tokens)
{
	// check tokens for includes
	bool found = true;
	while (found)
	{
		auto pos = std::find_if(std::begin(tokens), std::end(tokens),
								[](const html::token::tok &t) {
									return t.type == type_t::INCLUDE;
								});
		found = pos != std::end(tokens);
		if (found)
		{
			// prevent recursive include
			if (pos->name == file.filename().string())
			{
				throw recursive_include_error(pos->name);
			}

			// open file
			fs::path included_file = file.parent_path();
			included_file /= pos->name;
			std::ifstream ifs(included_file.string());
			if (!ifs.is_open())
			{
				throw file_not_found_error(pos->name);
			}

			// generate the token stream for this
			token::stream included_tokens;
			generate_token_stream(pos->filename, ifs, included_tokens);

			// now include the included stream on the pos iterator
			// but without the end tag
			if (included_tokens.size() &&
				included_tokens.rbegin()->type == type_t::END)
			{
				included_tokens.pop_back();
			}
			token::stream before{std::begin(tokens), pos};
			token::stream after{pos + 1, std::end(tokens)};

			token::stream total{before};
			std::copy(std::begin(included_tokens), std::end(included_tokens),
					  std::back_inserter(total));
			std::copy(std::begin(after), std::end(after),
					  std::back_inserter(total));

			tokens = std::move(total);
		}
	}
}

void generate_token_stream(fs::path file, std::istream &is,
						   token::stream &token_stream)
{
	size_t line = 1;
	token::tok next_token;

	// generate a token stream
	token_stream.clear();
	while (is)
	{
		// grab the next tonen
		tokenize(file, is, line, next_token);
		token_stream.push_back(next_token);
	}

	// add END
	next_token.clear();
	next_token.line = line;
	token_stream.push_back(next_token);

	generate_included_token_stream(file, token_stream);
}

// work on the tokens
std::string to_path_string(const std::vector<token::tok> &s)
{
	if (s.empty())
		return "";

	std::string ret;
	for (const auto &ct : s)
	{
		ret += ct.name + ".";
	}

	return ret;
}
std::string to_string(const token::tok &ct)
{
	// clang-format off
			std::string ret = "type_t::";
			switch (ct.type)
			{
			case type_t::TEXT:      ret += "TEXT    : "; break;
			case type_t::VAR:       ret += "VAR     : "; break;
			case type_t::FOR:       ret += "FOR     : "; break;
			case type_t::ENDFOR:    ret += "ENDFOR  : "; break;
			case type_t::IF:        ret += "IF      : "; break;
			case type_t::ELSE:      ret += "ELSE    : "; break;
			case type_t::ENDIF:     ret += "ENDIF   : "; break;
			case type_t::INCLUDE:   ret += "INCLUDE : "; break;
			case type_t::EXISTS:    ret += "EXISTS  : "; break;
			case type_t::ENDEXISTS: ret += "ENDEXISTS  : "; break;
			case type_t::END:       ret += "END     : "; break;
			}
	// clang-format on

	ret += "Line: " + std::to_string(ct.line) + " ";
	ret += "Name: " + ct.name;

	return ret;
}

void parse_token_stream(std::string &ret, token::stream::const_iterator begin,
						token::stream::const_iterator end,
						const html::item &data)
{
	// for IF ELSE ENDIF
	auto else_pos = begin;
	auto endif_pos = begin;

	// for FOR loop
	auto for_pos = begin;
	auto endfor_pos = begin;

	// for EXISTS
	auto exists_pos = begin;
	auto endexists_pos = begin;

	int sublevel = 0;

	for (auto pos = begin; pos != end; ++pos)
	{
		const token::tok &ct = *pos;

		switch (ct.type)
		{
			case type_t::TEXT:
				ret += ct.name;
				break;

			case type_t::VAR:
				if (data.childexists(ct.name))
				{
					ret += data.child(ct.name).get_string();
				}
				break;

			case type_t::FOR:
				// find matching endfor
				for_pos = endfor_pos = pos;
				sublevel = 0;

				for (auto tmp_pos = pos; tmp_pos != end; ++tmp_pos)
				{
					// sub for
					if (tmp_pos->type == type_t::FOR)
						++sublevel;
					if (tmp_pos->type == type_t::ENDFOR)
						--sublevel;

					if (sublevel == 0 && tmp_pos->type == type_t::ENDFOR)
					{
						endfor_pos = tmp_pos;
						break;
					}
				}

				// now repeat the content of the interval [for_pos, endfor_pos]
				if (data.childexists(ct.name))
				{
					auto children = data.child(ct.name).get_array();
					for (const auto &child : children)
					{
						parse_token_stream(ret, for_pos + 1, endfor_pos, child);
					}
				}
				pos = endfor_pos;

				break;

			case type_t::ENDFOR:
				throw token_stream_error("ENDFOR without matching FOR: " +
										 to_string(ct));
				break;

			case type_t::IF:
				// adjust else_pos and endif_pos
				else_pos = endif_pos = pos;
				sublevel = 0;

				for (auto tmp_pos = pos; tmp_pos != end; ++tmp_pos)
				{
					// sub ifs
					if (tmp_pos->type == type_t::IF)
						++sublevel;
					if (tmp_pos->type == type_t::ENDIF)
						--sublevel;

					// ELSE is "in" the if endif, so sublevel == 1
					if (sublevel == 1 && tmp_pos->type == type_t::ELSE)
						else_pos = tmp_pos;

					if (sublevel == 0 && tmp_pos->type == type_t::ENDIF)
					{
						endif_pos = tmp_pos;
						if (else_pos == pos) // no else found?
						{
							else_pos = endif_pos;
						}
						break;
					}
				}

				// evaluate it
				if (data.childexists(ct.name))
				{
					if (data.child(ct.name).get_bool())
					{
						// parse the iterator sequence [IF / (ELSE|ENDIF)]
						parse_token_stream(ret, pos + 1, else_pos, data);
					}
					else
					{
						// parse the iterator sequence [ELSE / ENDIF]
						parse_token_stream(ret, else_pos + 1, endif_pos, data);
					}
				}
				pos = endif_pos;

				break;

			case type_t::ELSE:
				throw token_stream_error("ELSE without matching IF: " +
										 to_string(ct));
				break;

			case type_t::ENDIF:
				throw token_stream_error("ENDIF without matching IF: " +
										 to_string(ct));
				break;

			case type_t::EXISTS:
				// adjust else_pos and endif_pos
				exists_pos = endexists_pos = pos;
				sublevel = 0;

				for (auto tmp_pos = pos; tmp_pos != end; ++tmp_pos)
				{
					// sub EXISTS
					if (tmp_pos->type == type_t::EXISTS)
						++sublevel;
					if (tmp_pos->type == type_t::ENDEXISTS)
						--sublevel;

					if (sublevel == 0 && tmp_pos->type == type_t::ENDEXISTS)
					{
						endexists_pos = tmp_pos;
						break;
					}
				}

				// evaluate it
				if (data.childexists(ct.name))
				{
					// parse the iterator sequence [EXISTS / ENDEXISTS]
					parse_token_stream(ret, pos + 1, endexists_pos, data);
				}
				pos = endexists_pos;

				break;

			case type_t::ENDEXISTS:
				throw token_stream_error("ENDEXISTS without matching EXISTS: " +
										 to_string(ct));
				break;

			case type_t::INCLUDE:
				throw token_stream_error("INCLUDE: still in token_stream" +
										 to_string(ct));
				break;

			case type_t::END:
				return;
				break;
		}
	}
}
} // namespace token

// ----------------------------------------
// main function
// parse in the file and get out the generated document
// ----------------------------------------
std::string parse(fs::path file, std::istream &is, const item &data)
{
	using namespace token;

	stream tokens;
	generate_token_stream(file, is, tokens);

	std::string ret;
	parse_token_stream(ret, tokens.cbegin(), tokens.cend(), data);
	return ret;
}
std::string parse(fs::path file, const item &data)
{
	std::ifstream ifs(file.string());
	if (!ifs.is_open())
	{
		throw file_not_found_error(file.string());
	}

	return parse(file, ifs, data);
}
} // namespace html
