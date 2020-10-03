//
// Created by t123yh on 2020/10/3.
//

#include "parser_function.h"
#include "token_parser.h"
#include "parser_const.h"
#include "parser_variable.h"
#include "parser_statement.h"
#include "parser_expression.h"

decl_header_parser::return_type decl_header_parser::parse(parsing_context &context) const {
    function_signature sign;
    
    auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
    sign.return_type = token2type(std::get<0>(idf));
    sign.identifier = std::get<1>(idf);
    
    context.record("声明头部");
    return sign;
}

parameter_list_parser::return_type parameter_list_parser::parse(parsing_context &context) const {
    std::vector<function_parameter> ret;
    while (!context.match(token_parser<RPARENT>())) {
        auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
        ret.push_back({.type = token2type(std::get<0>(idf)), .name= std::get<1>(idf)});
    }
    context.record("参数表");
    return ret;
}

compound_statement_parser::return_type compound_statement_parser::parse(parsing_context &context) const {
    statement_block ret;
    if (context.match(const_description_parser())) {
        ret.variables = context.expect_one(const_description_parser());
    }
    if (context.match(var_description_parser())) {
        auto r = context.expect_one(var_description_parser());
        for (auto &x : r) {
            ret.variables.push_back(x);
        }
    }
    ret.statements = context.expect_one(statements_parser());
    context.record("复合语句");
    return ret;
}

statements_parser::return_type statements_parser::parse(parsing_context &context) const {
    return_type ret;
    
    while (!context.match(token_parser<RBRACE>())) {
        ret.push_back(context.expect_one(statement_parser()));
    }
    
    context.record("语句列");
    return ret;
}

calling_parser::return_type calling_parser::parse(parsing_context &context) const {
    auto info = std::unique_ptr<function_call_info>(new function_call_info);
    info->name = context.expect_one(token_parser<IDENFR>());
    context.expect_one(token_parser<LPARENT>());
    info->arguments = context.expect_one(arguments_parser());
    context.expect_one(token_parser<RPARENT>());
    
    if (context.strategy == FINAL) {
        if (context.func_tab.find(info->name->text) == context.func_tab.end()) {
            throw parsing_failure("Undefined function");
        }
        if (context.func_tab[info->name->text]) {
            context.record("有返回值函数调用语句");
        } else {
            context.record("无返回值函数调用语句");
        }
    }
    
    return info;
}

arguments_parser::return_type arguments_parser::parse(parsing_context &context) const {
    std::vector<std::unique_ptr<expression>> arg_list;
    do {
        arg_list.push_back(context.expect_one(expression_parser()));
    } while (context.parse_if_match(token_parser<COMMA>()));
    context.record("值参数表");
    return arg_list;
}
