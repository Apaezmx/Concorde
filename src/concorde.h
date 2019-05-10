//
//  server.hpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#ifndef CONCORDE_H
#define CONCORDE_H

#include "request.h"

#include <stdio.h>
#include <typeindex>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <list>
#include <thread>

typedef ::httpparser::Request HttpRequest;

#define REGISTER_METHOD(name, path, f)\
class name : public concorde::ServerMethod\
{\
static concorde::Registrar<name> registrar; \
std::string logic(HttpRequest& request) override {return f(request);} \
};\
concorde::Registrar<name> name::registrar(path);

namespace concorde {
enum ResourceType {
    PUT,
    GET,
    POST,
    UNDEFINED
};
    
class ClientThread {
public:
    ClientThread();
    ~ClientThread();
    
    void init(int server_file_descriptor);
    
    bool is_closed();
    
    void handle_client();
    
private:
    int file_descriptor;
    struct sockaddr_in address;
    bool closed;
    std::unique_ptr<std::thread> thread_;
};
        
class ServerMethod {
public:
    static std::unordered_map<std::string, ServerMethod*>& registry();
    virtual std::string logic(HttpRequest& request) = 0;
};

template <class T> struct Registrar
{
    Registrar(std::string const& s) {
        ServerMethod::registry()[s] = new T();
    }
};

class Server {
public:
    Server(int port);
    ~Server();
    
    void run();

    void stop();

    std::thread run_async();

    static bool PortInUse(int port);

private:
    std::list<std::unique_ptr<ClientThread>> threads_;
    int socket_file_descriptor;
    struct sockaddr_in address;
    bool stop_ = false;
};
    
}  // namespace concorde
#endif /* CONCORDE_H */
