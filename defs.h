//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

#include <string>
#include <cstdint>

#include "tokenizer.h"
typedef std::vector<token>::iterator token_ptr;
struct var_def
{
    enum arr_desc_t {
        CONST, SCALAR_VAR, ARRAY_1D, ARRAY_2D
    };
    
    enum type_t
    {
        INT, CHAR
    };
    
    arr_desc_t array;
    type_t type;
    token_ptr identifier;
    int64_t value;
};

#endif //COMPILER_DEFS_H
