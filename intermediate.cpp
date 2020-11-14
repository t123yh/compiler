//
// Created by t123yh on 2020/11/12.
//

#include <stdexcept>
#include "intermediate.h"

void calculate_quadruple::generate_mips(std::vector<std::string> &output) {
    // TODO: 优化：如果一个变量是常数，则缩减为一条指令
    
    if (in->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in->stack_offset) + "($sp)");
    }
    if (in2->type == intermediate_variable::constant) {
        output.push_back("li $t9, " + std::to_string(in2->const_value));
    } else {
        output.push_back("lw $t9, " + std::to_string(in2->stack_offset) + "($sp)");
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
    if (in->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in->stack_offset) + "($sp)");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        output.push_back("sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void return_quadruple::generate_mips(std::vector<std::string> &output) {
    throw std::logic_error("No return");
}

void print_quadruple::generate_mips(std::vector<std::string> &output) {
    if (!str_name.empty()) {
        output.push_back("la $a0, " + str_name);
        output.emplace_back("li $v0, 4");
        output.emplace_back("syscall");
    }
    if (this->in) {
        if (in->type == intermediate_variable::constant) {
            output.push_back("li $a0, " + std::to_string(in->const_value));
        } else {
            output.push_back("lw $a0, " + std::to_string(in->stack_offset) + "($sp)");
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
    output.emplace_back("li $a0, 0x20");
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
    if (in->type == intermediate_variable::constant) {
        output.push_back("li $t8, " + std::to_string(in->const_value));
    } else {
        output.push_back("lw $t8, " + std::to_string(in->stack_offset) + "($sp)");
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
