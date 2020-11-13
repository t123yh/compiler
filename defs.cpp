//
// Created by t123yh on 2020/10/30.
//

#include <cassert>
#include <memory>
#include <stdexcept>
#include "defs.h"

std::shared_ptr<intermediate_variable> variable_access_expression::write_intermediate(generation_context &ctx) {
    auto* s = dynamic_cast<variable_symbol*>(ctx.glob.symbols.find_symbol(name->text));
    assert(s != nullptr);
    if (s->definition.array == var_def::ARRAY_1D || s->definition.array == var_def::ARRAY_2D) {
        throw std::logic_error("Array is not implemented");
    } else if (s->definition.array == var_def::CONST) {
        auto out = std::make_shared<temp_intermediate_variable>();
        constant_quadruple q;
        q.value = s->definition.value;
        q.out = out;
        ctx.current_block->quadruples.push_back(std::make_shared<quadruple>(q));
        return out;
    } else if (s->definition.array == var_def::PARAM || s->definition.array == var_def::SCALAR_VAR) {
        return s->var;
    } else {
        throw std::logic_error("WTF?");
    }
}

std::shared_ptr<intermediate_variable> constant_expression::write_intermediate(generation_context &ctx) {
    auto out = std::make_shared<temp_intermediate_variable>();
    constant_quadruple q;
    q.value = this->val;
    q.out = out;
    ctx.current_block->quadruples.push_back(std::make_shared<quadruple>(q));
    return out;
}

std::shared_ptr<intermediate_variable> calculate_expression::write_intermediate(generation_context &ctx) {
    calculate_quadruple c;
    auto out = std::make_shared<temp_intermediate_variable>();
    c.op = this->op;
    c.in1 = this->a->write_intermediate(ctx);
    c.in2 = this->b->write_intermediate(ctx);
    c.out = out;
    ctx.current_block->quadruples.push_back(std::make_shared<quadruple>(c));
    return out;
}

std::shared_ptr<intermediate_variable> calling_expression::write_intermediate(generation_context &ctx) {
    throw std::logic_error("Calling is not implemented");
}

void assignment_statement::write_intermediate(generation_context &ctx) {
    if (da || db) {
        throw std::logic_error("Array access is not implemented");
    }
    auto q = std::make_shared<assign_quadruple>();
    q->in = this->val->write_intermediate(ctx);
    auto* s = dynamic_cast<variable_symbol*>(ctx.glob.symbols.find_symbol(this->identifier->text));
    assert(s != nullptr);
    q->out = s->var;
    ctx.current_block->quadruples.push_back(q);
}

void calling_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("Not implemented");
}

void empty_statement::write_intermediate(generation_context &ctx) {}

void return_statement::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<return_quadruple>();
    q->out = this->val->write_intermediate(ctx);
    ctx.current_block->quadruples.push_back(q);
    ctx.new_block();
}

void print_statement::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<print_quadruple>();
    if (this->has_string) {
        q->str_name = ctx.glob.new_string(this->print_content->text);
    }
    if (this->has_val) {
        q->in = this->print_val->write_intermediate(ctx);
    }
}

void scan_statement::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<scan_quadruple>();
    auto* s = dynamic_cast<variable_symbol*>(ctx.glob.symbols.find_symbol(this->identifier->text));
    assert(s != nullptr);
    q->out = s->var;
    ctx.current_block->quadruples.push_back(q);
}

void block_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("Block not implemented");
}

void if_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("If not implemented");
}

void for_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("For not implemented");
}

void switch_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("Switch not implemented");
}

void while_statement::write_intermediate(generation_context &ctx) {
    throw std::logic_error("While not implemented");
}

void statement_block::populate_variables(generation_context &ctx) {
    for (auto& v : variables) {
        if (v.array == var_def::CONST) {
            ctx.glob.symbols.add_symbol(std::make_shared<variable_symbol>(v));
        } else if (v.array == var_def::SCALAR_VAR) {
            auto symb = std::make_shared<variable_symbol>(v);
            symb->var = std::make_shared<variable_intermediate_variable>();
            ctx.glob.symbols.add_symbol(symb);
        } else {
            throw std::logic_error("Array not impl");
        }
    }
}

void statement_block::generate_statements(generation_context& ctx) {
    for (auto& s : statements) {
        s->write_intermediate(ctx);
    }
}

void function::populate_variables(generation_context &ctx) {
    for (auto& p : signature.parameters) {
        assert(p.array == var_def::PARAM);
        auto symb = std::make_shared<variable_symbol>(p);
        symb->var = std::make_shared<variable_intermediate_variable>();
        ctx.glob.symbols.add_symbol(symb);
    }
}

void program::popluate_global_variables(global_generation_context& ggc) {
    for (auto& v : symbols) {
        if (v.array == var_def::CONST) {
            ggc.symbols.add_symbol(std::make_shared<variable_symbol>(v));
        } else if (v.array == var_def::SCALAR_VAR) {
            auto symb = std::make_shared<variable_symbol>(v);
            symb->var = std::make_shared<global_intermediate_variable>();
            ggc.symbols.add_symbol(symb);
        } else {
            throw std::logic_error("Array not impl");
        }
    }
    
}
