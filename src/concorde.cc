//
//  server.cpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#include <limits.h>
#include <unistd.h>
#if __APPLE__
    #include <sys/types.h>
    #include <sys/event.h>
    #include <sys/time.h>
#else
    #include <sys/epoll.h>
#endif
#include <fcntl.h>

#include <future>
#include <iostream>
#include <thread>
#include <chrono>

#include "httpparser/httprequestparser.h"
#include "httpparser/request.h"
#include "httpparser/response.h"
#include "concorde.h"

#define BUF_SIZE 1 << 8
#define DEBUG(x)
#define DEBUG(x) std::cerr <<  x << std::endl << std::flush  // Uncomment to see debug statements

namespace concorde {
namespace {
    void log_time(std::chrono::time_point<std::chrono::system_clock> start, const char* label) {
        auto end = std::chrono::system_clock::now();
        auto elapsed = end - start;
        std::cout << label << " " << elapsed.count() << '\n';
    }
    void error(const char *msg) {
        perror(msg);
        exit(1);
    }
    void error(const std::string& msg) {
        perror(msg.c_str());
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
    active = false;
    DEBUG("ClientThread created");
}
ClientThread::~ClientThread() {
    std::this_thread::sleep_for (std::chrono::microseconds(5000));
    if (thread_ && thread_->joinable())
        thread_->join();
    DEBUG("Thread Joined");
}

bool ClientThread::is_active() {
    return active;
}

void ClientThread::handle_client() {
    #define SAFE_RETURN free(buf); close(file_descriptor); active=false; return
    std::vector<char> full_message;
    char* buf = new char[BUF_SIZE];
    int num_bytes;
    do {
        DEBUG("Reading payload");
        num_bytes = recv(file_descriptor, buf, BUF_SIZE, 0);
        DEBUG(num_bytes);
        if (num_bytes > 0)
            full_message.insert(full_message.end(), buf, buf + num_bytes);
    } while (num_bytes > 0);

    if (num_bytes == -1 && full_message.size() == 0) {
        if (errno ==  EAGAIN || errno == EWOULDBLOCK) {
            // Probably nothing to read. Return gracefully.
            DEBUG("EAGAIN");
            httpparser::Response response = str2resp("Missing request bytes", 500);
            std::string out = response.inspect();
            write(file_descriptor, out.c_str(), out.length());
        } else {
            std::cerr << ("Read error " + std::to_string(errno)) << std::endl;
        }
        SAFE_RETURN;
    } else if (num_bytes == 0 && full_message.size() == 0) {
        // EOF
        SAFE_RETURN;
    }


    httpparser::Request req;
    httpparser::HttpRequestParser parser;
    auto result = parser.parse(req, &full_message[0], &full_message[0] + full_message.size());
    if (result != httpparser::HttpRequestParser::ParsingCompleted) {
        DEBUG("Could not parse request");
        DEBUG(buf);
        DEBUG(num_bytes);
        httpparser::Response response = str2resp("Could not parse request", 400);
        std::string out = response.inspect();
        write(file_descriptor, out.c_str(), out.length());
        SAFE_RETURN;
    }
    auto it = ServerMethod::registry().find(req.uri);
    if (it == ServerMethod::registry().end()) {
        httpparser::Response response = str2resp("Uri not found", 404);
        std::string out = response.inspect();
        write(file_descriptor, out.c_str(), out.length());
        SAFE_RETURN;
    }
    std::string data = it->second->logic(req);
    httpparser::Response response = str2resp(data, 200);
    std::string out = response.inspect();
    send(file_descriptor, out.c_str(), out.length(), 0);
    DEBUG("SENT 200");
    SAFE_RETURN;
}

void ClientThread::init(int server_file_descriptor) {
    active = true;
    if (thread_ && thread_->joinable())  // Just in case the thread is still active.
        thread_->join();
    socklen_t address_len = sizeof(address);
    file_descriptor = accept(server_file_descriptor,
                           (struct sockaddr *) &address,
                           &address_len);
    if (file_descriptor < 0)
        error("ERROR on accept");
    DEBUG("CONFIG DONE");
    thread_ = std::unique_ptr<std::thread>(new std::thread(&ClientThread::handle_client, this));
    DEBUG("LAUNCHED THREAD");
}

std::unordered_map<std::string, ServerMethod*>& ServerMethod::registry() {
    static std::unordered_map<std::string, ServerMethod*> impl;
    return impl;
}

Server::~Server() {
    DEBUG("Closing socket");
    close(socket_file_descriptor_);
    DEBUG("Closed socket");
}

Server::Server(int port) {
    socket_file_descriptor_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor_ < 0)
        error("ERROR opening socket");
    int len, rc, on = 1;
    rc = setsockopt(socket_file_descriptor_, SOL_SOCKET,  SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0) {
      close(socket_file_descriptor_);
      error("setsockopt() failed");
    }
    int flags = fcntl(socket_file_descriptor_, F_GETFL, 0);
    fcntl(socket_file_descriptor_, F_SETFL, flags | O_NONBLOCK);
    if (rc < 0) {
      close(socket_file_descriptor_);
      error("ioctl() failed");
    }
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(socket_file_descriptor_, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
      close(socket_file_descriptor_);
      error("ERROR on binding");
    }
    rc = listen(socket_file_descriptor_, 32);
    if (rc < 0) {
      close(socket_file_descriptor_);
      error("listen() failed");
    }

    #if __APPLE__
        queue_file_descriptor_ = kqueue();
    #else
        queue_file_descriptor_ = epoll_create1(0);
    #endif
    if (queue_file_descriptor_ == -1) {
        close(socket_file_descriptor_); 
        error("Error on creating epoll/kqueue instance ");
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
    #if __APPLE__
        struct kevent evSet;
        EV_SET(&evSet, socket_file_descriptor_, EVFILT_READ, EV_ADD, 0, 0, NULL);
        assert(-1 != kevent(queue_file_descriptor_, &evSet, 1, NULL, 0, NULL));

        struct kevent evList[32];
    #else
        epoll_event events;
        events.events = EPOLLIN;

        auto status = epoll_ctl(queue_file_descriptor_, EPOLL_CTL_ADD, socket_file_descriptor_, &events);
        if (status == -1) {
            error("Error running epoll_ctl " + std::to_string(errno));
        }
    #endif
    for (int i = 0; i < 10; ++i) {
        threads_.emplace_back(new ClientThread());
    }
    while (true) {
        if (stop_) {
          break;
        }

        #if __APPLE__
            DEBUG("Polling");
            int n = kevent(queue_file_descriptor_, NULL, 0, evList, 32, NULL);
            DEBUG("Events found " + std::to_string(n));
            if (n == -1) {
                error("kevent did not complete successfully " + std::to_string(errno));
                continue;
            }
        #else
            int n = epoll_wait(queue_file_descriptor_, &events, 5, 500);
            if (n == -1) {
                DEBUG("epoll_wait did not complete successfully " + std::to_string(errno));
                continue;
            }
        #endif
        for (int i = 0; i < n; ++i) {
            #if __APPLE__
                int fd = (int)evList[i].ident;
                if (fd == socket_file_descriptor_) {
            #endif
            DEBUG("New Request Thread");
            ClientThread* client_thread = nullptr;
            while (!client_thread) {
                for (int i = 0; i < 10; ++i) {
                    if (!threads_[i]->is_active()) {
                        DEBUG("Running with " + std::to_string(i) + "th thread");
                        client_thread = threads_[i].get();
                        break;
                    }
                }
            }
            DEBUG("INIT");
            client_thread->init(socket_file_descriptor_);
            DEBUG("DONE INIT");

            #if __APPLE__
                }
            #endif
        }
    }
    DEBUG("CLOSING CLIENT THREADS");
    for (int i = 9; i > -1; --i) {
        threads_[i].reset();
    }
}

void Server::stop() {
  stop_ = true;
}

std::thread Server::run_async() {
    return std::thread(&Server::run, this);
}

}  // namespace concorde
