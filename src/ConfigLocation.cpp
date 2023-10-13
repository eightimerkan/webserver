#include "inc/ConfigLocation.hpp"

ConfigLocation::ConfigLocation(std::string &content)
{

    this->_autoindex = 0;
    parseLocation(content);
    utils::printMap(this->_errorPages);
}

ConfigLocation::ConfigLocation(ConfigLocation const &src)
{

    *this = src;
}

ConfigLocation::~ConfigLocation()
{
}

void ConfigLocation::parseErrorPage(std::pair<std::string, std::string> &pair)
{

    size_t posb = 0;
    size_t pos = (pair.second).find(CHAR_SPACE);

    int code = stoi((pair.second).substr(posb, pos - posb));
    std::string page = (pair.second).substr(pos + 1, (pair.second).length() - pos);

    this->_errorPages.insert(std::pair<int, std::string>(code, page));
}

void ConfigLocation::parseAutoIndex(std::pair<std::string, std::string> &pair)
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

void ConfigLocation::parseMethods(std::pair<std::string, std::string> &pair)
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

void ConfigLocation::parseReturn(std::pair<std::string, std::string> &pair)
{
    this->_return = pair.second;
}

void ConfigLocation::dispatch(std::pair<std::string, std::string> &pair)
{
    if (pair.first == ROOT)
        setRoot(pair.second);
    else if (pair.first == INDEX)
        setIndex(pair.second);
    else if (pair.first == CGI_PASS)
        setCgiPass(pair.second);
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

void ConfigLocation::parseLocation(std::string &content)
{

    std::list<std::pair<std::string, std::string> > list;

    list = utils::parseBlock(content);
    for (std::list<std::pair<std::string, std::string> >::iterator it = list.begin(); it != list.end(); it++)
    {
        if (((*it).first).empty())
            continue;
        dispatch(*it);
    }
}

ConfigLocation &ConfigLocation::operator=(ConfigLocation const &rhs)
{

    if (this != &rhs)
    {
        this->_errorPages = rhs._errorPages;
        this->_root = rhs._root;
        this->_index = rhs._index;
        this->_autoindex = rhs._autoindex;
        this->_methods = rhs._methods;
        this->_return = rhs._return;
    }
    return (*this);
}

void ConfigLocation::setRoot(std::string &root)
{
    this->_root = root;
}

void ConfigLocation::setIndex(std::string &index)
{
    this->_index = index;
}

void ConfigLocation::setCgiPass(std::string &cgi_pass)
{
    this->_cgi_pass = cgi_pass;
}

std::string ConfigLocation::getCgiPass() const
{
    return (this->_cgi_pass);
}

std::map<int, std::string> ConfigLocation::getErrorPages(void) const
{
    return (this->_errorPages);
}

std::string ConfigLocation::getRoot(void) const
{
    return (this->_root);
}

std::string ConfigLocation::getIndex(void) const
{
    return (this->_index);
}

bool ConfigLocation::getAutoIndex(void) const
{
    return (this->_autoindex);
}

std::vector<std::string> ConfigLocation::getMethods(void) const
{
    return (this->_methods);
}

std::string ConfigLocation::getReturn(void) const
{
    return (this->_return);
}