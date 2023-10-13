#pragma once

#include <string>
#include <iostream>
#include <dirent.h>
#include "Config.hpp"
#include "Utils.hpp"

#define COLON ":"
#define SPACE " "
#define NEWLINE "\n"
#define SLASH "/"
#define ASSETS "website/assets"
#define CONTENT_LENGTH_FIELD "Content-Length"

namespace utils
{
    class Request
    {
    private:
        std::string _buffer;
        ConfigServer *_config;
        bool _isComplete;
        std::string _header;
        std::string _body;
        std::string _method;
        std::string _target;
        std::string _protocolVersion;
        std::map<std::string, std::string> _headerFields;
        std::string _path;
        std::string _response;
        std::string _return;
        std::string _cgi_pass;
        int _returnStatus;
        bool _autoIndexFlag;
        bool _cgiPassFlag;
        std::string _queryString;
        std::map<std::string, ConfigLocation *> _locations;
        void _checkPos(size_t pos);
        std::string _getParam(std::string toGet, size_t offset);
        void _checkPath();
        bool _checkMethod(std::vector<std::string> methods);
        void _parseStartingLine();
        void _parseHeaderFields();
        void _parseGetTarget();
        std::map<std::string, ConfigLocation *>::iterator _findLocation();

    public:
        Request();
        Request(const std::string &buffer, ConfigServer *config, bool isComplete);
        Request(const Request &src);
        ~Request();

        Request &operator=(const Request &rhs);
        void readBuffer();

        // setters
        void setReturnStatus(int code);

        // getters
        void clear();
        std::string getBuffer() const;
        bool getIsComplete() const;
        std::string getHeader() const;
        std::string getBody() const;
        std::string getMethod() const;
        std::string getTarget() const;
        std::string getProtocol() const;
        std::map<std::string, std::string> getHeaderFields() const;
        ConfigServer *getConfig() const;
        std::string getPath() const;
        std::string getReturn() const;
        std::string getResponse() const;
        int getReturnStatus() const;
        bool getAutoIndexFlag() const;
        bool getCgiPassFlag() const;
        std::string getQueryString() const;
        std::string getCgiPass() const;
        void setPath(std::string &path);
        int chunkRequest();
    };
}