//
//  main.cpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#include <iostream>
#include "concorde.h"
#include "request.h"

std::string login(const ::httpparser::Request& req) {
    return "Hola Mundo";
}

std::string confirm_user(const ::httpparser::Request& req) {
    return "";
}

int main(int argc, const char * argv[]) {
    concorde::Server serv(2236);
    serv.register_post("/login", login);
    serv.register_get("/index.html", confirm_user);
    serv.run();
    return 0;
}
