/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of comments [header].
 */

#pragma once

#include <sstream>
#include <string>

class CommentIntegration
{
public:
	CommentIntegration(std::string commenttype, std::string commentdata);
	std::string addHeader();
	std::string addComments();

private:
	std::string commenttype;
	std::string commentdata;
};
