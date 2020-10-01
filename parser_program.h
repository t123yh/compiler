//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_PARSER_PROGRAM_H
#define COMPILER_PARSER_PROGRAM_H

#include "parser.h"
#include "parser_const.h"

struct program_parser : public parser
{
    typedef struct{} return_type;
    
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_PROGRAM_H
