#include "inc/Server.hpp"

utils::Server::Server() {}

utils::Server::Server(int domain, int service, int protocol,
                      u_long interface, int backlog,
                      Configuration config) : _config(config)
{
    int generation = 1;
    _servers = _config.getConfigServer();
    _max_sd = 0;
    for (std::vector<ConfigServer *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        std::vector<int> port = (*it)->getPorts();
        Socket *socket = new Socket(domain, service, protocol, port, interface, backlog, generation);
        std::vector<int> tmp = socket->getSockets();
        for (std::vector<int>::iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2)
        {
            _socketServer[*it2] = *it;
            _socket[*it2] = socket;
            _sockfds.push_back(*it2);
            if (*it2 > _max_sd)
                _max_sd = *it2;
        }
        ++generation;
    }
    FD_ZERO(&_master_set);
    for (std::vector<int>::iterator it = _sockfds.begin(); it != _sockfds.end(); ++it)
        FD_SET((*it), &_master_set);
    _timeout.tv_sec = 0;
    _timeout.tv_usec = 0;
}

utils::Server::Server(Server const &src)
{
    *this = src;
}

utils::Server::~Server()
{
    std::map<int, Socket *>::iterator it = _socket.begin();
    int prev_gen = 0;

    for (; it != _socket.end(); ++it)
    {
        if (prev_gen != it->second->getGeneration())
        {
            prev_gen = it->second->getGeneration();
            delete it->second;
        }
    }
    std::vector<ConfigServer *> vectServers = _config.getConfigServer();
    std::vector<ConfigServer *>::iterator itServ = vectServers.begin();
    for (; itServ != vectServers.end(); ++itServ)
    {
        delete *itServ;
    }
}

utils::Server &utils::Server::operator=(Server const &rhs)
{
    if (this != &rhs)
    {
        _config = rhs._config;
        _timeout = rhs._timeout;
        _req = rhs._req;
        _socket = rhs._socket;
        _sockfds = rhs._sockfds;
        _max_sd = rhs._max_sd;
        _buf = rhs._buf;
        _working_set = rhs._working_set;
        _master_set = rhs._master_set;
        _socketServer = rhs._socketServer;
        _servers = rhs._servers;
    }
    return (*this);
}

int utils::Server::_accepter(int sockfd)
{
    struct sockaddr_in address = _socket[sockfd]->getAddress();
    int new_sd;
    int addrlen = sizeof(address);

    new_sd = accept(sockfd, (struct sockaddr *)&address,
                    (socklen_t *)&addrlen);
    _socketServer[new_sd] = _socketServer[sockfd];
    return new_sd;
}

int utils::Server::_handler(int sockfd)
{
    char cbuf[BUFFER_SIZE];
    ssize_t bytesRead = 1;
    memset(cbuf, 0, BUFFER_SIZE);
    bytesRead = read(sockfd, cbuf, BUFFER_SIZE - 1);
    if (bytesRead > 0)
        _buf.append(cbuf, bytesRead);
    else if (bytesRead == 0)
        return -1;
    else
        return 0;
    Request req(_buf, _socketServer[sockfd], false);
    _req.clear();
    _req = req;
    if (req.getMethod() != "POST" || (req.getMethod() == "POST" && !req.getHeaderFields()[CONTENT_LENGTH_FIELD].empty() && _buf.length() >= std::stoul(req.getHeaderFields()[CONTENT_LENGTH_FIELD].c_str())))
    {
        Request req(_buf, _socketServer[sockfd], true);
        _req.clear();
        _req = req;
    }
    return bytesRead;
}

int utils::Server::_responder(int sockfd)
{
    int ret;
    Response response(_req);
    std::string toSend;
    if (_req.getReturnStatus() >= 0)
    {
        toSend = response.getResponse();
        ret = send(sockfd, toSend.c_str(), toSend.size(), 0);
    }
    else if (this->_req.getMethod() == "DELETE")
        ret = this->_deleteFile(sockfd);
    else if (_checkCgi(this->_req))
    {
        CgiHandler cgi = CgiHandler(_req, sockfd);
        ret = cgi.execute();
        if (ret)
        {
            close(sockfd);
            FD_CLR(sockfd, &_master_set);
            if (sockfd == _max_sd)
            {
                while (!FD_ISSET(_max_sd, &_master_set))
                    _max_sd -= 1;
            }
        }
    }
    else
    {
        toSend = response.getResponse();
        ret = send(sockfd, toSend.c_str(), toSend.size(), 0);
    }
    _buf.clear();
    return ret;
}

bool utils::Server::_checkSocket(int sockfd)
{
    std::vector<int>::iterator it;

    for (it = _sockfds.begin(); it != _sockfds.end(); ++it)
    {
        if (sockfd == *it)
            return true;
    }
    return false;
}

void utils::Server::launcher()
{
    int ret = 0, sds_ready = 0, new_sd = 0;
    bool end_server = false, close_conn = false;
    signal(SIGPIPE, SIG_IGN);
    while (!end_server)
    {
        memcpy(&_working_set, &_master_set, sizeof(_master_set));
        ret = select(_max_sd + 1, &_working_set, NULL, NULL, NULL);
        if (ret == -1)
        {
            std::cerr << "Select() Error" << std::endl;
            break;
        }
        else if (ret == 0)
        {
            std::cout << "Timeout" << std::endl;
            break;
        }
        sds_ready = ret;
        for (int i = 0; i <= _max_sd && sds_ready > 0; ++i)
        {
            if (FD_ISSET(i, &_working_set))
            {
                sds_ready -= 1;
                if (_checkSocket(i))
                {
                    while (true)
                    {
                        new_sd = _accepter(i);
                        if (new_sd < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                std::cerr << "Accept() error" << std::endl;
                                end_server = true;
                            }
                            break;
                        }
                        FD_SET(new_sd, &_master_set);
                        if (new_sd > _max_sd)
                            _max_sd = new_sd;
                        if (new_sd == -1)
                            break;
                    }
                }
                else
                {
                    close_conn = false;
                    ret = _handler(i);
                    if (ret <= 0)
                        close_conn = true;
                    if (_req.getIsComplete() && (_req.getMethod() != "POST" || (_req.getMethod() == "POST" && !_req.getHeaderFields()[CONTENT_LENGTH_FIELD].empty() && _buf.length() >= std::stoul(_req.getHeaderFields()[CONTENT_LENGTH_FIELD].c_str()))))
                    {
                        ret = _responder(i);
                        if (ret == -1)
                            close_conn = true;
                    }
                    if (close_conn)
                    {
                        _buf.clear();
                        close(i);
                        FD_CLR(i, &_master_set);
                        if (i == _max_sd)
                        {
                            while (!FD_ISSET(_max_sd, &_master_set))
                                _max_sd -= 1;
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i <= _max_sd; ++i)
    {
        if (FD_ISSET(i, &_master_set))
            close(i);
    }
}

void utils::Server::test_connection(int to_test)
{
    std::cerr << "Webserver error: " << strerror(to_test) << std::endl;
}

bool utils::Server::_checkCgi(Request &req)
{
    if ((checkExtension(req.getPath(), ".php") || checkExtension(req.getPath(), ".py")) && req.getCgiPassFlag())
        return (true);
    return (false);
}

int utils::Server::_deleteFile(int socketFd)
{
    std::string response = "HTTP/1.1 204 No Content\n\n";
    remove((this->_req.getPath()).c_str());
    return send(socketFd, response.c_str(), response.size(), 0);
}

std::map<int, utils::Socket *> utils::Server::getServerSd() { return _socket; }
std::map<int, ConfigServer *> utils::Server::getSocketServer() const { return _socketServer; }
std::vector<ConfigServer *> utils::Server::getServers() const { return _servers; }