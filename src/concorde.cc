//
//  server.cpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#include "httprequestparser.h"
#include "request.h"
#include "response.h"
#include "concorde.h"

#include <future>
#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <thread>
#include <chrono>

namespace concorde {
namespace {
    void error(const char *msg) {
        perror(msg);
        exit(1);
    }
    httpparser::Response str2resp(std::string data, unsigned int code) {
        httpparser::Response response;
        response.statusCode = code;
        response.versionMajor = 1;
        response.versionMinor = 1;
        if (code == 200) {
            response.status = "OK";
        } else {
            response.status = "FAIL";
        }
        // Fill headers.
        httpparser::Response::HeaderItem item;
        item.name = "Content-Type";
        item.value = "text/html";
        response.headers.push_back(item);
        item.name = "Server";
        item.value = "Concorde";
        response.headers.push_back(item);
        item.name = "Content-Length";
        item.value = std::to_string(data.size()) + "\n";
        response.headers.push_back(item);
        for (char c : data) {
            response.content.push_back(c);
        }
        return response;
    }
}

ClientThread::ClientThread() {
    closed = false;
}
ClientThread::~ClientThread() {
    close(file_descriptor);
}

bool ClientThread::is_closed() {
    return closed;
}

void ClientThread::handle_client() {
    char buf[2<<10];
    bzero(buf,2<<10);
    std::cout << "Reading payload" << std::endl;
    size_t num_bytes = read(file_descriptor, buf, 2<<10);
    httpparser::Request req;
    httpparser::HttpRequestParser parser;
    auto result = parser.parse(req, buf, buf + num_bytes);
    if (result != httpparser::HttpRequestParser::ParsingCompleted) {
        std::cout << "Could not parse request" << std::endl;
        httpparser::Response response = str2resp("Could not parse request", 400);
        std::string out = response.inspect();
        write(file_descriptor, out.c_str(), out.length());
        closed=true;
        return;
    }
    std::cout << req.inspect() << std::endl;  // For debugging. TODO remove.
    auto it = ServerMethod::registry().find(req.uri);
    if (it == ServerMethod::registry().end()) {
        httpparser::Response response = str2resp("Uri not found", 404);
        std::string out = response.inspect();
        write(file_descriptor, out.c_str(), out.length());
        closed=true;
        return;
    }
    std::string data = it->second->logic(req);
    httpparser::Response response = str2resp(data, 200);
    std::string out = response.inspect();
    std::cout << "Response " << out << std::endl;
    write(file_descriptor, out.c_str(), out.length());

    closed = true;
}

void ClientThread::init(int server_file_descriptor) {
    socklen_t address_len = sizeof(address);
    file_descriptor = accept(server_file_descriptor,
                           (struct sockaddr *) &address,
                           &address_len);
    if (file_descriptor < 0)
        error("ERROR on accept");
    std::cout << "Async" << std::endl;
    std::async(&ClientThread::handle_client, this);
}
    
std::unordered_map<std::string, ServerMethod*>& ServerMethod::registry() {
    static std::unordered_map<std::string, ServerMethod*> impl;
    return impl;
}

Server::~Server() {
    close(socket_file_descriptor);
}

Server::Server(int port) {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0)
        error("ERROR opening socket");
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(socket_file_descriptor, (struct sockaddr *) &address,
             sizeof(address)) < 0)
        error("ERROR on binding");
    listen(socket_file_descriptor,5);
}

void Server::run() {
    while (true) {
        struct pollfd fds;
        fds.fd = socket_file_descriptor;
        fds.events = POLLIN;
        int ready = poll(&fds, 1, 2000);
        if (ready < 0)
            continue;
        
        ClientThread client_thread;
        std::cout << "New Client" << std::endl;
        client_thread.init(socket_file_descriptor);
        threads.push_back(client_thread);
        for (auto it = threads.begin(); it != threads.end(); it++) {
            if (it->is_closed()) {
                std::cout << "Errasing client" << std::endl;
                it = threads.erase(it);
            }
        }
    }
}

}  // namespace concorde
