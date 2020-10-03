//
// Created by t123yh on 2020/10/1.
//

#ifndef COMPILER_PARSER_VARIABLE_H
#define COMPILER_PARSER_VARIABLE_H

#include "defs.h"
#include "parser.h"

struct typed_constant_parser : public parser {
    typedef int64_t return_type;
    var_def::type_t t;
    typed_constant_parser(var_def::type_t t);
    return_type parse(parsing_context &context) const;
};

struct var_definition_with_init_parser : public parser
{
    typedef var_def return_type;
    
    return_type parse(parsing_context &context) const;
};

struct var_definition_without_init_parser : public parser
{
    typedef std::vector<var_def> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct var_definition_parser : public parser
{
    typedef std::vector<var_def> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct var_description_parser : public parser
{
    typedef std::vector<var_def> return_type;
    
    return_type parse(parsing_context &context) const;
};


#endif //COMPILER_PARSER_VARIABLE_H
