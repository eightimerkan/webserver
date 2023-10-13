#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "Utils.hpp"
#include "ConfigLocation.hpp"

#define LOCATION "location"
#define END_LOCATION_BLOCK "}\n"
#define COMMENT_DELIMITER "#"
#define LISTEN "listen"
#define HOST "host"
#define SERVER_NAME "server_name"
#define CGI_PASS "cgi_pass"
#define CLIENT_SIZE "client_max_body_size"
#define ERROR_PAGE "error_page"
#define ROOT "root"
#define INDEX "index"
#define AUTOINDEX "autoindex"
#define ON "on"
#define OFF "off"
#define METHODS "methods"
#define RETURN "return"
#define CHAR_SPACE ' '

class ConfigLocation;

class ConfigServer
{

private:
	std::vector<std::string> _serverBlocks;
	std::map<std::string, ConfigLocation *> _mapLocations;
	std::vector<int> _ports;
	std::string _host;
	std::vector<std::string> _serverNames;
	unsigned int _client_max_body_size;
	std::map<int, std::string> _errorPages;
	std::string _root;
	std::string _index;
	std::string _cgi_pass;
	bool _autoindex;
	std::vector<std::string> _methods;
	std::map<int, std::string> _return;

	ConfigServer(void);

	void createServerBlocks(std::string &content);
	void parseListen(std::pair<std::string, std::string> &pair);
	void parseServerNames(std::pair<std::string, std::string> &pair);
	void parseClientSize(std::pair<std::string, std::string> &pair);
	void parseErrorPage(std::pair<std::string, std::string> &pair);
	void parseAutoIndex(std::pair<std::string, std::string> &pair);
	void parseMethods(std::pair<std::string, std::string> &pair);
	void parseReturn(std::pair<std::string, std::string> &pair);
	void dispatch(std::pair<std::string, std::string> &pair);
	std::string extractLocPath(std::string &content) const;
	void parseServer();

public:
	ConfigServer(std::string &content);
	ConfigServer(ConfigServer const &src);
	~ConfigServer(void);

	ConfigServer &operator=(ConfigServer const &rhs);

	void setHost(std::string &host);
	void setRoot(std::string &root);
	void setIndex(std::string &index);
	void setCgiPass(std::string &cgi_pass);

	std::vector<std::string> getServerBlocks(void) const;
	std::map<std::string, ConfigLocation *> getLocation(void) const;
	std::vector<int> getPorts(void) const;
	std::string getHost(void) const;
	std::vector<std::string> getServerNames(void) const;
	unsigned int getClientMaxBodySize(void) const;
	std::map<int, std::string> getErrorPages(void) const;
	std::string getRoot(void) const;
	std::string getIndex(void) const;
	bool getAutoIndex(void) const;
	std::vector<std::string> getMethods(void) const;
	std::map<int, std::string> getReturn(void) const;
};

#endif