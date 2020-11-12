//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_PARSER_STATEMENT_H
#define COMPILER_PARSER_STATEMENT_H

#include <memory>
#include "defs.h"
#include "parser.h"

struct statement_parser : public parser {
    typedef std::shared_ptr<statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct assignment_parser : public parser {
    typedef std::shared_ptr<assignment_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct return_parser : public parser {
    typedef std::shared_ptr<return_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct print_parser : public parser {
    typedef std::shared_ptr<print_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct scan_parser : public parser {
    typedef std::shared_ptr<scan_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_STATEMENT_H
