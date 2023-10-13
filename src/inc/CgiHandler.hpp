#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <sys/wait.h>
#include "Request.hpp"
#include "Response.hpp"

#define CONTENT_TYPE "CONTENT_TYPE"
#define CONTENT_LENGTH "CONTENT_LENGTH"
#define REQUEST_METHOD "REQUEST_METHOD"
#define SERVER_PROTOCOL "SERVER_PROTOCOL"
#define HTTP_USER_AGENT "HTTP_USER_AGENT"
#define PATH_INFO "PATH_INFO"
#define SCRIPT_FILENAME "SCRIPT_FILENAME"
#define SCRIPT_NAME "SCRIPT_NAME"
#define USER_AGENT_FIELD "User-Agent"
#define COOKIE "Cookie"
#define CONTENT_TYPE_FIELD "Content-Type"
#define QUERY_STRING "QUERY_STRING"
#define GATEWAY_INTERFACE "GATEWAY_INTERFACE"
#define REDIRECT_STATUS "REDIRECT_STATUS"
#define HTTP_COOKIE "HTTP_COOKIE"

#define BUFFER_LENGTH 4096

class Request;

namespace utils
{

    class CgiHandler
    {
    private:
        Request _req;
        std::string _executable;
        std::map<std::string, std::string> _envVariables;
        int _socketFd;

        void initializeEnv(void);
        std::string createEnvString(std::string leftString, std::string righString);

        CgiHandler();

    public:
        CgiHandler(Request req, int socketFd);
        ~CgiHandler();
        CgiHandler(CgiHandler const &src);

        CgiHandler &operator=(CgiHandler const &rhs);

        Request getRequest(void) const;
        std::string getExecutable(void) const;
        std::map<std::string, std::string> getEnvVariables(void) const;
        int getSocketFd(void) const;

        int execute(void);
    };
}
#endif