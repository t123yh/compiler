//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_UTILS_H
#define COMPILER_UTILS_H

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif //COMPILER_UTILS_H
