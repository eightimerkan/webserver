#include "inc/Socket.hpp"

utils::Socket::Socket() {}

utils::Socket::Socket(int domain, int service, int protocol, std::vector<int> ports,
                      u_long interface, int backlog, int generation)
{
    int ret;
    int on = 1;
    int sockfd;
    std::vector<int>::iterator it;

    _generation = generation;
    for (it = ports.begin(); it != ports.end(); ++it)
    {
        _address.sin_family = domain;
        _address.sin_port = htons(*it);
        _address.sin_addr.s_addr = htonl(interface);

        sockfd = socket(domain, service, protocol);
        test_connection(sockfd, sockfd);
        _backlog = backlog;
        ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
        test_connection(ret, sockfd);
        ret = fcntl(sockfd, F_SETFL, O_NONBLOCK);
        test_connection(ret, sockfd);
        connect_to_network(sockfd);
        _sockfds.push_back(sockfd);
    }
    return;
}

utils::Socket::Socket(Socket const &src)
{
    *this = src;
}

utils::Socket::~Socket() {}

utils::Socket &utils::Socket::operator=(Socket const &rhs)
{
    if (this != &rhs)
    {
        _sockfds = rhs._sockfds;
        _max_sd = rhs._max_sd;
        _backlog = rhs._backlog;
        _address = rhs._address;
        _generation = rhs._generation;
    }
    return (*this);
}

void utils::Socket::connect_to_network(int sockfd)
{
    int ret = 0;

    ret = bind(sockfd, (struct sockaddr *)&_address, sizeof(_address));
    test_connection(ret, sockfd);
    ret = listen(sockfd, _backlog);
    test_connection(ret, sockfd);
    _max_sd = sockfd;
}

void utils::Socket::setGeneration(int generation) { _generation = generation; }

void utils::Socket::test_connection(int to_test, int socket)
{
    if (to_test < 0)
    {
        std::cerr << "Webserver error: " << strerror(to_test) << std::endl;
        close(socket);
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in utils::Socket::getAddress() const { return _address; }

std::vector<int> utils::Socket::getSockets() const { return _sockfds; }

int utils::Socket::getMaxsd() const { return _max_sd; }

int utils::Socket::getGeneration() const { return _generation; }
