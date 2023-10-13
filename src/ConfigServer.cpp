#include "inc/ConfigServer.hpp"

ConfigServer::ConfigServer(std::string &content)
{
	createServerBlocks(content);
	parseServer();
}

ConfigServer::ConfigServer(ConfigServer const &src)
{
	*this = src;
}

ConfigServer::~ConfigServer()
{
	std::map<std::string, ConfigLocation *>::iterator it = _mapLocations.begin();
	for (; it != _mapLocations.end(); ++it)
	{
		delete it->second;
	}
	this->_mapLocations.clear();
}

void ConfigServer::createServerBlocks(std::string &content)
{

	size_t posb = 0;
	size_t pose = content.find(LOCATION, posb);

	this->_serverBlocks.push_back(content.substr(posb, pose - posb - 1));
	while (posb != std::string::npos)
	{
		posb = content.find(LOCATION, posb);
		pose = content.find(END_LOCATION_BLOCK, posb);
		if (posb < content.length() && pose << content.length())
		{
			this->_serverBlocks.push_back(content.substr(posb, pose + 2 - posb));
			posb = pose + 1;
			pose = posb;
		}
		else
			break;
	}
}

void ConfigServer::parseListen(std::pair<std::string, std::string> &pair)
{

	int port;

	for (size_t i = 0; i < pair.second.length(); i++)
	{
		if (!isdigit(pair.second[i]))
		{
			std::cout << "Error: problem with port format.\n";
			exit(EXIT_FAILURE);
		}
	}
	port = stoi(pair.second);
	if (port > 65535 || port < 1)
	{
		std::cout << "Error: problem with port format.\n";
		exit(EXIT_FAILURE);
	}
	this->_ports.push_back(port);
}

void ConfigServer::parseServerNames(std::pair<std::string, std::string> &pair)
{

	size_t posb = 0;
	size_t pos = (pair.second).find(' ');

	while (pos != std::string::npos)
	{
		this->_serverNames.push_back((pair.second).substr(posb, pos - posb));
		posb = pos + 1;
		pos = (pair.second).find(' ', posb);
	}
	this->_serverNames.push_back((pair.second).substr(posb, (pair.second).length() - posb));
}

void ConfigServer::parseClientSize(std::pair<std::string, std::string> &pair)
{
	this->_client_max_body_size = stoi(pair.second);
}

void ConfigServer::parseErrorPage(std::pair<std::string, std::string> &pair)
{

	size_t posb = 0;
	size_t pos = (pair.second).find(' ');

	int code = stoi((pair.second).substr(posb, pos - posb));
	std::string page = (pair.second).substr(pos + 1, (pair.second).length() - pos);

	this->_errorPages.insert(std::pair<int, std::string>(code, page));
}

void ConfigServer::parseAutoIndex(std::pair<std::string, std::string> &pair)
{

	if ((pair.second) == ON)
		this->_autoindex = true;
	else if ((pair.second) == OFF)
		this->_autoindex = false;
	else
	{
		std::cout << "Error: wrong autoindex format\n";
		exit(EXIT_FAILURE);
	}
}

void ConfigServer::parseMethods(std::pair<std::string, std::string> &pair)
{

	size_t posb = 0;
	size_t pos = (pair.second).find(CHAR_SPACE);

	while (pos != std::string::npos)
	{
		this->_methods.push_back((pair.second).substr(posb, pos - posb));
		posb = pos + 1;
		pos = (pair.second).find(CHAR_SPACE, posb);
	}
	this->_methods.push_back((pair.second).substr(posb, (pair.second).length() - posb));
}

void ConfigServer::parseReturn(std::pair<std::string, std::string> &pair)
{

	size_t posb = 0;
	size_t pos = (pair.second).find(CHAR_SPACE);

	// check format code
	int code = stoi((pair.second).substr(posb, pos - posb));
	std::string page = (pair.second).substr(pos + 1, (pair.second).length() - pos);

	this->_return.insert(std::pair<int, std::string>(code, page));
}

void ConfigServer::dispatch(std::pair<std::string, std::string> &pair)
{

	if (pair.first == HOST)
		setHost(pair.second);
	else if (pair.first == ROOT)
		setRoot(pair.second);
	else if (pair.first == INDEX)
		setIndex(pair.second);
	else if (pair.first == LISTEN)
		parseListen(pair);
	else if (pair.first == SERVER_NAME)
		parseServerNames(pair);
	else if (pair.first == CGI_PASS)
		setCgiPass(pair.second);
	else if (pair.first == CLIENT_SIZE)
		parseClientSize(pair);
	else if (pair.first == ERROR_PAGE)
		parseErrorPage(pair);
	else if (pair.first == AUTOINDEX)
		parseAutoIndex(pair);
	else if (pair.first == METHODS)
		parseMethods(pair);
	else if (pair.first == RETURN)
		parseReturn(pair);
	else
	{
		std::cout << "Error: unknown directive in config file\n";
		exit(EXIT_FAILURE);
	}
}

std::string ConfigServer::extractLocPath(std::string &content) const
{

	size_t posa = content.find(CHAR_SPACE);
	size_t posb = content.find(CHAR_SPACE, posa + 1);

	return (content.substr(posa + 1, posb - posa - 1));
}

void ConfigServer::parseServer()
{

	std::list<std::pair<std::string, std::string> > list;
	std::string locPath;

	list = utils::parseBlock(this->_serverBlocks[0]);
	for (std::list<std::pair<std::string, std::string> >::iterator it = list.begin(); it != list.end(); it++)
	{
		if (((*it).first).empty())
			continue;
		dispatch(*it);
	}
	for (unsigned int i = 1; i < this->_serverBlocks.size(); i++)
	{
		locPath = extractLocPath(this->_serverBlocks[i]);
		this->_mapLocations[locPath] = new ConfigLocation(this->_serverBlocks[i]);
	}
}

ConfigServer &ConfigServer::operator=(ConfigServer const &rhs)
{

	if (this != &rhs)
	{
		this->_serverBlocks = rhs._serverBlocks;
		this->_ports = rhs._ports;
		this->_host = rhs._host;
		this->_serverNames = rhs._serverNames;
		this->_client_max_body_size = rhs._client_max_body_size;
		this->_errorPages = rhs._errorPages;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_autoindex = rhs._autoindex;
		this->_methods = rhs._methods;
		this->_return = rhs._return;
	}
	return (*this);
}

void ConfigServer::setHost(std::string &host)
{
	this->_host = host;
}

void ConfigServer::setRoot(std::string &root)
{
	this->_root = root;
}

void ConfigServer::setIndex(std::string &index)
{
	this->_index = index;
}

void ConfigServer::setCgiPass(std::string &cgi_pass)
{
	this->_cgi_pass = cgi_pass;
}

std::vector<std::string> ConfigServer::getServerBlocks(void) const
{
	return (this->_serverBlocks);
}

std::map<std::string, ConfigLocation *> ConfigServer::getLocation(void) const
{
	return (this->_mapLocations);
}

std::vector<int> ConfigServer::getPorts(void) const
{
	return (this->_ports);
}

std::string ConfigServer::getHost(void) const
{
	return (this->_host);
}

std::vector<std::string> ConfigServer::getServerNames(void) const
{
	return (this->_serverNames);
}

unsigned int ConfigServer::getClientMaxBodySize(void) const
{
	return (this->_client_max_body_size);
}

std::map<int, std::string> ConfigServer::getErrorPages(void) const
{
	return (this->_errorPages);
}

std::string ConfigServer::getRoot(void) const
{
	return (this->_root);
}

std::string ConfigServer::getIndex(void) const
{
	return (this->_index);
}

bool ConfigServer::getAutoIndex(void) const
{
	return (this->_autoindex);
}

std::vector<std::string> ConfigServer::getMethods(void) const
{
	return (this->_methods);
}

std::map<int, std::string> ConfigServer::getReturn(void) const
{
	return (this->_return);
}
