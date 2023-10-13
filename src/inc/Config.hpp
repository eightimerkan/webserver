#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "ConfigServer.hpp"
#include "Utils.hpp"

#define START_SERVER_BLOCK "server {"
#define END_SERVER_BLOCK "\n}"

class ConfigServer;

class Configuration
{

private:
    std::vector<ConfigServer *> _vect_servers;
    std::vector<std::string> _config_blocks;

    void parseBlockConfig(std::string const &path);
    void parseConfig();

public:
    Configuration();
    Configuration(std::string const &path);
    Configuration(Configuration const &src);
    ~Configuration();

    Configuration &operator=(Configuration const &rhs);

    std::vector<ConfigServer *> getConfigServer(void) const;
};

#endif