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
    
    typedef token_type_t type_t;
    
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
    typedef token_type_t op_t;
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

struct calling_statement : public statement {
    std::unique_ptr<function_call_info> call_info;
    calling_statement(std::unique_ptr<function_call_info>&& call_info) : call_info(std::move(call_info)) {}
};

struct empty_statement : public statement {

};

struct return_statement: public statement {
    std::unique_ptr<expression> val;
};

struct print_statement: public statement {
    bool has_string, has_val;
    token_ptr print_content;
    std::unique_ptr<expression> print_val;
};

struct scan_statement : public statement {
    token_ptr identifier;
};

struct block_statement : public statement {
    std::vector<std::unique_ptr<statement>> statements;
    block_statement(std::vector<std::unique_ptr<statement>> && st) : statements(std::move(st)) {}
};

struct statement_block {
    std::vector<var_def> variables;
    std::vector<std::unique_ptr<statement>> statements;
};

struct function {
    function_signature signature;
    statement_block statements;
};

struct condition {
    std::unique_ptr<expression> exp1, exp2;
    token_type_t comparator;
};

struct if_statement : public statement {
    condition cond;
    std::unique_ptr<statement> if_body, else_body;
};

struct while_statement : public statement {
    condition cond;
    std::unique_ptr<statement> body;
};

struct for_statement : public statement {
    token_ptr initial_var;
    std::unique_ptr<expression> initial_exp;
    condition cond;
    
    token_ptr step_set_var;
    token_ptr step_get_var;
    int64_t step_len;
    
    std::unique_ptr<statement> body;
};

struct switch_cond : public statement {
    int64_t val;
    std::unique_ptr<statement> body;
};

struct switch_statement : public statement {
    std::unique_ptr<expression> exp;
    std::vector<switch_cond> conditions;
    std::unique_ptr<statement> default_body;
};

#endif //COMPILER_DEFS_H
