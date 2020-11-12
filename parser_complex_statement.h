//
// Created by t123yh on 2020/10/3.
//

#ifndef COMPILER_PARSER_COMPLEX_STATEMENT_H
#define COMPILER_PARSER_COMPLEX_STATEMENT_H

#include "parser.h"

struct condition_parser : public parser {
    
    typedef condition return_type;
    
    return_type parse(parsing_context &context) const;
};

struct if_parser : public parser {
    typedef std::shared_ptr<if_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct while_parser : public parser {
    typedef std::shared_ptr<while_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct step_len_parser : public parser {
    typedef uint64_t return_type;
    
    return_type parse(parsing_context &context) const;
};

struct for_parser : public parser {
    typedef std::shared_ptr<for_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct switch_parser : public parser {
    typedef std::shared_ptr<switch_statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

struct switch_cond_parser : public parser {
    typedef switch_cond return_type;
    token_type_t type;
    explicit switch_cond_parser(token_type_t t) : type(t){}
    return_type parse(parsing_context &context) const;
};

struct switch_cond_table_parser : public parser {
    typedef std::vector<switch_cond> return_type;
    token_type_t type;
    explicit switch_cond_table_parser(token_type_t t) : type(t){}
    return_type parse(parsing_context &context) const;
};

struct switch_default_parser : public parser {
    typedef std::shared_ptr<statement> return_type;
    
    return_type parse(parsing_context &context) const;
};

#endif //COMPILER_PARSER_COMPLEX_STATEMENT_H
