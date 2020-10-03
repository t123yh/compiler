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
        INT = 1, CHAR, VOID
    };
    
    arr_desc_t array;
    type_t type;
    token_ptr identifier;
    int64_t value;
};

static inline var_def::type_t token2type(token_ptr tk) {
    switch (tk->type) {
        case INTTK:
            return var_def::INT;
        case CHARTK:
            return var_def::CHAR;
        case VOIDTK:
            return var_def::VOID;
        default:
            return 0;
    }
}

#endif //COMPILER_DEFS_H
