#include "inc/Utils.hpp"

std::string utils::readFile(const std::string &file)
{
    std::ifstream input_file(file);
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    return buffer.str();
}

bool utils::checkExtension(const std::string &file, const std::string &ext)
{
    size_t lastDot = file.find_last_of('.');

    if (lastDot != std::string::npos && lastDot < file.size())
    {
        std::string extension = file.substr(lastDot);
        return (extension.compare(ext) == 0);
    }
    return false;
}

bool utils::checkNoExtension(const std::string &file)
{
    size_t lastDot = file.find_last_of('.');

    if (lastDot != std::string::npos && lastDot < file.size())
    {
        return false;
    }
    return true;
}

bool utils::fileExists(const std::string &file)
{
    return access(file.c_str(), F_OK) == 0;
}

bool utils::isDirectory(const std::string &path)
{
    struct stat params;
    if (stat(path.c_str(), &params) == 0)
        return S_ISDIR(params.st_mode);
    return false;
}

void utils::trimTrailingChar(std::string &str, char trail)
{
    size_t pos = str.find_last_not_of(trail);

    if (pos != std::string::npos)
        str.erase(pos + 1);
}

void utils::trimOneBlock(std::string &str, char trail)
{
    size_t pos = str.find_last_of(trail);

    if (pos != std::string::npos)
        str.erase(pos);
}

std::string utils::removeRootPath(const std::string &target, const std::string &root)
{
    std::string toReturn = target;

    if (!root.compare("/"))
        return target;
    if (!toReturn.find(root))
        toReturn.erase(0, root.size());
    return toReturn;
}

std::string utils::substrUntil(std::string str, char separ)
{
    size_t pos = str.find_first_of(separ);
    std::string toReturn;

    if (pos != std::string::npos)
    {
        toReturn = str.substr(0, pos);
        return toReturn;
    }
    return "";
}

std::list<std::pair<std::string, std::string> > utils::parseBlock(std::string &content)
{
    std::list<std::pair<std::string, std::string> > list;
    std::pair<std::string, std::string> pair;
    std::istringstream ss(content);
    std::string buffer;
    size_t pos1, pos2;

    getline(ss, buffer);
    while (getline(ss, buffer))
    {
        if (buffer.empty())
            continue;
        while (buffer[0] == CHAR_TAB)
            buffer.erase(0, 1);
        if (!buffer.compare(CLOSING_BRACE))
            continue;
        pos1 = buffer.find(CHAR_SPACE);
        pair.first = buffer.substr(0, pos1);
        pos2 = buffer.find(CHAR_SEMI_COLON);
        pair.second = buffer.substr(pos1 + 1, pos2 - pos1 - 1);
        list.push_back(pair);
    }
    return (list);
}

void utils::printMap(std::map<int, std::string> map)
{
    std::map<int, std::string>::iterator it = map.begin();

    while (it != map.end())
    {
        std::cout << it->first << ", " << it->second << "\n";
        it++;
    }
}

void utils::printMapStrings(std::map<std::string, std::string> map)
{
    std::map<std::string, std::string>::iterator it = map.begin();

    while (it != map.end())
    {
        std::cout << it->first << ", " << it->second << "\n";
        it++;
    }
}
