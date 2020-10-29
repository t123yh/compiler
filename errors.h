//
// Created by t123yh on 2020/10/29.
//

#ifndef COMPILER_ERRORS_H
#define COMPILER_ERRORS_H

#include <vector>

enum error_type {
    E_INVALID_TOKEN = 'a',
    E_COND_N_CHAR = 'f',
    E_INDEX_N_CHAR = 'i',
    E_SHOULD_BE_SEMICOLON = 'k',
    E_SHOULD_BE_PARENTHESIS = 'l',
    E_SHOULD_BE_BRACKET = 'm',
    E_MISSING_DEFAULT_IN_SWITCH = 'p',
};

struct error
{
    int line;
    error_type type;
    
    bool operator < (const error &other) const
    {
        return this->line < other.line;
    }
};

typedef std::vector<error> error_container;


#endif //COMPILER_ERRORS_H
