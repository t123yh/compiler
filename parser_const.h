//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_PARSER_CONST_H
#define COMPILER_PARSER_CONST_H

#include "parser.h"
#include "token_parser.h"
#include "defs.h"

struct unsigned_parser : public parser
{
    typedef uint64_t return_type;
    
    return_type parse(parsing_context &context) const;
};

struct integer_parser : public parser
{
    typedef int64_t return_type;
    
    return_type parse(parsing_context &context) const;
};

struct const_definition_parser : public parser
{
    typedef std::vector<var_def> return_type;
    
    var_def parse_single(parsing_context& context, token_type_t type) const;
    
    return_type parse(parsing_context &context) const;
};

struct const_description_parser : public parser
{
    typedef std::vector<var_def> return_type;
    
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_CONST_H
