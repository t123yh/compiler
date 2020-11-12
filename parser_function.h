//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_PARSER_FUNCTION_H
#define COMPILER_PARSER_FUNCTION_H


#include "defs.h"
#include "parser.h"

struct decl_header_parser : public parser
{
    typedef function_signature return_type;
    
    return_type parse(parsing_context &context) const;
};

struct parameter_list_parser : public parser {
    typedef std::vector<var_def> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct compound_statement_parser : public parser {
    typedef statement_block return_type;
    
    return_type parse(parsing_context &context) const;
};

struct statements_parser: public parser
{
    typedef std::vector<std::shared_ptr<statement>> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct arguments_parser: public parser
{
    typedef std::vector<std::shared_ptr<expression>> return_type;
    return_type parse(parsing_context &context) const;
};

struct calling_parser : public parser {
    typedef std::shared_ptr<function_call_info> return_type;
    return_type parse(parsing_context &context) const;
};

struct function_parser : public parser {
    typedef std::shared_ptr<function> return_type;
    return_type parse(parsing_context &context) const;
};

struct main_function_parser : public parser {
    typedef statement_block return_type;
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_FUNCTION_H
