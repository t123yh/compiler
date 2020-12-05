//
// Created by t123yh on 2020/9/29.
//

#include <fstream>
#include "parser.h"
#include "tokenizer.h"
#include "parsing_failure.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <array>

std::ofstream nullstream;
error_container garbage;

parsing_context parsing_context::fork_trial() {
    nullstream.setstate(std::ios_base::badbit);
    return parsing_context{start, current, end, nullstream, TRIAL, garbage};
}

void parsing_context::advance() {
    if (current != end)
        current++;
    else
        throw parsing_failure("failed to advance: end reached");
}

void parsing_context::record(const char* text) {
    debug_output << "<" << text << ">" << std::endl;
}

symbol* parsing_context::add_symbol(std::shared_ptr<symbol> item) {
    if (symbols.check_duplicate(item->get_name())) {
        errors.push_back(error{prev_line(), E_REDEFINED_SYMBOL});
        return nullptr;
    } else {
        symbols.add_symbol(std::move(item));
        return symbols.symbols.back().item.get();
    }
}

void parsing_context::ensure_variable_existance(token_ptr iden, bool write) {
    if (this->strategy == FINAL) {
        auto* symb = dynamic_cast<variable_symbol*>(symbols.find_symbol(iden->text));
        if (symb == nullptr) {
            errors.push_back(error{iden->line, E_UNDEFINED_SYMBOL});
        } else if (write) {
            if (symb->definition.array == var_def::CONST) {
                errors.push_back(error{iden->line, E_ASSIGN_TO_CONST});
            }
        }
    }
}

const char* parser::get_name() const {
    return "";
}

token_type_t get_expression_type(const expression* expr, parsing_context& ctx) {
    auto cond_type = INTTK;
    if (expr->force_int) {
        return INTTK;
    }
    auto* t = dynamic_cast<const constant_expression*>(expr);
    if (t && t->type == CHARCON)
        cond_type = CHARTK;
    auto* t2 = dynamic_cast<const variable_access_expression*>(expr);
    if (t2) {
        auto* vara = dynamic_cast<variable_symbol*>(ctx.symbols.find_symbol(t2->name->text));
        if (vara != nullptr)
            return vara->definition.type;
    }
    auto* t3 = dynamic_cast<const calling_expression*>(expr);
    if (t3) {
        auto* vara = dynamic_cast<function_symbol*>(ctx.symbols.find_symbol(t3->call_info->name->text));
        if (vara != nullptr)
            return vara->sign.return_type;
    }
    
    return cond_type;
}

