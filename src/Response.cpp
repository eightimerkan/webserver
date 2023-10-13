#include "inc/Response.hpp"

utils::Response::Response() {}

utils::Response::Response(const Request &req) : _req(req) { createResponse(); }

utils::Response::Response(const Response &src) { *this = src; }

utils::Response::~Response() {}

utils::Response &utils::Response::operator=(const Response &rhs)
{
    if (this != &rhs)
    {
        _msg = rhs._msg;
        _response = rhs._response;
        _ret = rhs._ret;
        _req = rhs._req;
    }
    return *this;
}

void utils::Response::createResponse()
{
    std::string httpStatusCode = "200 OK";
    if(_req.getReturnStatus() == -2)
    {
        httpStatusCode = "301 Moved Permanently";
    }
    if (_req.getReturnStatus() >= 0)
    {
        checkRequest();
    }
    else
    {
        if (utils::checkExtension(_req.getTarget(), ".css"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: text/css\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".ico"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: image/x-icon\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".jpg") || utils::checkExtension(_req.getTarget(), ".jpeg"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: image/jpeg\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".png"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: image/png\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".pdf"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: application/pdf\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".gz") || utils::checkExtension(_req.getTarget(), ".tgz"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: application/gzip\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".zip"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: application/zip\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".js"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: application/javascript\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".txt"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: text/plain\r\nContent-Length: ";
        else if (utils::checkExtension(_req.getTarget(), ".csv"))
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: text/csv\r\nContent-Length: ";
        else
            _response = "HTTP/1.1 " + httpStatusCode + "\r\nContent-Type: text/html\r\nContent-Length: ";
        if (_req.getAutoIndexFlag())
        {
            _msg = createAutoIndex();
            if (!_msg.size())
            {
                _req.setReturnStatus(403);
                checkRequest();
            }
        }
        else
        {
            _msg = utils::readFile(_req.getPath());
        }
        _response += std::to_string(_msg.size());
        if(_req.getReturnStatus() == -2)
            _response += "\r\nLocation: " + _req.getReturn();
        _response += "\r\n\r\n";
        _response += _msg;
    }
}

void utils::Response::checkRequest()
{
    int returnStatus = _req.getReturnStatus();
    std::map<int, std::string> errorPath = _req.getConfig()->getErrorPages();
    _msg = utils::readFile(errorPath[returnStatus]);
    switch (returnStatus)
    {
    case 400:
        _response = "HTTP/1.1 400 Bad Request\r\n";
        break;
    case 403:
        _response = "HTTP/1.1 403 Forbidden\r\n";
        break;
    case 404:
        _response = "HTTP/1.1 404 Not Found\r\n";
        break;
    case 405:
        _response = "HTTP/1.1 405 Method Not Allowed\r\n";
        break;
    case 411:
        _response = "HTTP/1.1 411 Length Required\r\n";
        break;
    case 413:
        _response = "HTTP/1.1 413 Content Too Large\r\n";
        break;
    case 500:
        _response = "HTTP/1.1 500 Internal Server Error\r\n";
        break;
    case 501:
        _response = "HTTP/1.1 501 Not Implemented\r\n";
        break;
    default:
        break;
    }
    if (_msg.size() == 0)
    {
        _msg = utils::readFile("website/html/error_pages/default_error.html");
        _response = "HTTP/1.1 400 Bad Request\r\n";
    }
    _response += "Content-Type: text/html\r\n";
    _response += "Content-Length: " + std::to_string(_msg.length());
    _response += "\r\n\r\n";
    _response += _msg;
}

std::string utils::Response::createAutoIndex()
{
    std::string path = _req.getPath();
    std::string target = _req.getTarget();
    std::stringstream buf;
    DIR *dir;
    struct dirent *entry;

    buf << "<!DOCTYPE html>\n";
    buf << "<html lang=\"en\">\n";
    buf << "<head><title>AutoIndex for " << path << "</title></head>\n";
    buf << "<body>\n";
    buf << "<ul>\n";
    dir = opendir(path.c_str());
    if (!dir)
    {
        std::cerr << "Could not open the directory" << std::endl;
        return "";
    }
    else
    {
        entry = readdir(dir);
        while (entry)
        {
            if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
            {
                entry = readdir(dir);
                continue;
            }
            if (!fileExists(path + target + entry->d_name) && target[target.size() - 1] != '/')
                buf << "<li><a href=\"" << target + "/" + entry->d_name << "\">";
            else
                buf << "<li><a href=\"" << target + entry->d_name << "\">";
            buf << entry->d_name << "</a></li>\n";
            entry = readdir(dir);
        }
    }
    closedir(dir);
    buf << "</ul>\n";
    buf << "</body>\n";
    buf << "</html>\n";
    return buf.str();
}

std::string utils::Response::getMsg() const { return _msg; }
std::string utils::Response::getResponse() const { return _response; }
int utils::Response::getRet() const { return _ret; }
utils::Request utils::Response::getRequest() const { return _req; }