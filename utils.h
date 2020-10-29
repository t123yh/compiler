//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_UTILS_H
#define COMPILER_UTILS_H
#include <memory>
#include <string>
#include <algorithm>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

static inline std::string str_to_lower(std::string data) {
    std::for_each(data.begin(), data.end(), [](char & c){
        c = ::tolower(c);
    });
    return data;
}

#endif //COMPILER_UTILS_H
