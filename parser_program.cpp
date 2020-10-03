//
// Created by t123yh on 2020/10/1.
//

#include "parser_program.h"
#include "parser_variable.h"
#include "parser_function.h"

program_parser::return_type program_parser::parse(parsing_context &context) const {
    program result;
    std::vector<var_def>& global_symbol_table = result.symbols;
    if (context.match(const_description_parser())) {
        auto r = context.expect_one(const_description_parser());
        for (auto& x : r) {
            global_symbol_table.push_back(x);
        }
    }
    if (context.match(var_description_parser())) {
        auto r = context.expect_one( var_description_parser());
        for (auto& x : r) {
            global_symbol_table.push_back(x);
        }
    }
    
    while (!context.match(token_parser<VOIDTK>(), token_parser<MAINTK>())) {
        auto func = context.expect_one(function_parser());
        if (context.strategy == FINAL) {
            context.func_tab[func->signature.identifier->text] = func->signature.return_type != VOIDTK;
        }
        result.functions.push_back(std::move(func));
    }
    result.main_function = context.expect_one(main_function_parser());
    context.record("程序");
    return result;
}
