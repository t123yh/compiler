//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_PARSING_FAILURE_H
#define COMPILER_PARSING_FAILURE_H

#include <exception>
#include <string>

class parsing_failure : public std::exception
{
public:
    parsing_failure(std::string why);
};


#endif //COMPILER_PARSING_FAILURE_H
