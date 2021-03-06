//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_UTILS_H
#define COMPILER_UTILS_H
#include <memory>
#include <string>
#include <algorithm>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

static inline std::string str_to_lower(std::string data) {
    std::for_each(data.begin(), data.end(), [](char & c){
        c = ::tolower(c);
    });
    return data;
}

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE_NAME TOKENPASTE2(__uniq__, __LINE__)

#endif //COMPILER_UTILS_H
