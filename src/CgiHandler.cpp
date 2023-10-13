#include "inc/CgiHandler.hpp"

// Destructor
utils::CgiHandler::~CgiHandler()
{
    this->_envVariables.clear();
}

// Constructor
utils::CgiHandler::CgiHandler(utils::Request req, int socketFd)
{
    this->_req = req;
    this->_socketFd = socketFd;
    this->_executable = this->_req.getPath();
    CgiHandler::initializeEnv();
}

// Copy constructor
utils::CgiHandler::CgiHandler(CgiHandler const &src)
{
    *this = src;
}

// Override = operator
utils::CgiHandler &utils::CgiHandler::operator=(CgiHandler const &rhs)
{
    if (this != &rhs)
    {
        this->_envVariables = rhs.getEnvVariables();
        this->_executable = rhs.getExecutable();
        this->_req = rhs.getRequest();
        this->_socketFd = rhs.getSocketFd();
    }
    return (*this);
}

utils::Request utils::CgiHandler::getRequest(void) const
{
    return (this->_req);
}

std::string utils::CgiHandler::getExecutable(void) const
{
    return (this->_executable);
}

std::map<std::string, std::string> utils::CgiHandler::getEnvVariables(void) const
{
    return (this->_envVariables);
}

int utils::CgiHandler::getSocketFd(void) const
{
    return (this->_socketFd);
}

std::string utils::CgiHandler::createEnvString(std::string leftString, std::string righString)
{
    return (leftString + "=" + righString.c_str());
}

void utils::CgiHandler::initializeEnv(void)
{
    this->_envVariables[CONTENT_TYPE] = createEnvString(CONTENT_TYPE, this->_req.getHeaderFields()[CONTENT_TYPE_FIELD]);
    this->_envVariables[CONTENT_LENGTH] = createEnvString(CONTENT_LENGTH, std::to_string(this->_req.getBody().length()));
    this->_envVariables[HTTP_USER_AGENT] = createEnvString(HTTP_USER_AGENT, this->_req.getHeaderFields()[USER_AGENT_FIELD]);
    this->_envVariables[PATH_INFO] = createEnvString(PATH_INFO, this->_req.getTarget());
    this->_envVariables[REQUEST_METHOD] = createEnvString(REQUEST_METHOD, this->_req.getMethod());
    this->_envVariables[SCRIPT_FILENAME] = createEnvString(SCRIPT_FILENAME, this->_req.getPath());
    this->_envVariables[SCRIPT_NAME] = createEnvString(SCRIPT_NAME, this->_req.getTarget());
    this->_envVariables[SERVER_PROTOCOL] = createEnvString(SERVER_PROTOCOL, this->_req.getProtocol());
    this->_envVariables[QUERY_STRING] = createEnvString(QUERY_STRING, this->_req.getQueryString());
    this->_envVariables[GATEWAY_INTERFACE] = createEnvString(GATEWAY_INTERFACE, "CGI/1.1");
    this->_envVariables[REDIRECT_STATUS] = createEnvString(REDIRECT_STATUS, "false");
    this->_envVariables[HTTP_COOKIE] = createEnvString(HTTP_COOKIE, this->_req.getHeaderFields()[COOKIE]);
}

int utils::CgiHandler::execute(void)
{
    char *env[] = {
        &this->_envVariables[CONTENT_TYPE][0],
        &this->_envVariables[CONTENT_LENGTH][0],
        &this->_envVariables[HTTP_USER_AGENT][0],
        &this->_envVariables[PATH_INFO][0],
        &this->_envVariables[REQUEST_METHOD][0],
        &this->_envVariables[SCRIPT_FILENAME][0],
        &this->_envVariables[SCRIPT_NAME][0],
        &this->_envVariables[SERVER_PROTOCOL][0],
        &this->_envVariables[QUERY_STRING][0],
        &this->_envVariables[GATEWAY_INTERFACE][0],
        &this->_envVariables[REDIRECT_STATUS][0],
        &this->_envVariables[HTTP_COOKIE][0],
        NULL};
    int pipe_in[2];
    int pipe_out[2];
    int status = 0;
    std::string path = _req.getPath();
    std::string cgiPath = _req.getCgiPass();
    char *argvPhp[] = {(char *)cgiPath.c_str(), (char *)"-d expose_php=off", (char *)"-f", (char *)path.c_str(), NULL};
    char *argvPython[] = {(char *)cgiPath.c_str(), (char *)path.c_str(), NULL};
    std::string body = this->_req.getBody();
    if (pipe(pipe_in) < 0)
        return -1;
    if (pipe(pipe_out) < 0)
        return -1;
    int pid = fork();
    if (pid == 0)
    {
        close(pipe_out[0]);
        close(pipe_in[1]);
        if (dup2(pipe_in[0], STDIN_FILENO) == -1)
            return -1;
        close(pipe_in[0]);
        if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
            return -1;
        close(pipe_out[1]);
        execve(checkExtension(_req.getPath(), ".php") ? argvPhp[0] : argvPython[0], checkExtension(_req.getPath(), ".php") ? argvPhp : argvPython, env);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        int retPid;
        int ret;
        close(pipe_out[1]);
        close(pipe_in[0]);
        if (body.length() && write(pipe_in[1], body.c_str(), body.length()) <= 0)
            return -1;
        close(pipe_in[1]);
        std::string res = "HTTP/1.1 200 OK\r\n";
        status = send(this->_socketFd, res.c_str(), res.length(), 0);
        char buf[BUFFER_LENGTH];
        memset(buf, 0, BUFFER_LENGTH);
        while ((ret = read(pipe_out[0], buf, BUFFER_LENGTH - 1)))
        {
            if (ret < 0)
                return -1;
            // res.append(buf);
            status = send(this->_socketFd, buf, strlen(buf), 0);
            memset(buf, 0, BUFFER_LENGTH);
        }
        close(pipe_out[0]);
        if (waitpid(pid, &retPid, 0) == -1)
        {
            // int es = WEXITSTATUS(retPid);
            this->_req.setReturnStatus(500);
            Response rep = Response(_req);
            std::string toSend = rep.getResponse();
            status = send(this->_socketFd, toSend.c_str(), toSend.length(), 0);
        }
    }
    return status;
}
