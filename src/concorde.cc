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
#include <limits.h>

#define BUF_SIZE 1 << 24
#define DEBUG(x) std::cerr <<  x << std::flush

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
    // To use for malloc unique_ptrs.
    struct free_delete {
        void operator()(void* x) { free(x); }
    };
}

ClientThread::ClientThread() {
    closed = false;
}
ClientThread::~ClientThread() {
    closed = true;
    thread_->join();
    close(file_descriptor);
}

bool ClientThread::is_closed() {
    return closed;
}

void ClientThread::handle_client() {
    char* buf = new char[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    DEBUG("Reading payload");
    size_t num_bytes = read(file_descriptor, buf, BUF_SIZE);
    httpparser::Request req;
    httpparser::HttpRequestParser parser;
    auto result = parser.parse(req, buf, buf + num_bytes);
    free(buf);
    if (result != httpparser::HttpRequestParser::ParsingCompleted) {
        DEBUG("Could not parse request");
        httpparser::Response response = str2resp("Could not parse request", 400);
        std::string out = response.inspect();
        write(file_descriptor, out.c_str(), out.length());
        closed=true;
        return;
    }
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
    thread_ = std::unique_ptr<std::thread>(new std::thread(&ClientThread::handle_client, this));
}
    
std::unordered_map<std::string, ServerMethod*>& ServerMethod::registry() {
    static std::unordered_map<std::string, ServerMethod*> impl;
    return impl;
}

Server::~Server() {
    DEBUG("Closing socket");
    close(socket_file_descriptor);
}

Server::Server(int port) {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0)
        error("ERROR opening socket");
    int len, rc, on = 1;
    rc = setsockopt(socket_file_descriptor, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0) {
      close(socket_file_descriptor);
      error("setsockopt() failed");
    }
    rc = ioctl(socket_file_descriptor, FIONBIO, (char *)&on);
    if (rc < 0) {
      close(socket_file_descriptor);
      error("ioctl() failed");
    }
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(socket_file_descriptor, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
      close(socket_file_descriptor);
      error("ERROR on binding");
    }
    rc = listen(socket_file_descriptor, 32);
    if (rc < 0) {
      close(socket_file_descriptor);
      error("listen() failed");
    }
}

bool Server::PortInUse(int port) {
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0)
    return true;
  struct sockaddr_in address;
  bzero((char *) &address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (bind(sfd, (struct sockaddr *) &address,
           sizeof(address)) < 0)
    return true;
  close(sfd);
  return false;
}

void Server::run() {
    while (true) {
        if (stop_) {
          break;
        }
        for (auto it = threads_.begin(); it != threads_.end(); it++) {
            if ((*it)->is_closed()) {
                DEBUG("Erasing client");
                it = threads_.erase(it);
            }
        }
        struct pollfd fds;
        fds.fd = socket_file_descriptor;
        fds.events = POLLIN;
        int ready = poll(&fds, 1, 500);
        if (ready < 0 || fds.revents != POLLIN)
            continue;
        
        auto client_thread = std::unique_ptr<ClientThread>(new ClientThread());
        DEBUG("New Request Thread");
        client_thread->init(socket_file_descriptor);
        threads_.emplace_back(std::move(client_thread));
    }
}

void Server::stop() {
  stop_ = true;
}

std::thread Server::run_async() {
    return std::thread(&Server::run, this);
}

}  // namespace concorde
