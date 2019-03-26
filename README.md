# Concorde

A dead simple and fast web server for C++. Inspired in Python's Flask.

## Getting Started

The code is as simple as
```
#include <iostream>
#include "concorde.h"

REGISTER_POST(login, "/index",
              [](const HttpRequest& request) {
                  return "Hola Mundo";
              });

int main(int argc, const char * argv[]) {
    concorde::Server serv(2235);
    serv.run();
    return 0;
}

```

This will bring up a server on port 2236 which listens for post requests on /login and get requests on /index.html.

### Prerequisites

No prerequisites.

### Installing

Running with the precompiled library, just add the library when compiling.
eg: g++ -std=c++11 -o mymain mymain.cpp -lconcorde

Be sure to include libconcorde.so in your LD_LIBRARY_PATH. Alternatively you can use the param "-L/my/path/to/libconcorde.so"
to compile your project.

#### Build from scratch

To build the project from scratch you can use
```
g++ -std=c++11 -c -fpic concorde.cc
```

This will create a *concorde.o* file. You can then run

```
g++ -shared -o libconcorde.so concorde.o
```

To create the library which you then use to build your binary.

## Built With

XCode and / or G++ on Linux.

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Andres Paez MArtinez** - *Initial work* - [Apaezmx](https://github.com/Apaezmx).

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Inspired in Flask. Fast simple and easy to use!
* Using [httpparser](https://github.com/nekipelov/httpparser) by nekipelov 
