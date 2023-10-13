#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "Utils.hpp"

#define COMMENT_DELIMITER "#"
#define ERROR_PAGE "error_page"
#define ROOT "root"
#define INDEX "index"
#define CGI_PASS "cgi_pass"
#define AUTOINDEX "autoindex"
#define ON "on"
#define OFF "off"
#define METHODS "methods"
#define RETURN "return"
#define CHAR_SPACE ' '

class ConfigLocation
{

private:
	std::map<int, std::string> _errorPages;
	std::string _root;
	std::string _index;
	std::string _cgi_pass;
	bool _autoindex;
	std::vector<std::string> _methods;
	std::string _return;

	ConfigLocation(void);

	void parseErrorPage(std::pair<std::string, std::string> &pair);
	void parseAutoIndex(std::pair<std::string, std::string> &pair);
	void parseMethods(std::pair<std::string, std::string> &pair);
	void parseReturn(std::pair<std::string, std::string> &pair);
	void dispatch(std::pair<std::string, std::string> &pair);
	void parseLocation(std::string &content);

public:
	ConfigLocation(std::string &content);
	ConfigLocation(ConfigLocation const &src);
	~ConfigLocation(void);

	ConfigLocation &operator=(ConfigLocation const &rhs);

	void setRoot(std::string &root);
	void setIndex(std::string &index);
	void setCgiPass(std::string &cgi_pass);

	std::map<int, std::string> getErrorPages(void) const;
	std::string getRoot(void) const;
	std::string getIndex(void) const;
	std::string getCgiPass(void) const;
	bool getAutoIndex(void) const;
	std::vector<std::string> getMethods(void) const;
	std::string getReturn(void) const;
};

#endif