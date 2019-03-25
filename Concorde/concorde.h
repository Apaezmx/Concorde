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
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <list>

namespace concorde {
enum ResourceType {
    PUT,
    GET,
    POST,
    UNDEFINED
};
typedef std::function<std::string(::httpparser::Request&)> Method;
    
class ClientThread {
public:
    ClientThread(std::unordered_map<std::string, Method>* res);
    ~ClientThread();
    
    void init(int server_file_descriptor);
    
    bool is_closed();
    
    void handle_client();
    
private:
    int file_descriptor;
    struct sockaddr_in address;
    bool closed;
    std::unordered_map<std::string, Method>* resources;  // Not owner!
};

class Server {
public:
    Server(int port);
    ~Server();
    
    void run();
    
    void register_post(const std::string& path, Method method);
    void register_get(const std::string& path, Method method);
    
private:
    std::list<ClientThread> threads;
    std::unordered_map<std::string, Method> resources;
    int socket_file_descriptor;
    struct sockaddr_in address;
    
};
    
}  // namespace concorde
#endif /* CONCORDE_H */
