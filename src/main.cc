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
    if (argc != 2) {
      std::cout << "One argument (port) is needed for Concorde to run." << std::endl;
      return 1;
    }
    concorde::Server serv(atoi(argv[1]));
    serv.run();
    return 0;
}
