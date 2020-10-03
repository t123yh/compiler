//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_PARSER_EXPRESSION_H
#define COMPILER_PARSER_EXPRESSION_H


#include "parser.h"

struct factor_parser : public parser {
    typedef std::unique_ptr<expression> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct term_parser : public parser {
    typedef std::unique_ptr<expression> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct expression_parser : public parser {
    typedef std::unique_ptr<expression> return_type;
    
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_EXPRESSION_H
