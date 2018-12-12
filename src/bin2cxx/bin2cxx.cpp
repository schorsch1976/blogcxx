/*
 * blogcxx :: https://www.blogcxx.de
 * Read an input file and write it as a cxx/h file
 */

#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
namespace po = boost::program_options;

#include "Shared/Debug.h"
#include "Shared/PrintVersion.h"
#include "Shared/constants.h"
#include "Shared/filesystem.h"

#include <fstream>
#include <iomanip>
#include <iterator>

std::string to_array_name(fs::path filename)
{
	std::string name = "bin2cxx_" + filename.stem().string();
	return name;
}
void CppMode(const std::vector<unsigned char> &buffer, fs::path filename,
			 std::ostream &os)
{
	os << "#include <array>\n\n"
	   << "std::array<unsigned char, " << buffer.size() << "> "
	   << to_array_name(filename) << " = {{\n\t";

	for (size_t i = 0; i < buffer.size(); ++i)
	{
		os << "0x" << std::setfill('0') << std::setw(2) << std::hex
		   << (unsigned int)(buffer[i]);
		if (i < buffer.size() - 1)
		{
			os << ", ";
		}
		if ((i + 1) % 32 == 0)
		{
			os << "\n\t";
		}
	}
	os << " }};\n\n";
}

void HppMode(const std::vector<unsigned char> &buffer, fs::path filename,
			 std::ostream &os)
{
	os << "#pragma once\n\n"
	   << "#include <array>\n\n"
	   << "extern std::array<unsigned char, " << buffer.size() << "> "
	   << to_array_name(filename) << ";\n";
}
void CMode(const std::vector<unsigned char> &buffer, fs::path filename,
		   std::ostream &os)
{
	os << "const char[" << buffer.size() << "] " << to_array_name(filename)
	   << " = {\n\t";

	for (size_t i = 0; i < buffer.size(); ++i)
	{
		os << "0x" << std::setfill('0') << std::setw(2) << std::hex
		   << (unsigned int)(buffer[i]);
		if (i < buffer.size() - 1)
		{
			os << ", ";
		}
		if ((i + 1) % 32 == 0)
		{
			os << "\n\t";
		}
	}
	os << " };\n\n";
}

std::string MakeClean(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), '('), str.end());
	str.erase(std::remove(str.begin(), str.end(), ')'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\''), str.end());
	return str;
}

int main(int argc, char **argv)
{
	Debug::Status debug_status;

	try
	{
		po::options_description cfg_file("Configuration file");

		int mode = 0;
		std::string inf;
		std::string outf;
		po::options_description options("Options");
		options.add_options()
			// clang-format off
			("help,h",		"Prints this help")
			("version,v",	"Prints version information.")

			("in",			po::value<std::string>(&inf),	"The input file to convert")
			("out",			po::value<std::string>(&outf),	"The output file")

			("mode",		po::value<int>(&mode)->default_value(0),
							"conversion mode:\n0=cpp\n1=h,2=c");
		// clang-format on

		// now get the value of the command line to get the config file
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv)
					  .options(options)
					  .allow_unregistered()
					  .run(),
				  vm);
		po::notify(vm);

		// remove () from the filenames (when called from cmake)
		fs::path infile{MakeClean(inf)};
		fs::path outfile{MakeClean(outf)};

		// check for help and version
		if (vm.count("help"))
		{
			std::cout << std::boolalpha << options << std::endl;
			return EXIT_FAILURE;
		}

		else if (vm.count("version"))
		{
			PrintVersion("bin2cxx");
			return EXIT_FAILURE;
		}

		// check
		if (!fs::exists(infile))
		{
			THROW_FATAL("input file '%1% does not exists.", infile.string());
		}
		if (!fs::is_regular_file(infile))
		{
			THROW_FATAL("input file '%1% is not a regular file.",
						infile.string());
		}

		// open infile
		std::vector<unsigned char> buffer;
		std::ifstream ifs(infile.string(), std::ios::binary);
		if (!ifs.is_open())
		{
			THROW_FATAL("input file '%1% could not be openend",
						infile.string());
		}

		// open outfile
		std::ofstream ofs(outfile.string(), std::ios::binary);
		if (!ofs.is_open())
		{
			THROW_FATAL("Could not open outfile '%1%", outfile.string());
		}

		// read it
		ifs.unsetf(std::ios::skipws);
		std::copy(std::istream_iterator<unsigned char>(ifs),
				  std::istream_iterator<unsigned char>(),
				  std::back_inserter(buffer));

		// convert
		PrintVersion("bin2cxx");
		switch (mode)
		{
			case 0:
				CppMode(buffer, infile, ofs);
				PRINT("CppMode: Created '%1% with %2% bytes from %3%.",
					  outfile.string(), buffer.size(), infile.string());
				return EXIT_SUCCESS;
			case 1:
				HppMode(buffer, infile, ofs);
				PRINT("HppMode: Created '%1% with %2% bytes from %3%.",
					  outfile.string(), buffer.size(), infile.string());
				return EXIT_SUCCESS;
			case 2:
				CMode(buffer, infile, ofs);
				PRINT("CMode: Created '%1% with %2% bytes from %3%.",
					  outfile.string(), buffer.size(), infile.string());
				return EXIT_SUCCESS;
			default:
				THROW_FATAL("undefined mode %1%", mode);
		}
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
