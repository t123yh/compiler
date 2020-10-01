//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_PARSER_PROGRAM_H
#define COMPILER_PARSER_PROGRAM_H

#include "parser.h"
#include "const_parsers.h"

struct program_parser : public parser
{
    typedef struct{} return_type;
    
    return_type parse(parsing_context &context) const {
        std::vector<constant_definition> global_const_table;
        if (context.match(const_description_parser())) {
            global_const_table = context.expect_one(const_description_parser());
        }
        std::cout << "asf";
        return {};
    }
};

#endif //COMPILER_PARSER_PROGRAM_H
