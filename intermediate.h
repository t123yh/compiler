//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_INTERMEDIATE_H
#define COMPILER_INTERMEDIATE_H

#include <memory>
#include "base_defs.h"

enum mips_reg_t {
    reg_zero = 0,
    reg_at = 1,
    reg_v0 = 2,
    reg_v1 = 3,
    reg_a0 = 4,
    reg_a1 = 5,
    reg_a2 = 6,
    reg_a3 = 7,
    reg_t0 = 8,
    reg_t1 = 9,
    reg_t2 = 10,
    reg_t3 = 11,
    reg_t4 = 12,
    reg_t5 = 13,
    reg_t6 = 14,
    reg_t7 = 15,
    reg_s0 = 16,
    reg_s1 = 17,
    reg_s2 = 18,
    reg_s3 = 19,
    reg_s4 = 20,
    reg_s5 = 21,
    reg_s6 = 22,
    reg_s7 = 23,
    reg_t8 = 24,
    reg_t9 = 25,
    reg_k0 = 26,
    reg_k1 = 27,
    reg_gp = 28,
    reg_sp = 29,
    reg_fp = 30,
    reg_ra = 31,
};

const char* reg_to_string(mips_reg_t reg);

struct intermediate_variable
{
    virtual ~intermediate_variable() = default;
    enum type_t {local, temp, parameter, constant};
    type_t type;
    int stack_offset;
    int64_t const_value;
};

struct global_variable
{
    virtual ~global_variable() = default;
    std::string name;
};

struct quadruple
{
    std::shared_ptr<intermediate_variable> out;
    std::shared_ptr<intermediate_variable> in, in2;
    virtual void generate_mips(std::vector<std::string>& output) = 0;
    virtual ~quadruple() = default;
};

struct calculate_quadruple : quadruple {
    token_type_t op;
    void generate_mips(std::vector<std::string> &output) override;
};

struct global_variable_access_quadruple : quadruple {
    std::string name;
    void generate_mips(std::vector<std::string> &output) override;
};

struct global_variable_write_quadruple : quadruple {
    std::string name;
    void generate_mips(std::vector<std::string> &output) override;
};

struct calling_quadruple : quadruple {
};

struct assign_quadruple : quadruple {
    void generate_mips(std::vector<std::string> &output) override;
};

struct return_quadruple : quadruple {
    void generate_mips(std::vector<std::string> &output) override;
};

struct print_quadruple : quadruple {
    std::string str_name;
    token_type_t type;
    void generate_mips(std::vector<std::string> &output) override;
};

struct scan_quadruple : quadruple {
    token_type_t type;
    void generate_mips(std::vector<std::string> &output) override;
};

struct quadruple_block {
    std::string block_name;
    std::vector<std::shared_ptr<quadruple>> quadruples;
    std::weak_ptr<quadruple_block> next_block;
    std::weak_ptr<quadruple_block> next_block_2;
    
    void generate_mips(std::vector<std::string>& file);
};


#endif //COMPILER_INTERMEDIATE_H
