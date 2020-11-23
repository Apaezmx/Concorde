# Concorde

A dead simple and fast web server for C++. Inspired in Python's Flask. Still have issues with high loads.

## Getting Started

The code is as simple as
```
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

```

This will bring up a server on port 2235 which listens for post requests on /login and get requests on /index.html.

### Prerequisites

No prerequisites.

### Installing

Running with the precompiled library, just add the library when compiling.
eg: g++ -std=c++11 -o mymain mymain.cpp -lconcorde

Be sure to include `src/libconcorde.so` in your LD_LIBRARY_PATH. Alternatively you can use the param "-L/my/path/to/libconcorde.so"
to compile your project.

#### Build from scratch

To build the project from scratch clone the repo please first install boost (`brew install boost` on MacOs). 
Then you need to cp the googletest directory:
`cp -r googletest-master googletest`

Then initialize git submodules and update them:
`git submodule init`
`git submodule update`

finally build the project

`make`
`make install`
`make tests`

Optionally you can rebuild the buildsystem using cmake:
`cmake .`

This should generate `src/libconcorde.so` which you can use when linking your code.

#### Tests

Use `ctest` or `make tests && ./tests` to run the tests.

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
