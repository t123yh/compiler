//
// Created by t123yh on 2020/10/1.
//

#include "parser_variable.h"
#include "parser_const.h"

constant_parser::return_type constant_parser::parse(parsing_context &context) const {
    int64_t val = 0;
    if (t == var_def::INT && context.match(integer_parser())) {
        val = context.expect_one(integer_parser());
    } else if (t == var_def::CHAR && context.match(token_parser<CHARCON>())) {
        val = (unsigned char) context.expect_one(token_parser<CHARCON>())->text[0];
    } else {
        throw parsing_failure("Constant type mismatch");
    }
    
    context.record("常量");
    return val;
}

constant_parser::constant_parser(var_def::type_t t) : t(t) {

}

template <typename TParser> struct list_parser : public parser
{
    typedef std::vector<typename TParser::return_type> return_type;
    
    TParser parser;
    int len;
    
    list_parser(TParser parser, int len) : parser(parser), len(len) {}
    
    return_type parse(parsing_context &context) const {
        return_type r;
        context.expect_one(token_parser<LBRACE>());
        int cnt = 0;
        token_ptr end_tk;
        do {
            cnt++;
            // TODO: use val
            r.push_back(context.expect_one(parser));
            end_tk = context.expect_one(token_parser<COMMA, RBRACE>());
        } while (end_tk->type == COMMA);
        if (cnt != len) {
            throw parsing_failure("Array size mismatch");
        }
        return r;
    }
};

var_def var_definition_with_init_parser::parse(parsing_context &context) const {
    var_def v;
    auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
    v.type = std::get<0>(idf)->type == INTTK ? var_def::INT : var_def::CHAR;
    v.identifier = std::get<1>(idf);
    
    // TODO: set dimension for v
    int dimen1 = -1, dimen2 = -1;
    if (context.parse_if_match(token_parser<LBRACK>())) {
        dimen1 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>()));
        if (dimen1 <= 0) {
            throw parsing_failure("Length of array is too small");
        }
    }
    if (context.parse_if_match(token_parser<LBRACK>())) {
        dimen2 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>()));
        if (dimen2 <= 0) {
            throw parsing_failure("Length of array is too small");
        }
    }
    
    
    context.expect_one(token_parser<ASSIGN>());
    
    // TODO: Initial value
    if (dimen1 == -1 && dimen2 == -1) { // scalar
        v.array = var_def::SCALAR_VAR;
        v.value = context.expect_one(constant_parser(v.type));
    } else if (dimen2 == -1) { // 1d array
        v.array = var_def::ARRAY_1D;
        volatile auto iv = context.expect_one(list_parser<constant_parser>(constant_parser(v.type), dimen1));
    } else { // 2d array
        v.array = var_def::ARRAY_2D;
        volatile auto iv = context.expect_one(list_parser<list_parser<constant_parser>>(list_parser<constant_parser>(constant_parser(v.type), dimen2), dimen1));
    }
    
    context.record("变量定义及初始化");
    return v;
}

std::vector<var_def>
var_definition_without_init_parser::parse(parsing_context &context) const {
    std::vector<var_def> vs;
    while (true) {
        var_def v;
        auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
        v.type = token2type(std::get<0>(idf));
        v.identifier = std::get<1>(idf);
    
        // TODO: set dimension for v
        int dimen1 = -1, dimen2 = -1;
        if (context.parse_if_match(token_parser<LBRACK>())) {
            dimen1 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>()));
            if (dimen1 <= 0) {
                throw parsing_failure("Length of array is too small");
            }
        }
        if (context.parse_if_match(token_parser<LBRACK>())) {
            dimen2 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>()));
            if (dimen2 <= 0) {
                throw parsing_failure("Length of array is too small");
            }
        }
        
        if (context.match(token_parser<ASSIGN>())) {
            throw parsing_failure("Not me!");
        }
        vs.push_back(v);
        if (!context.parse_if_match(token_parser<COMMA>())) {
            break;
        }
    }
    
    context.record("变量定义无初始化");
    return vs;
}

var_definition_parser::return_type var_definition_parser::parse(parsing_context &context) const {
    std::vector<var_def> vars;
    
    if (context.match(var_definition_with_init_parser())) {
        vars.push_back(context.expect_one(var_definition_with_init_parser()));
    } else if (context.match(var_definition_without_init_parser())) {
        auto vs = context.expect_one(var_definition_without_init_parser());
        for (auto& x : vs) {
            vars.push_back(x);
        }
    } else {
        throw parsing_failure("Invalid var def");
    }
    
    context.record("变量定义");
    return vars;
}

var_description_parser::return_type var_description_parser::parse(parsing_context &context) const {
    std::vector<var_def> vars;
    do {
        auto vs = context.expect_one(var_definition_parser());
        for (auto &x : vs) {
            vars.push_back(x);
        }
        context.expect_one(token_parser<SEMICN>());
    } while (context.match(var_definition_parser(), token_parser<SEMICN>()));
    context.record("变量说明");
    return vars;
}
