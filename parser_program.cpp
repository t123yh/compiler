//
// Created by t123yh on 2020/10/1.
//

#include "parser_program.h"

program_parser::return_type program_parser::parse(parsing_context &context) const {
    std::vector<constant_definition> global_const_table;
    if (context.match(const_description_parser())) {
        global_const_table = context.expect_one(const_description_parser());
    }
    std::cout << "asf";
    return {};
}
