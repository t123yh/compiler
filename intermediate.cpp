//
// Created by t123yh on 2020/11/12.
//

#include <stdexcept>
#include <cassert>
#include "intermediate.h"
#include "generation.h"

void calculate_quadruple::generate_mips(std::vector<std::string> &output) {
    // TODO: 优化：如果一个变量是常数，则缩减为一条指令
    
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    if (in_list[1]->type == intermediate_variable::constant) {
        write_output(output, "li $t9, " + std::to_string(in_list[1]->const_value));
    } else {
        write_output(output, "lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
    }
    
    switch (this->op) {
        case PLUS: {
            write_output(output, "addu $t8, $t8, $t9");
        }
            break;
        case MINU: {
            write_output(output, "subu $t8, $t8, $t9");
        }
            break;
        case MULT: {
            write_output(output, "mul $t8, $t8, $t9");
        }
            break;
        case DIV:
            write_output(output, "div $t8, $t8, $t9");
            break;
        default:
            throw std::logic_error("Wrong op");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot write to const");
    } else {
        write_output(output, "sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void assign_quadruple::generate_mips(std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        write_output(output, "sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void print_quadruple::generate_mips(std::vector<std::string> &output) {
    if (!str_name.empty()) {
        write_output(output, "la $a0, " + str_name);
        write_output(output, "li $v0, 4");
        write_output(output, "syscall");
    }
    if (this->in_list.size() > 0) {
        if (in_list[0]->type == intermediate_variable::constant) {
            write_output(output, "li $a0, " + std::to_string(in_list[0]->const_value));
        } else {
            write_output(output, "lw $a0, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
        if (type == CHARTK) {
            write_output(output, "li $v0, 11");
        } else if (type == INTTK) {
            write_output(output, "li $v0, 1");
        } else {
            throw std::logic_error("No type!!");
        }
        write_output(output, "syscall");
    }
    write_output(output, "li $a0, 10");
    write_output(output, "li $v0, 11");
    write_output(output, "syscall");
}

void scan_quadruple::generate_mips(std::vector<std::string> &output) {
    if (type == CHARTK) {
        write_output(output, "li $v0, 12");
    } else if (type == INTTK) {
        write_output(output, "li $v0, 5");
    } else {
        throw std::logic_error("No type!!");
    }
    write_output(output, "syscall");
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    } else {
        write_output(output, "sw $v0, " + std::to_string(out->stack_offset) + "($sp)");
    }
}

void global_variable_write_quadruple::generate_mips(std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t7, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t7, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    
    if (arr == var_def::SCALAR_VAR) {
        write_output(output, "sw $t7, " + this->name);
    } if (arr == var_def::ARRAY_1D) {
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "sw $t7, " + this->name + "($t8)");
    } else if (arr == var_def::ARRAY_2D) {
        if (in_list[2]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[2]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[2]->stack_offset) + "($sp)");
        }
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t9, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "mul $t9, $t9, " + std::to_string(dimen));
        write_output(output, "addu $t8, $t9, $t8");
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "sw $t7, " + this->name + "($t8)");
    }
}

void global_variable_access_quadruple::generate_mips(std::vector<std::string> &output) {
    if (arr == var_def::SCALAR_VAR) {
        write_output(output, "lw $t8, " + this->name);
    } else if (arr == var_def::ARRAY_1D) {
        if (in_list[0]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[0]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "lw $t8, " + this->name + "($t8)");
    } else if (arr == var_def::ARRAY_2D) {
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        if (in_list[0]->type == intermediate_variable::constant) {
            write_output(output, "li $t9, " + std::to_string(in_list[0]->const_value));
        } else {
            write_output(output, "lw $t9, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
        write_output(output, "mul $t9, $t9, " + std::to_string(dimen));
        write_output(output, "addu $t8, $t9, $t8");
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "lw $t8, " + this->name + "($t8)");
    }
    
    if (out->type == intermediate_variable::constant) {
        throw std::logic_error("Cannot assign to const");
    }
    write_output(output, "sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
}

void quadruple_block::generate_mips(std::vector<std::string> &file) {
    for (auto& i : quadruples) {
        i->generate_mips(file);
    }
    assert(this->eop);
    this->eop->generate_mips(*this, file);
}

void calling_quadruple::generate_mips(std::vector<std::string> &output) {
    for (int i = 0; i < this->in_list.size(); i++) {
        if (i < 4) {
            if (in_list[i]->type == intermediate_variable::constant) {
                write_output(output, "li $a" + std::to_string(i) + ", " + std::to_string(in_list[i]->const_value));
            } else {
                write_output(output, "lw $a" + std::to_string(i) + ", " + std::to_string(in_list[i]->stack_offset) + "($sp)");
            }
        } else {
            if (in_list[i]->type == intermediate_variable::constant) {
                write_output(output, "li $t8, " + std::to_string(in_list[i]->const_value));
            } else {
                write_output(output, "lw $t8, " + std::to_string(in_list[i]->stack_offset) + "($sp)");
            }
            write_output(output, "sw $t8, " + std::to_string(i * 4) + "($sp)");
        }
    }
    
    write_output(output, "jal func_" + this->function_name);
    
    if (this->out) {
        write_output(output, "sw $v0, " + std::to_string(this->out->stack_offset) + "($sp)");
    }
}

void condition_exit::generate_mips(quadruple_block& blk, std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    if (in_list[1]->type == intermediate_variable::constant) {
        write_output(output, "li $t9, " + std::to_string(in_list[1]->const_value));
    } else {
        write_output(output, "lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
    }
    
    std::string instr;
    switch (this->comparator) {
        case EQL:
            instr = "beq";
            break;
        case NEQ:
            instr = "bne";
            break;
        case LEQ:
            instr = "ble";
            break;
        case LSS:
            instr = "blt";
            break;
        case GEQ:
            instr = "bge";
            break;
        case GRE:
            instr = "bgt";
            break;
        default:
            throw std::logic_error("Invalid comparator");
    }
    write_output(output, instr + " $t8, $t9, " + pass_block.lock()->block_name);
    write_output(output, "j " + fail_block.lock()->block_name);
}

std::vector<std::weak_ptr<quadruple_block>> condition_exit::get_next_blocks() {
    return {pass_block, fail_block};
}

void return_exit::generate_mips(quadruple_block &blk, std::vector<std::string> &output) {
    if (in_list.size() > 0) {
        if (in_list[0]->type == intermediate_variable::constant) {
            write_output(output, "li $v0, " + std::to_string(in_list[0]->const_value));
        } else {
            write_output(output, "lw $v0, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
        }
    }
    write_output(output, "lw $ra, " + std::to_string(ctx.save_ra_depth()) + "($sp)");
    write_output(output, "addiu $sp, $sp, " + std::to_string(ctx.stack_depth()));
    write_output(output, "jr $ra");
}

std::vector<std::weak_ptr<quadruple_block>> return_exit::get_next_blocks() {
    return {};
}

void jump_exit::generate_mips(quadruple_block &blk, std::vector<std::string> &output) {
    write_output(output, "j " + next_block.lock()->block_name);
}

std::vector<std::weak_ptr<quadruple_block>> jump_exit::get_next_blocks() {
    return {next_block};
}

std::vector<std::weak_ptr<quadruple_block>> switch_exit::get_next_blocks() {
    std::vector<std::weak_ptr<quadruple_block>> ret;
    for (auto& b : value_table) {
        ret.push_back(b.second);
    }
    ret.push_back(default_block);
    return ret;
}

void switch_exit::generate_mips(quadruple_block &blk, std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t8, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    
    for (auto& c : value_table) {
        write_output(output, "beq $t8, " + std::to_string(c.first) + ", " + c.second.lock()->block_name);
    }
    write_output(output, "j " + default_block.lock()->block_name);
}

void local_array_access_quadruple::generate_mips(std::vector<std::string> &output) {
    if (arr == var_def::ARRAY_1D) {
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "addu $t8, $t8, $sp");
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($t8)");
    } else if (arr == var_def::ARRAY_2D) {
        if (in_list[2]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[2]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[2]->stack_offset) + "($sp)");
        }
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t9, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "mul $t9, $t9, " + std::to_string(dimen));
        write_output(output, "addu $t8, $t9, $t8");
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "addu $t8, $t8, $sp");
        write_output(output, "lw $t8, " + std::to_string(in_list[0]->stack_offset) + "($t8)");
    } else {
        throw std::logic_error("written some shit?");
    }
    
    write_output(output, "sw $t8, " + std::to_string(out->stack_offset) + "($sp)");
}

void local_array_write_quadruple::generate_mips(std::vector<std::string> &output) {
    if (in_list[0]->type == intermediate_variable::constant) {
        write_output(output, "li $t7, " + std::to_string(in_list[0]->const_value));
    } else {
        write_output(output, "lw $t7, " + std::to_string(in_list[0]->stack_offset) + "($sp)");
    }
    
    if (arr == var_def::ARRAY_1D) {
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "addu $t8, $t8, $sp");
        write_output(output, "sw $t7, " + std::to_string(out->stack_offset) + "($t8)");
    } else if (arr == var_def::ARRAY_2D) {
        if (in_list[2]->type == intermediate_variable::constant) {
            write_output(output, "li $t8, " + std::to_string(in_list[2]->const_value));
        } else {
            write_output(output, "lw $t8, " + std::to_string(in_list[2]->stack_offset) + "($sp)");
        }
        if (in_list[1]->type == intermediate_variable::constant) {
            write_output(output, "li $t9, " + std::to_string(in_list[1]->const_value));
        } else {
            write_output(output, "lw $t9, " + std::to_string(in_list[1]->stack_offset) + "($sp)");
        }
        write_output(output, "mul $t9, $t9, " + std::to_string(dimen));
        write_output(output, "addu $t8, $t9, $t8");
        write_output(output, "sll $t8, $t8, 2");
        write_output(output, "addu $t8, $t8, $sp");
        write_output(output, "sw $t7, " + std::to_string(out->stack_offset) + "($t8)");
    } else {
        throw std::logic_error("written some shit?");
    }
}

void quadruple::write_output(std::vector<std::string> &output, const std::string& text) {
    if (line != -1) {
        output.push_back(text + "    # line: " + std::to_string(line));
    } else {
        output.push_back(text);
    }
}

void exit_op::write_output(std::vector<std::string> &output, const std::string &text) {
    if (line != -1) {
        output.push_back(text + "    # line: " + std::to_string(line));
    } else {
        output.push_back(text);
    }
}
