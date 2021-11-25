#ifndef LOGGER_H
#define LOGGER_H
#define DEBUG_MODE true
#include <iostream>

class logger {
public:
    logger();
    ~logger();
//    template<class T>
//    logger &operator<<(const T &msg);
    template<class T>
    logger& operator<<(const T &msg) {
        if(DEBUG_MODE) {
            std::cout << msg;
        }
        return *this;
    }
};

#endif
