//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_INTERMEDIATE_H
#define COMPILER_INTERMEDIATE_H

#include <memory>
#include "base_defs.h"

struct generation_context;

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
    int parameter_index;
    int64_t const_value;
};

struct global_variable
{
    virtual ~global_variable() = default;
    std::string name;
};

struct quadruple
{
    generation_context& ctx;
    quadruple(generation_context& ctx) : ctx(ctx) {}
    std::shared_ptr<intermediate_variable> out;
    std::vector<std::shared_ptr<intermediate_variable>> in_list;
    virtual void generate_mips(std::vector<std::string>& output) = 0;
    virtual ~quadruple() = default;
};

struct calculate_quadruple : quadruple {
    token_type_t op;
    calculate_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct global_variable_access_quadruple : quadruple {
    std::string name;
    global_variable_access_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct global_variable_write_quadruple : quadruple {
    std::string name;
    global_variable_write_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct calling_quadruple : quadruple {
    std::string function_name;
    calling_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct assign_quadruple : quadruple {
    assign_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};


struct print_quadruple : quadruple {
    std::string str_name;
    token_type_t type;
    print_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct scan_quadruple : quadruple {
    token_type_t type;
    scan_quadruple(generation_context& ctx) : quadruple(ctx){}
    void generate_mips(std::vector<std::string> &output) override;
};

struct quadruple_block;
struct exit_op {
    generation_context& ctx;
    std::vector<std::shared_ptr<intermediate_variable>> in_list;
    exit_op(generation_context& ctx): ctx(ctx){}
    virtual void generate_mips(quadruple_block& blk, std::vector<std::string> &output) = 0;
};

struct condition_exit : exit_op {
    condition_exit(generation_context& ctx) : exit_op(ctx){}
    token_type_t comparator;
    void generate_mips(quadruple_block& blk, std::vector<std::string> &output) override;
};

struct switch_exit : exit_op {
    switch_exit(generation_context& ctx) : exit_op(ctx){}
    std::vector<int64_t> value_table;
    void generate_mips(quadruple_block& blk, std::vector<std::string> &output) override;
};

struct jump_exit : exit_op {
    jump_exit(generation_context& ctx) : exit_op(ctx){}
    void generate_mips(quadruple_block& blk, std::vector<std::string> &output) override;
};

struct return_exit : exit_op {
    return_exit(generation_context& ctx) : exit_op(ctx){}
    void generate_mips(quadruple_block& blk, std::vector<std::string> &output) override;
};

struct quadruple_block {
    std::string block_name;
    std::vector<std::shared_ptr<quadruple>> quadruples;
    std::vector<std::weak_ptr<quadruple_block>> next_blocks;
    std::shared_ptr<exit_op> eop;
    
    generation_context& ctx;
    
    quadruple_block(generation_context& ctx) : ctx(ctx){}
    
    void generate_mips(std::vector<std::string>& file);
};


#endif //COMPILER_INTERMEDIATE_H
