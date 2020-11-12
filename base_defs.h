//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

#include <string>
#include <cstdint>
#include <memory>

#include "tokenizer.h"
typedef token* token_ptr;
typedef std::vector<token>::iterator token_iter;
struct var_def
{
    enum arr_desc_t {
        CONST, SCALAR_VAR, ARRAY_1D, ARRAY_2D, PARAM
    };
    
    typedef token_type_t type_t;
    
    arr_desc_t array;
    type_t type;
    token_ptr identifier;
    int64_t value;
};

struct function_signature
{
    var_def::type_t return_type;
    token_ptr identifier;
    std::vector<var_def> parameters;
};


#endif //COMPILER_DEFS_H
