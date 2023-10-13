#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <list>
#include <utility>
#include <vector>
#include <map>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define LOCATION "location"
#define END_LOCATION_BLOCK "}\n"
#define COMMENT_DELIMITER "#"
#define LISTEN "listen"
#define HOST "host"
#define SERVER_NAME "server_name"
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

#define CHAR_TAB '\t'
#define CHAR_SPACE ' '
#define CHAR_SEMI_COLON ';'
#define CLOSING_BRACE "}"

namespace utils
{
    std::string readFile(const std::string &file);
    bool checkExtension(const std::string &file, const std::string &ext);
    bool checkNoExtension(const std::string &file);
    bool fileExists(const std::string &file);
    bool isDirectory(const std::string &path);
    void trimTrailingChar(std::string &str, char trail);
    void trimOneBlock(std::string &str, char separ);
    std::string substrUntil(std::string str, char separ);
    std::string removeRootPath(const std::string &target, const std::string &root);

    template <typename T>
    void printVector(std::vector<T> vector)
    {
        for (size_t i = 0; i < vector.size(); i++)
            std::cout << i << ": " << vector[i] << "\n";
    }
    std::list<std::pair<std::string, std::string> > parseBlock(std::string &content);
    void printMap(std::map<int, std::string> map);
    void printMapStrings(std::map<std::string, std::string> map);
}