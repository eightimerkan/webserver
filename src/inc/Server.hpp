#pragma once

#include <unistd.h>
#include <string>
#include <sys/time.h> // struct timeval
#include <fstream>
#include <sstream>
#include <map>
#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "CgiHandler.hpp"

#define BUFFER_SIZE 8192

namespace utils
{
    class Server
    {
    private:
        struct timeval _timeout;
        std::map<int, Socket *> _socket;
        std::vector<int> _sockfds;
        Request _req;
        Configuration _config;
        int _max_sd;
        std::string _buf;
        fd_set _working_set, _master_set;
        std::map<int, ConfigServer *> _socketServer;
        std::vector<ConfigServer *> _servers;
        int _accepter(int sockfd);
        int _handler(int sockfd);
        int _responder(int sockfd);
        bool _checkSocket(int sockfd);
        bool _checkCgi(Request &req);
        int _deleteFile(int socketFds);

    public:
        Server();
        Server(int domain, int service, int protocol,
               u_long interface, int backlog,
               Configuration config);
        Server(const Server &src);
        ~Server();

        Server &operator=(const Server &rhs);
        void launcher();
        void test_connection(int to_test);

        // getters
        std::map<int, utils::Socket *> getServerSd();
        std::map<int, ConfigServer *> getSocketServer() const;
        std::vector<ConfigServer *> getServers() const;
    };
}