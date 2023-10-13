#include "inc/Config.hpp"
#include "inc/Utils.hpp"

Configuration::Configuration()
{
}

Configuration::Configuration(std::string const &path)
{
	parseBlockConfig(path);
	parseConfig();
}

Configuration::Configuration(Configuration const &src)
{
	*this = src;
}
Configuration::~Configuration()
{
	this->_vect_servers.clear();
}

Configuration &Configuration::operator=(Configuration const &rhs)
{

	if (this != &rhs)
	{
		this->_vect_servers = rhs._vect_servers;
		this->_config_blocks = rhs._config_blocks;
	}
	return (*this);
}

void Configuration::parseBlockConfig(std::string const &path)
{

	std::ifstream ifs;
	size_t posb = 0;
	size_t pose = 0;

	if (!utils::checkExtension(path, ".conf"))
	{
		std::cout << "Error : wrong file extension\n";
		exit(EXIT_FAILURE);
	}
	ifs.open(path);
	if (ifs.fail())
	{
		std::cout << "Error : empty configuration file\n";
		exit(EXIT_FAILURE);
	}
	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	while (posb != std::string::npos)
	{
		posb = content.find(START_SERVER_BLOCK, posb);
		pose = content.find(END_SERVER_BLOCK, posb);
		if (posb < content.length() && pose << content.length())
		{
			this->_config_blocks.push_back(content.substr(posb, pose + 2 - posb));
			posb = pose + 1;
			pose = posb;
		}
		else
			break;
	}
	ifs.close();
}

void Configuration::parseConfig()
{
	for (unsigned int i = 0; i < this->_config_blocks.size(); i++)
	{
		this->_vect_servers.push_back(new ConfigServer(this->_config_blocks[i]));
	}
}

std::vector<ConfigServer *> Configuration::getConfigServer(void) const
{
	return (this->_vect_servers);
}