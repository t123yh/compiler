//
// Created by t123yh on 2020/9/29.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#include "tokenizer.h"
#include "parsing_failure.h"
#include <iostream>
#include <map>

#include "defs.h"
enum class variable_type {
    INT,
    CHAR
};
struct variable_info {
    std::string name;
    variable_type type;
    int d, x, y;
};

enum parsing_strategy {
    TRIAL, FINAL
};

struct parsing_context;

struct parser {
    virtual const char* get_name() const;
};


struct parsing_context
{
    token_ptr current, end;
    std::ostream &debug_output;
    parsing_strategy strategy;
    error_container& errors;
    std::map<std::string, bool> func_tab;
    
    void advance();
    parsing_context fork_trial();
    
    void record(const char* txt);
    
    template <typename... TParsers> std::tuple<typename TParsers::return_type...> expect(TParsers... args) {
        return std::tuple<typename TParsers::return_type...>{args.parse(*this)...};
    }
    
    template <typename TParser> typename TParser::return_type expect_one(TParser arg) {
        return arg.parse(*this);
    }
    
    template <typename... TParsers> bool match(TParsers... args) {
        auto cloned = fork_trial();
        try {
            cloned.expect(args...);
        } catch (parsing_failure &fail) {
            return false;
        }
        return true;
    }
    
    template <typename... TParsers> bool parse_if_match(TParsers... args) {
        if (match(args...)) {
            expect(args...);
            return true;
        }
        return false;
    }
    
    int line() {
        return this->current->line;
    }
};

static inline bool is_charcon(const std::unique_ptr<expression>& ptr, parsing_context& ctx, error_type ex) {
    auto* e1 = dynamic_cast<constant_expression*>(ptr.get());
    if (e1) {
        if (e1->type == CHARCON) {
            ctx.errors.push_back({e1->line, ex});
            return true;
        }
    }
    return false;
}


#endif //COMPILER_PARSER_H
