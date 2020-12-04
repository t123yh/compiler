//
// Created by t123yh on 2020/11/12.
//

#include <stdexcept>
#include <cassert>
#include "generation.h"

void generation_context::new_block() {
    block_id++;
    auto nb = std::make_shared<quadruple_block>(*this);
    nb->block_name = this->name + "_b_" + std::to_string(block_id);
    this->blocks.push_back(nb);
    this->current_block = nb;
}

std::shared_ptr<intermediate_variable> generation_context::new_temp_var() {
    auto out = std::make_shared<intermediate_variable>();
    out->type = intermediate_variable::temp;
    temp_vars.push_back(out);
    return out;
}

void generation_context::insert_parameter(var_def v, int i) {
    auto out = std::make_shared<intermediate_variable>();
    out->type = intermediate_variable::parameter;
    out->parameter_index = i;
    variables.insert({v.identifier->text, std::make_tuple(v, out)});
}

void generation_context::insert_variable(var_def v) {
    auto out = std::make_shared<intermediate_variable>();
    if (v.array == var_def::CONST) {
        out->type = intermediate_variable::constant;
        out->const_value = v.value;
    } else if (v.array == var_def::SCALAR_VAR) {
        out->type = intermediate_variable::local;
        auto ass = std::make_shared<assign_quadruple>(*this);
        ass->in_list.push_back(std::make_shared<intermediate_variable>());
        ass->in_list[0]->type = intermediate_variable::constant;
        ass->in_list[0]->const_value = v.value;
        ass->out = out;
        current_block->quadruples.push_back(ass);
    } else {
        throw std::logic_error("Not supported");
    }
    variables.insert({v.identifier->text, std::make_tuple(v, out)});
}

void generation_context::assign_stack_space() {
    local_variable_count = temp_vars.size();
    for (auto &v : variables) {
        if (std::get<1>(v.second)->type == intermediate_variable::local) {
            switch (std::get<0>(v.second).array) {
                case var_def::SCALAR_VAR:
                    local_variable_count += 1;
                    break;
                default:
                    throw std::logic_error("FUCKED");
            }
        }
    }
    
    int idx = 0;
    for (auto &v : variables) {
        auto t = std::get<1>(v.second)->type;
        if (t == intermediate_variable::local) {
            switch (std::get<0>(v.second).array) {
                case var_def::SCALAR_VAR:
                    std::get<1>(v.second)->stack_offset = get_sp_offset(idx);
                    idx++;
                    break;
                default:
                    throw std::logic_error("FUCKED");
            }
        } else if (t == intermediate_variable::parameter) {
            std::get<1>(v.second)->stack_offset = stack_depth() + std::get<1>(v.second)->parameter_index * 4;
        } else {
            throw std::logic_error("FUCKED");
        }
    }
    for (auto &v : temp_vars) {
        v->stack_offset = get_sp_offset(idx);
        idx++;
    }
}

void generation_context::generate_mips(std::vector<std::string> &result) {
    result.push_back("func_" + this->name + ":");
    result.push_back("addiu $sp, $sp, -" + std::to_string(this->stack_depth()));
    result.push_back("sw $ra, " + std::to_string(this->save_ra_depth()) + "($sp)");
    for (int i = 0; i < 4; i++) {
        int depth = this->stack_depth() + i * 4;
        result.push_back("sw $a" + std::to_string(i) + ", " + std::to_string(depth) + "($sp)");
    }
    for (auto &block : blocks) {
        result.push_back(block->block_name + ":");
        block->generate_mips(result);
    }
}

int generation_context::get_sp_offset(int v) {
    return max_arg_count * 4 + 4 + save_reg_count * 4 + v * 4;
}

int generation_context::stack_depth() {
    assert(local_variable_count != -1);
    return local_variable_count * 4
           + 4  // return address
           + save_reg_count * 4
           + max_arg_count * 4;
}

int generation_context::save_ra_depth() {
    return max_arg_count * 4 + save_reg_count * 4;
}

std::string global_generation_context::new_string(std::string content) {
    string_id++;
    string_table.emplace_back(std::string("str" + std::to_string(string_id)), content);
    return std::get<0>(string_table.back());
}

void global_generation_context::insert_variable(var_def v) {
    auto out = std::make_shared<global_variable>();
    variables.insert({v.identifier->text, v});
}

static const char* mips_reg_table[] = {
        [0] = "zero",
        [1] = "at",
        [2] = "v0",
        [3] = "v1",
        [4] = "a0",
        [5] = "a1",
        [6] = "a2",
        [7] = "a3",
        [8] = "t0",
        [9] = "t1",
        [10] = "t2",
        [11] = "t3",
        [12] = "t4",
        [13] = "t5",
        [14] = "t6",
        [15] = "t7",
        [16] = "s0",
        [17] = "s1",
        [18] = "s2",
        [19] = "s3",
        [20] = "s4",
        [21] = "s5",
        [22] = "s6",
        [23] = "s7",
        [24] = "t8",
        [25] = "t9",
        [26] = "k0",
        [27] = "k1",
        [28] = "gp",
        [29] = "sp",
        [30] = "fp",
        [31] = "ra",
};

const char* reg_to_string(mips_reg_t reg) {
    return mips_reg_table[reg];
}
