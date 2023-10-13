#include "src/inc/Server.hpp"
#include "src/inc/Config.hpp"

int main(int argc, char **argv)
{
    (void)argv;
    if (argc > 2)
    {
        std::cerr << RED << "Error: arguments is not correct!" << RESET << std::endl;
        return 1;
    }
    Configuration *config;

    if (argc == 1)
        config = new Configuration("config_files/default.conf");
    else
        config = new Configuration(argv[1]);

    utils::Server server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 64, *config);
    server.launcher();

    delete config;

    return (0);
}
