//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

#include <string>
#include <cstdint>
#include <memory>

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

struct function_parameter {
    var_def::type_t type;
    token_ptr name;
};

struct function_signature
{
    var_def::type_t return_type;
    token_ptr identifier;
    std::vector<function_parameter> parameters;
};

struct expression {
};

struct variable_access_expression : public expression {
    token_ptr name;
    std::unique_ptr<expression> idx1, idx2;
};

struct constant_expression : public expression {
    constant_expression(int64_t v) : val(v) {}
    int64_t val;
};

struct calculate_expression : public expression {
    enum op_t {
        T_MUL, T_DIV, T_PLUS, T_MINUS
    };
    std::unique_ptr<expression> a, b;
    op_t op;
};

struct function_call_info {
    token_ptr name;
    std::vector<std::unique_ptr<expression>> arguments;
};

struct calling_expression : public expression {
    std::unique_ptr<function_call_info> call_info;
};

struct statement {

};

struct assignment_statement : public statement {
    token_ptr identifier;
    std::unique_ptr<expression> da, db, val;
};

struct statement_block {
    std::vector<var_def> variables;
    std::vector<std::unique_ptr<statement>> statements;
};

struct function {
    function_signature signature;
    statement_block statements;
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
            return (var_def::type_t)0;
    }
}

#endif //COMPILER_DEFS_H
