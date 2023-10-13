#include "inc/Request.hpp"

utils::Request::Request() { _returnStatus = -1; }

utils::Request::Request(const std::string &buffer, ConfigServer *config, bool isComplete)
    : _buffer(buffer), _config(config), _isComplete(isComplete)
{
    _returnStatus = -1;
    _locations = _config->getLocation();
    _autoIndexFlag = false;
    readBuffer();
}

utils::Request::Request(Request const &src)
{
    *this = src;
}

utils::Request::~Request()
{
    _isComplete = false;
}

utils::Request &utils::Request::operator=(Request const &rhs)
{
    if (this != &rhs)
    {
        _config = rhs._config;
        _buffer = rhs._buffer;
        _header = rhs._header;
        _isComplete = rhs._isComplete;
        _body = rhs._body;
        _method = rhs._method;
        _target = rhs._target;
        _protocolVersion = rhs._protocolVersion;
        _headerFields = rhs._headerFields;
        _path = rhs._path;
        _response = rhs._response;
        _return = rhs._return;
        _returnStatus = rhs._returnStatus;
        _locations = rhs._locations;
        _autoIndexFlag = rhs._autoIndexFlag;
        _cgi_pass = rhs._cgi_pass;
        _cgiPassFlag = rhs._cgiPassFlag;
        _queryString = rhs._queryString;
    }
    return (*this);
}

void utils::Request::_checkPos(size_t pos)
{
    if (pos == std::string::npos)
        std::cerr << "Invalid character position" << std::endl;
}

void utils::Request::clear()
{
    _buffer.clear();
    _body.clear();
    _method.clear();
    _target.clear();
    _protocolVersion.clear();
    _path.clear();
    _response.clear();
    _cgi_pass.clear();
    _queryString.clear();
    _headerFields.clear();
}

std::string utils::Request::_getParam(std::string toGet, size_t offset)
{
    size_t end;
    size_t start;
    std::string param;

    start = _buffer.find(toGet);
    end = _buffer.find("\r\n", start);
    param = _buffer.substr(start + offset, end - start - offset);
    return (param);
}

void utils::Request::readBuffer()
{
    size_t crlf = 0;
    // size_t pos;
    std::string host;

    crlf = _buffer.find("\r\n\r\n");
    if (crlf == std::string::npos || !_buffer.size())
    {
        std::cerr << "Nothing was found" << std::endl;
        _returnStatus = 400;
        return;
    }
    _header = _buffer.substr(0, crlf);

    if (_isComplete && _header[0] != 'G' && _header[0] != 'P' && _header[0] != 'D')
    {
        std::cerr << "Invalid method" << std::endl;
        _returnStatus = 405;
        return;
    }
    _parseStartingLine();
    _parseHeaderFields();
    _body = _buffer.substr(crlf + 4);
    if (this->_headerFields.count("Transfer-Encoding") &&
        this->_headerFields["Transfer-Encoding"] == "chunked\r")
    {
        int contentLength = chunkRequest();
        this->_headerFields[CONTENT_LENGTH_FIELD] = contentLength;
    }
    if (_headerFields["Host"].find(":") != std::string::npos)
        host = utils::substrUntil(_headerFields["Host"], ':');
    else
        host = _headerFields["Host"];
    if (_isComplete && (!host.size() || host.compare(_config->getHost())))
    {
        std::cerr << "Host pair error" << std::endl;
        _returnStatus = 400;
        return;
    }
    if (_isComplete && this->_returnStatus < 0 && this->_method == "POST" && this->_headerFields.count(CONTENT_LENGTH_FIELD) == 0 && this->_headerFields.count("Transfer-Encoding") == 0)
    {
        std::cerr << "Content length is missing with request POST\n";
        this->_returnStatus = 411;
        return;
    }
    if (_isComplete && this->_body.size() > this->_config->getClientMaxBodySize() &&
        this->_returnStatus < 0)
    {
        std::cerr << "Body size is higher than client max body size field\n";
        this->_returnStatus = 413;
        return;
    }
    if (_isComplete && this->_returnStatus < 0 && this->_method == "POST" && this->_headerFields.count("Transfer-Encoding") == 0 && this->_headerFields.count(CONTENT_LENGTH_FIELD) && this->_body.size() != atof(this->_headerFields[CONTENT_LENGTH_FIELD].c_str()))
    {
        std::cerr << "Body size doesn't match content length\n";
        this->_returnStatus = 501;
        return;
    }
}

void utils::Request::_checkPath()
{

    std::map<std::string, ConfigLocation *>::iterator tmp;
    std::string target;

    tmp = _findLocation();
    target = _target;
    _autoIndexFlag = false;
    _cgi_pass = tmp->second->getCgiPass();
    if (tmp->second->getCgiPass().empty())
        _cgiPassFlag = false;
    else
        _cgiPassFlag = true;
    _returnStatus = -1;
    if (!_checkMethod(tmp->second->getMethods()))
    {
        _returnStatus = 405;
        return;
    }
    if (tmp->first.compare(SLASH))
    {
        target.erase(0, tmp->first.size());
    }
    _path = tmp->second->getRoot() + target;
    if(!tmp->second->getReturn().empty())
    {
        _return = tmp->second->getReturn();
        _returnStatus = -2;
        return;
    }
    utils::trimTrailingChar(_path, '/');
    if (!isDirectory(_path))
    {
        if (utils::checkNoExtension(_path))
            _path += ".html";
        if (!utils::fileExists(_path))
            _returnStatus = 404;
    }
    else
    {
        if (tmp->second->getAutoIndex())
            _autoIndexFlag = true;
        else
            _path += SLASH + tmp->second->getIndex();
    }
}

bool utils::Request::_checkMethod(std::vector<std::string> methods)
{
    std::vector<std::string>::iterator it = methods.begin();
    std::vector<std::string>::iterator itEnd = methods.end();

    if (methods.empty() && !_method.compare("GET"))
        return true;
    for (; it != itEnd; ++it)
    {
        if (!_method.compare(*it))
            return true;
    }
    return false;
}

void utils::Request::_parseStartingLine()
{
    size_t pos;
    size_t start;
    std::string file;

    pos = _header.find(" ");
    _method = _header.substr(0, pos);
    if ((_method.compare("GET") && _method.compare("POST") && _method.compare("DELETE")) || !_method.size())
    {
        _returnStatus = 405;
        return;
    }
    start = pos + 1;
    pos = _header.find(" ", start);
    _target = _header.substr(start, pos - start);
    _queryString = _target;
    _parseGetTarget();

    _checkPath();
    start = pos + 1;
    pos = _header.find("\r\n", start);
    _protocolVersion = _header.substr(start, pos - start);
    if ((_protocolVersion.compare("HTTP/1.0") && _protocolVersion.compare("HTTP/1.1")) || !_protocolVersion.size())
        _returnStatus = 400;
}

void utils::Request::_parseHeaderFields()
{
    size_t pos, pos1, pos2;
    std::pair<std::string, std::string> pair;

    pos = _header.find("\r\n");
    pos = pos + 2;
    std::string content = _header.substr(pos);
    pos = 0;
    while (pos != std::string::npos)
    {
        pos1 = content.find(COLON, pos);
        if (pos1 == std::string::npos)
        {
            _returnStatus = 400;
            return;
        }
        if (pos == 0)
            pair.first = content.substr(pos, pos1);
        else
            pair.first = content.substr(pos + 1, pos1 - pos - 1);
        pos2 = content.find(SPACE, pos1);
        if (pos2 == std::string::npos)
        {
            _returnStatus = 400;
            return;
        }
        pos = content.find(NEWLINE, pos2);
        pair.second = content.substr(pos2 + 1, pos - pos2 - 2);
        this->_headerFields.insert(pair);
    }
}

void utils::Request::_parseGetTarget(void)
{
    size_t pos2 = _target.find("?");
    if (pos2 != std::string::npos)
    {
        _target.erase(pos2, std::string::npos);
    }
    _queryString.erase(0, pos2 + 1);
}

std::map<std::string, ConfigLocation *>::iterator utils::Request::_findLocation()
{

    std::string target;
    int pos = 42;

    target = _target;
    while (pos != 0 && _locations[target] == NULL)
    {
        pos = target.find_last_of('/');
        target = target.substr(0, pos);
    }
    if (pos == 0)
        target = "/";
    return _locations.find(target);
}

int utils::Request::chunkRequest(void)
{
    std::string newBody = this->_body;
    std::string bodyToRet;
    int contentLength = 0;
    int tempBody;
    int tempLength;
    std::stringstream stream;

    while ((tempBody = newBody.find("\r\n")))
    {
        if (atoi(newBody.substr(0, tempBody).c_str()) == 0)
            break;
        stream << newBody.substr(0, tempBody).c_str();
        stream >> std::hex >> tempLength;
        contentLength += tempLength;
        newBody.erase(0, tempBody + 2);
        tempBody = newBody.find("\r\n");
        bodyToRet.append(newBody.substr(0, tempBody));
        bodyToRet.append("\n");
        contentLength += 1;
        newBody.erase(0, tempBody + 2);
    }
    bodyToRet.erase(bodyToRet.size() - 1, bodyToRet.size());
    this->_body = bodyToRet;
    return (contentLength);
}

void utils::Request::setReturnStatus(int code) { _returnStatus = code; }

std::string utils::Request::getBuffer() const { return _buffer; }
std::string utils::Request::getHeader() const { return _header; }
bool utils::Request::getIsComplete() const { return _isComplete; }
std::string utils::Request::getBody() const { return _body; }
std::string utils::Request::getMethod() const { return _method; }
std::string utils::Request::getTarget() const { return _target; }
std::string utils::Request::getProtocol() const { return _protocolVersion; }
std::map<std::string, std::string> utils::Request::getHeaderFields() const { return _headerFields; }
ConfigServer *utils::Request::getConfig() const { return _config; }
std::string utils::Request::getPath() const { return _path; }
std::string utils::Request::getReturn() const { return _return; }
std::string utils::Request::getResponse() const { return _response; }
int utils::Request::getReturnStatus() const { return _returnStatus; }
bool utils::Request::getAutoIndexFlag() const { return _autoIndexFlag; }
bool utils::Request::getCgiPassFlag() const { return _cgiPassFlag; }
std::string utils::Request::getQueryString() const { return _queryString; }
std::string utils::Request::getCgiPass() const { return _cgi_pass; }
void utils::Request::setPath(std::string &path) { this->_path = path; }