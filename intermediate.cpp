//
// Created by t123yh on 2020/11/12.
//

#include <stdexcept>
#include "intermediate.h"
#include "generation.h"

void calculate_quadruple::generate_mips(std::vector<std::string> &output) {
    // TODO: 优化：如果一个变量是常数，则缩减为一条指令
    
    if (in_list[0]->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    if (in_list[1]->type == intermediate_variable::constant) {
        output.push_back("li $t9, " + std::to_string(in_list[1]->const_value));
    } else {
        output.push_back("lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
    }
    
    switch (this->op) {
        case PLUS: {
            output.emplace_back("add $t8, $t8, $t9");
        }
            break;
        case MINU: {
            output.emplace_back("sub $t8, $t8, $t9");
        }
            break;
        case MULT: {
            output.emplace_back("mul $t8, $t8, $t9");
        }
            break;
        case DIV:
            output.emplace_back("div $t8, $t8, $t9");
            break;
        default:
            throw std::logic_error("Wrong op");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot write to const");
    } else {
        output.push_back("sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void assign_quadruple::generate_mips(std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        output.push_back("sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void return_quadruple::generate_mips(std::vector<std::string> &output) {
    if (this->in_list.size() > 0) {
        if (in_list[0]->type == intermediate_variable::constant) {
            output.push_back("li $v0, " + std::to_string(in_list[0]->const_value));
        } else {
            output.push_back("lw $v0, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
    }
    output.push_back("lw $ra, " + std::to_string(ctx.save_ra_depth()) + "($sp)");
    output.push_back("addiu $sp, $sp, " + std::to_string(ctx.stack_depth()));
    output.emplace_back("jr $ra");
}

void print_quadruple::generate_mips(std::vector<std::string> &output) {
    if (!str_name.empty()) {
        output.push_back("la $a0, " + str_name);
        output.emplace_back("li $v0, 4");
        output.emplace_back("syscall");
    }
    if (this->in_list.size() > 0) {
        if (in_list[0]->type == intermediate_variable::constant) {
            output.push_back("li $a0, " + std::to_string(in_list[0]->const_value));
        } else {
            output.push_back("lw $a0, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
        if (type == CHARTK) {
            output.emplace_back("li $v0, 11");
        } else if (type == INTTK) {
            output.emplace_back("li $v0, 1");
        } else {
            throw std::logic_error("No type!!");
        }
        output.emplace_back("syscall");
    }
    output.emplace_back("li $a0, 10");
    output.emplace_back("li $v0, 11");
    output.emplace_back("syscall");
}

void scan_quadruple::generate_mips(std::vector<std::string> &output) {
    if (type == CHARTK) {
        output.emplace_back("li $v0, 12");
    } else if (type == INTTK) {
        output.emplace_back("li $v0, 5");
    } else {
        throw std::logic_error("No type!!");
    }
    output.emplace_back("syscall");
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        output.push_back("sw $v0, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void global_variable_write_quadruple::generate_mips(std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    output.push_back("sw $t8, " + this->name);
}

void global_variable_access_quadruple::generate_mips(std::vector<std::string> &output) {
    output.push_back("lw $t8, " + this->name);
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        output.push_back("sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void quadruple_block::generate_mips(std::vector<std::string> &file) {
    for (auto& i : quadruples) {
        i->generate_mips(file);
    }
}

void calling_quadruple::generate_mips(std::vector<std::string> &output) {
    for (int i = 0; i < this->in_list.size(); i++) {
        if (i < 4) {
            if (in_list[i]->type == intermediate_variable::constant) {
                output.push_back("li $a" + std::to_string(i) + ", " + std::to_string(in_list[i]->const_value));
            } else {
                output.push_back("lw $a" + std::to_string(i) + ", " + std::to_string(in_list[i]->stack_offset) + "($sp)");
            }
        } else {
            if (in_list[i]->type == intermediate_variable::constant) {
                output.push_back("li $t8, " + std::to_string(in_list[i]->const_value));
            } else {
                output.push_back("lw $t8, " + std::to_string(in_list[i]->stack_offset) + "($sp)");
            }
            output.push_back("sw $t8, " + std::to_string(i * 4) + "($sp)");
        }
    }
    
    output.push_back("jal func_" + this->function_name);
    
    if (this->out) {
        output.push_back("sw $v0, " + std::to_string(this->out->stack_offset) + "($sp)");
    }
}
