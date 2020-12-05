//
// Created by t123yh on 2020/10/1.
//

#include "parser_variable.h"
#include "parser_const.h"
#include "parser_function.h"

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
        /*
        if (cnt != len) {
            context.errors.push_back(error{context.line(), E_ARRAY_INIT_SIZE_MISMATCH});
        }
         */
        return r;
    }
};

var_def var_definition_with_init_parser::parse(parsing_context &context) const {
    var_def v;
    auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
    v.type = std::get<0>(idf)->type;
    v.identifier = std::get<1>(idf);
    
    if (context.parse_if_match(token_parser<LBRACK>())) {
        v.dimen1 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>(true)));
        if (v.dimen1 <= 0) {
            throw parsing_failure("Length of array is too small");
        }
    }
    if (context.parse_if_match(token_parser<LBRACK>())) {
        v.dimen2 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>(true)));
        if (v.dimen2 <= 0) {
            throw parsing_failure("Length of array is too small");
        }
    }
    
    
    context.expect_one(token_parser<ASSIGN>());
    
    bool init_ok = false;
    // TODO: Initial value
    if (v.dimen1 == -1 && v.dimen2 == -1) { // scalar
        v.array = var_def::SCALAR_VAR;
        v.value = context.expect_one(typed_constant_parser(v.type));
        init_ok = true;
    } else {
        if (context.match(token_parser<LBRACE>(), token_parser<LBRACE>())) {
            v.array = var_def::ARRAY_2D;
            auto iv = context.expect_one(list_parser<list_parser<typed_constant_parser>>(list_parser<typed_constant_parser>(typed_constant_parser(v.type), v.dimen2), v.dimen1));
            if (v.dimen1 == iv.size()) {
                init_ok = true;
                for (auto& cont : iv) {
                    if (cont.size() != v.dimen2) {
                        init_ok = false;
                        break;
                    }
                }
            }
            v.initialization = std::make_shared<std::vector<int64_t>>();
            for (auto& i1 : iv) {
                for (auto& i2 : i1) {
                    v.initialization->push_back(i2);
                }
            }
        } else if (context.match(token_parser<LBRACE>())) {
            v.array = var_def::ARRAY_1D;
            auto iv = context.expect_one(list_parser<typed_constant_parser>(typed_constant_parser(v.type), v.dimen1));
            init_ok = v.dimen2 == -1 && v.dimen1 == iv.size();
            v.initialization = std::make_shared<std::vector<int64_t>>();
            for (auto& i1 : iv) {
                v.initialization->push_back(i1);
            }
        } else {
            throw parsing_failure("Invalid array init");
        }
    }
    
    if (!init_ok) {
        context.errors.push_back(error{context.line(), E_ARRAY_INIT_SIZE_MISMATCH});
    }
    
    if (context.strategy == FINAL) {
        context.add_symbol(std::make_shared<variable_symbol>(v));
    }
    
    context.record("变量定义及初始化");
    return v;
}

std::vector<var_def>
var_definition_without_init_parser::parse(parsing_context &context) const {
    std::vector<var_def> vs;
    auto type = context.expect_one(token_parser<INTTK, CHARTK>())->type;
    while (true) {
        var_def v{};
        v.type = type;
        v.identifier = context.expect_one(token_parser<IDENFR>());
        v.array = var_def::SCALAR_VAR;
            
        if (context.parse_if_match(token_parser<LBRACK>())) {
            v.array = var_def::ARRAY_1D;
            v.dimen1 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>(true)));
            if (v.dimen1 <= 0) {
                throw parsing_failure("Length of array is too small");
            }
        }
        if (context.parse_if_match(token_parser<LBRACK>())) {
            v.array = var_def::ARRAY_2D;
            v.dimen2 = std::get<0>(context.expect(unsigned_parser(), token_parser<RBRACK>(true)));
            if (v.dimen2 <= 0) {
                throw parsing_failure("Length of array is too small");
            }
        }
        
        if (context.match(token_parser<ASSIGN>())) {
            throw parsing_failure("Not me!");
        }
        vs.push_back(v);
    
        if (context.strategy == FINAL) {
            context.add_symbol(std::make_shared<variable_symbol>(v));
        }
        
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
    } while (context.match(var_definition_parser()) && !context.match(decl_header_parser(), token_parser<LPARENT>()));
    context.record("变量说明");
    return vars;
}
