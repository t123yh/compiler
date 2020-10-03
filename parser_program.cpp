//
// Created by t123yh on 2020/10/1.
//

#include "parser_program.h"
#include "parser_variable.h"

program_parser::return_type program_parser::parse(parsing_context &context) const {
    std::vector<var_def> global_symbol_table;
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
    std::cout << "asf";
    return {};
}
