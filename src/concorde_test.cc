//
//  concorde_test.cpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#include <unistd.h>
#include <poll.h>
#include <sys/ioctl.h>

#include <future>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "httpparser/httprequestparser.h"
#include "httpparser/request.h"
#include "httpparser/response.h"
#include "concorde.h"

// using curlpp::options;

namespace concorde {

REGISTER_METHOD(Hello, "/hello",
                [](const HttpRequest& request) {
                  return "hello";
                });

REGISTER_METHOD(login, "/index",
                [](const HttpRequest& request) {
                  std::string s = "";
                  for (const auto& header : request.headers) {
                    s += header.name + ": " + header.value + "\n";
                  }
                  s += std::string(request.content.begin(),
                    request.content.end());
                  return s;
                });

int PortGen() {
  static int port = 15295;
  return port++;
}

TEST(DefaultTest, ServerConstructorDoesntCrash) {
  int port = PortGen();
  Server server(port);
}

// Brings up an actual server to test against.
class ServerTests : public ::testing::Test {
 protected:
  void SetUp() override {
    test_port_ = PortGen();
    std::cout << "New server at " << test_port_ << std::endl;
    server_ = std::unique_ptr<Server>(new Server(test_port_));
    thread_ = server_->run_async();
    // Wait for server to start.
  }

  void TearDown() override {
    // We need to send a signal to break out of the poll loop
    server_->stop();
    std::ostringstream os;
    std::string s = "http://localhost:" + std::to_string(test_port_) + "/hello";
    std::cerr << "Quit signal\n";
    os << curlpp::options::Url(s);
    std::cerr << "Quit signal sent\n";
    thread_.join();
  }

  int test_port_;
  std::thread thread_;
  std::unique_ptr<Server> server_;
};

TEST_F(ServerTests, GetRequestHappyPath) {
  curlpp::Cleanup myCleanup;
  {
    std::ostringstream os;
    std::string s = "http://localhost:" + std::to_string(test_port_) + "/hello";
    os << curlpp::options::Url(s);
    EXPECT_EQ("hello", os.str());
  }
}
}  // namespace concorde.
