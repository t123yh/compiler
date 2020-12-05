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
        auto& def = std::get<0>(l->second);
        auto& iv = std::get<1>(l->second);
        if (def.array == var_def::ARRAY_2D || def.array == var_def::ARRAY_1D) {
            auto v = ctx.new_temp_var();
            auto a = std::make_shared<local_array_access_quadruple>(ctx);
            a->in_list.push_back(iv);
            a->arr = def.array;
            std::shared_ptr<intermediate_variable> im1, im2;
            assert(this->idx1);
            im1 = this->idx1->write_intermediate(ctx);
            a->in_list.push_back(im1);
            a->out = v;
            if (def.array == var_def::ARRAY_2D) {
                a->dimen = def.dimen2;
                assert(this->idx2);
                im2 = this->idx2->write_intermediate(ctx);
                a->in_list.push_back(im2);
            }
            ctx.current_block->quadruples.push_back(a);
            return v;
        } else {
            return iv;
        }
    } else {
        auto g = ctx.glob.variables.find(name->text);
        if (g == ctx.glob.variables.end())
            throw parsing_failure("Variable not found");
        if (g->second.array == var_def::SCALAR_VAR || g->second.array == var_def::ARRAY_1D || g->second.array == var_def::ARRAY_2D) {
            auto v = ctx.new_temp_var();
            auto q = std::make_shared<global_variable_access_quadruple>(ctx);
            q->out = v;
            q->arr = g->second.array;
            q->name = name->text;
            if (g->second.array == var_def::ARRAY_1D || g->second.array == var_def::ARRAY_2D) {
                assert(this->idx1);
                auto im1 = this->idx1->write_intermediate(ctx);
                q->in_list.push_back(im1);
            }
            if (g->second.array == var_def::ARRAY_2D) {
                q->dimen = g->second.dimen2;
                assert(this->idx2);
                auto im2 = this->idx2->write_intermediate(ctx);
                q->in_list.push_back(im2);
            }
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

static void write(generation_context& ctx, const std::shared_ptr<intermediate_variable>& in, const std::string& vname, std::shared_ptr<expression> idx1, std::shared_ptr<expression> idx2) {
    auto l = ctx.variables.find(vname);
    if (l != ctx.variables.end()) {
        auto& def = std::get<0>(l->second);
        auto& iv = std::get<1>(l->second);
        if (def.array == var_def::ARRAY_1D || def.array == var_def::ARRAY_2D) {
            auto q = std::make_shared<local_array_write_quadruple>(ctx);
            q->out = std::get<1>(l->second);
            q->arr = def.array;
            q->in_list.push_back(in);
            std::shared_ptr<intermediate_variable> im1, im2;
            assert(idx1);
            im1 = idx1->write_intermediate(ctx);
            q->in_list.push_back(im1);
            if (def.array == var_def::ARRAY_2D) {
                assert(idx2);
                im2 = idx2->write_intermediate(ctx);
                q->dimen = def.dimen2;
                q->in_list.push_back(im2);
            }
            ctx.current_block->quadruples.push_back(q);
        } else {
            auto q = std::make_shared<assign_quadruple>(ctx);
            q->out = std::get<1>(l->second);
            q->in_list.push_back(in);
            ctx.current_block->quadruples.push_back(q);
        }
    } else {
        // global bar
        auto g = ctx.glob.variables.find(vname);
        if (g == ctx.glob.variables.end())
            throw parsing_failure("Variable not found");
        if (g->second.array == var_def::SCALAR_VAR) {
            auto q = std::make_shared<global_variable_write_quadruple>(ctx);
            q->in_list.push_back(in);
            q->arr = g->second.array;
            q->name = vname;
            ctx.current_block->quadruples.push_back(q);
        } else if (g->second.array == var_def::ARRAY_1D || g->second.array == var_def::ARRAY_2D) {
            auto q = std::make_shared<global_variable_write_quadruple>(ctx);
            q->in_list.push_back(in);
            q->arr = g->second.array;
            q->name = vname;
            if (g->second.array == var_def::ARRAY_1D || g->second.array == var_def::ARRAY_2D) {
                assert(idx1);
                auto im1 = idx1->write_intermediate(ctx);
                q->in_list.push_back(im1);
            }
            if (g->second.array == var_def::ARRAY_2D) {
                q->dimen = g->second.dimen2;
                assert(idx2);
                auto im2 = idx2->write_intermediate(ctx);
                q->in_list.push_back(im2);
            }
            ctx.current_block->quadruples.push_back(q);
        }
    }
}

void assignment_statement::write_intermediate(generation_context &ctx) {
    auto in = this->val->write_intermediate(ctx);
    write(ctx, in, identifier->text, idx1, idx2);
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
    auto q = std::make_shared<return_exit>(ctx);
    if (this->val) {
        q->in_list.push_back(this->val->write_intermediate(ctx));
    }
    ctx.current_block->eop = q;
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
    write(ctx, out, identifier->text, nullptr, nullptr);
}

void block_statement::write_intermediate(generation_context &ctx) {
    for (auto& s : this->statements) {
        s->write_intermediate(ctx);
    }
}

void if_statement::write_intermediate(generation_context &ctx) {
    auto c1 = this->cond.exp1->write_intermediate(ctx);
    auto c2 = this->cond.exp2->write_intermediate(ctx);
    auto l = std::make_shared<condition_exit>(ctx);
    l->comparator = this->cond.comparator;
    l->in_list.push_back(c1);
    l->in_list.push_back(c2);
    ctx.current_block->eop = l;
    
    ctx.new_block();
    l->pass_block = ctx.current_block;
    this->if_body->write_intermediate(ctx);
    auto ib = ctx.current_block;
    
    std::shared_ptr<quadruple_block> eb{};
    
    if (this->else_body) {
        ctx.new_block();
        l->fail_block = ctx.current_block;
        this->else_body->write_intermediate(ctx);
        eb = ctx.current_block;
    }
    
    ctx.new_block();
    auto exit = std::make_shared<jump_exit>(ctx);
    exit->next_block = ctx.current_block;
    ib->eop = exit;
    if (!eb) {
        l->fail_block = ctx.current_block;
    } else {
        eb->eop = exit;
    }
}

void for_statement::write_intermediate(generation_context &ctx) {
    auto iexp = initial_exp->write_intermediate(ctx);
    write(ctx, iexp, initial_var->text, nullptr, nullptr);
    
    auto c1 = this->cond.exp1->write_intermediate(ctx);
    auto c2 = this->cond.exp2->write_intermediate(ctx);
    auto l = std::make_shared<condition_exit>(ctx);
    l->comparator = this->cond.comparator;
    l->in_list.push_back(c1);
    l->in_list.push_back(c2);
    ctx.current_block->eop = l;
    
    ctx.new_block();
    auto ib = ctx.current_block;
    l->pass_block = ib;
    this->body->write_intermediate(ctx);
    
    auto v1 = std::make_shared<variable_access_expression>();
    v1->name = this->step_get_var;
    auto cexp = std::make_shared<calculate_expression>();
    cexp->a = v1;
    cexp->op = PLUS;
    cexp->b = std::make_shared<constant_expression>(this->step_len, INTCON, -1);
    auto ass = std::make_shared<assignment_statement>();
    ass->identifier = this->step_set_var;
    ass->val = cexp;
    ass->write_intermediate(ctx);
    
    ctx.current_block->eop = l;
    
    ctx.new_block();
    l->fail_block = ctx.current_block;
}

void switch_statement::write_intermediate(generation_context &ctx) {
    auto val = this->exp->write_intermediate(ctx);
    auto l = std::make_shared<switch_exit>(ctx);
    l->in_list.push_back(val);
    ctx.current_block->eop = l;
    
    std::vector<std::shared_ptr<quadruple_block>> blocks{};
    for (auto& cond : conditions) {
        ctx.new_block();
        l->value_table.emplace_back(cond.val, ctx.current_block);
        
        cond.body->write_intermediate(ctx);
        
        blocks.push_back(ctx.current_block);
    }
    
    if (default_body) {
        ctx.new_block();
        l->default_block = ctx.current_block;
        default_body->write_intermediate(ctx);
        blocks.push_back(ctx.current_block);
    }
    
    ctx.new_block();
    if (!default_body) {
        l->default_block = ctx.current_block;
    }
    auto exit = std::make_shared<jump_exit>(ctx);
    exit->next_block = ctx.current_block;
    for (const auto& b : blocks) {
        b->eop = exit;
    }
}

void while_statement::write_intermediate(generation_context &ctx) {
    auto c1 = this->cond.exp1->write_intermediate(ctx);
    auto c2 = this->cond.exp2->write_intermediate(ctx);
    auto l = std::make_shared<condition_exit>(ctx);
    l->comparator = this->cond.comparator;
    l->in_list.push_back(c1);
    l->in_list.push_back(c2);
    ctx.current_block->eop = l;
    
    ctx.new_block();
    l->pass_block = ctx.current_block;
    this->body->write_intermediate(ctx);
    ctx.current_block->eop = l;
    
    ctx.new_block();
    l->fail_block = ctx.current_block;
}

void statement_block::populate_variables(generation_context &ctx) {
    for (auto& v : variables) {
        ctx.insert_variable(v);
    }
}

void statement_block::generate_statements(generation_context& ctx) {
    statement->write_intermediate(ctx);
    if (!ctx.current_block->eop) {
        ctx.current_block->eop = std::make_shared<return_exit>(ctx);
    }
}

void function::populate_parameters(generation_context &ctx) {
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
