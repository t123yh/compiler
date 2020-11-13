//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_INTERMEDIATE_H
#define COMPILER_INTERMEDIATE_H

#include <memory>
#include "base_defs.h"

struct intermediate_variable
{
    virtual ~intermediate_variable() = default;
};

struct temp_intermediate_variable : intermediate_variable
{
};

struct variable_intermediate_variable : intermediate_variable
{
};

struct global_intermediate_variable : intermediate_variable
{
};

enum quadruple_operation {
    QADD, QSUB, QMUL, QDIV, QLI, QPRINTF, QSCANF
};

struct quadruple
{
    std::shared_ptr<intermediate_variable> out;
    virtual ~quadruple() = default;
};

struct calculate_quadruple : quadruple {
    token_type_t op;
    std::shared_ptr<intermediate_variable> in1, in2;
};

struct constant_quadruple : quadruple {
    int64_t value;
};

struct calling_quadruple : quadruple {
};

struct assign_quadruple : quadruple {
    std::shared_ptr<intermediate_variable> in;
};

struct return_quadruple : quadruple {
    std::shared_ptr<intermediate_variable> in;
};

struct print_quadruple : quadruple {
    std::shared_ptr<intermediate_variable> in;
    std::string str_name;
};

struct scan_quadruple : quadruple {
};

struct quadruple_block {
    std::vector<std::shared_ptr<quadruple>> quadruples;
    std::weak_ptr<quadruple_block> next_block;
    std::weak_ptr<quadruple_block> next_block_2;
};


#endif //COMPILER_INTERMEDIATE_H
