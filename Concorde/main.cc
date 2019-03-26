//
//  main.cpp
//  Concorde
//
//  Created by Andres Paez Martinez on 2/14/19.
//  Copyright © 2019 Andres Paez Martinez. All rights reserved.
//

#include <iostream>
#include "concorde.h"

REGISTER_METHOD(login, "/index",
              [](const HttpRequest& request) {
                  return "Hola Mundo";
              });

int main(int argc, const char * argv[]) {
    concorde::Server serv(2235);
    serv.run();
    return 0;
}
