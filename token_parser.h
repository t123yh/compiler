//
// Created by t123yh on 2020/10/1.
//

#ifndef COMPILER_TOKEN_PARSER_H
#define COMPILER_TOKEN_PARSER_H

#include "parser.h"

template <token_type_t... acceptable_tokens> struct token_parser : public parser {
    typedef token_ptr return_type;
    return_type parse(parsing_context &context) const {
        for (token_type_t t : {acceptable_tokens...}) {
            if (t == context.current->type) {
                context.debug_output << context.current->pretty_print();
                auto ret = context.current;
                context.advance();
                return ret;
            }
        }
        throw parsing_failure("Invalid token");
    }
};

#endif //COMPILER_TOKEN_PARSER_H
