//
// Created by t123yh on 2020/10/30.
//

#include <cassert>
#include <memory>
#include <stdexcept>
#include "defs.h"
#include "parsing_failure.h"

std::shared_ptr<intermediate_variable> variable_access_expression::write_intermediate(generation_context &ctx) {
    auto l = ctx.variables.find(name->text);
    if (l != ctx.variables.end()) {
        if ((this->idx1) || (this->idx2)) {
            throw std::logic_error("Array is not supported");
        }
        return std::get<1>(l->second);
    } else {
        auto g = ctx.glob.variables.find(name->text);
        if (g == ctx.glob.variables.end())
            throw parsing_failure("Variable not found");
        if (g->second.array == var_def::SCALAR_VAR) {
            auto v = ctx.new_temp_var();
            auto q = std::make_shared<global_variable_access_quadruple>(ctx);
            q->out = v;
            q->name = name->text;
            ctx.current_block->quadruples.push_back(q);
            return v;
        } else if (g->second.array == var_def::CONST) {
            auto q = std::make_shared<intermediate_variable>();
            q->type = intermediate_variable::constant;
            q->const_value = g->second.value;
            return q;
        }
    }
    throw std::logic_error("WTF?");
}

std::shared_ptr<intermediate_variable> constant_expression::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<intermediate_variable>();
    q->type = intermediate_variable::constant;
    q->const_value = this->val;
    return q;
}

std::shared_ptr<intermediate_variable> calculate_expression::write_intermediate(generation_context &ctx) {
    auto c = std::make_shared<calculate_quadruple>(ctx);
    auto out = ctx.new_temp_var();
    c->op = this->op;
    c->in_list.push_back(this->a->write_intermediate(ctx));
    c->in_list.push_back(this->b->write_intermediate(ctx));
    c->out = out;
    ctx.current_block->quadruples.push_back(c);
    return out;
}

std::shared_ptr<intermediate_variable> calling_expression::write_intermediate(generation_context &ctx) {
    auto c = std::make_shared<calling_quadruple>(ctx);
    c->function_name = this->call_info->name->text;
    ctx.max_arg_count = std::max(ctx.max_arg_count, (int)this->call_info->arguments.size());
    for (auto& item : this->call_info->arguments) {
        c->in_list.push_back(item->write_intermediate(ctx));
    }
    auto out = ctx.new_temp_var();
    c->out = out;
    ctx.current_block->quadruples.push_back(c);
    return out;
}

static void write(generation_context& ctx, const std::shared_ptr<intermediate_variable>& in, const std::string& vname) {
    auto l = ctx.variables.find(vname);
    if (l != ctx.variables.end()) {
        auto q = std::make_shared<assign_quadruple>(ctx);
        q->out = std::get<1>(l->second);
        q->in_list.push_back(in);
        ctx.current_block->quadruples.push_back(q);
    } else {
        // global bar
        auto g = ctx.glob.variables.find(vname);
        if (g == ctx.glob.variables.end())
            throw parsing_failure("Variable not found");
        if (g->second.array == var_def::SCALAR_VAR) {
            auto q = std::make_shared<global_variable_write_quadruple>(ctx);
            q->in_list.push_back(in);
            q->name = vname;
            ctx.current_block->quadruples.push_back(q);
        } else {
            throw std::logic_error("Not supported");
        }
    }
}

void assignment_statement::write_intermediate(generation_context &ctx) {
    if (idx1 || idx2) {
        throw std::logic_error("Array access is not implemented");
    }
    auto in = this->val->write_intermediate(ctx);
    write(ctx, in, identifier->text);
}

void calling_statement::write_intermediate(generation_context &ctx) {
    auto c = std::make_shared<calling_quadruple>(ctx);
    c->function_name = this->call_info->name->text;
    ctx.max_arg_count = std::max(ctx.max_arg_count, (int)this->call_info->arguments.size());
    for (auto& item : this->call_info->arguments) {
        c->in_list.push_back(item->write_intermediate(ctx));
    }
    ctx.current_block->quadruples.push_back(c);
}

void empty_statement::write_intermediate(generation_context &ctx) {}

void return_statement::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<return_quadruple>(ctx);
    q->in_list.push_back(this->val->write_intermediate(ctx));
    ctx.current_block->quadruples.push_back(q);
    ctx.new_block();
}

void print_statement::write_intermediate(generation_context &ctx) {
    auto q = std::make_shared<print_quadruple>(ctx);
    if (this->has_string) {
        q->str_name = ctx.glob.new_string(this->print_content->text);
    }
    
    if (this->has_val) {
        q->in_list.push_back(this->print_val->write_intermediate(ctx));
        q->type = this->val_type;
    }
    
    ctx.current_block->quadruples.push_back(q);
}

void scan_statement::write_intermediate(generation_context &ctx) {
    auto out = ctx.new_temp_var();
    auto q = std::make_shared<scan_quadruple>(ctx);
    q->out = out;
    q->type = this->val_type;
    ctx.current_block->quadruples.push_back(q);
    write(ctx, out, identifier->text);
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
        ctx.insert_variable(v);
    }
}

void statement_block::generate_statements(generation_context& ctx) {
    for (auto& s : statements) {
        s->write_intermediate(ctx);
    }
    ctx.current_block->quadruples.push_back(std::make_shared<return_quadruple>(ctx));
}

void function::populate_variables(generation_context &ctx) {
    for (int i = 0; i < signature.parameters.size(); i++) {
        auto& p = signature.parameters[i];
        assert(p.array == var_def::PARAM);
        ctx.insert_parameter(p, i);
    }
}

void program::popluate_global_variables(global_generation_context& ggc) {
    for (auto& v : symbols) {
        ggc.variables.insert({v.identifier->text, v});
    }
}
