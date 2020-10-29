//
// Created by t123yh on 2020/10/1.
//

#ifndef COMPILER_TOKEN_PARSER_H
#define COMPILER_TOKEN_PARSER_H

#include "parser.h"
#include "utils.h"

template <token_type_t... acceptable_tokens> struct token_parser : public parser {
    typedef token_ptr return_type;
    return_type parse(parsing_context &context) const {
        
        token_type_t tokenList[] = {acceptable_tokens...};
        
        if (COUNT_OF(tokenList) == 1 && context.strategy == FINAL) {
            if (tokenList[0] == RPARENT){
                if (context.current->type != RPARENT) {
                    context.errors.push_back(error{context.prev_line(), E_SHOULD_BE_PARENTHESIS});
                    return nullptr;
                }
            }
            if (tokenList[0] == RBRACK){
                if (context.current->type != RBRACK) {
                    context.errors.push_back(error{context.prev_line(), E_SHOULD_BE_BRACKET});
                    return nullptr;
                }
            }
            if (tokenList[0] == SEMICN){
                if (context.current->type != SEMICN) {
                    context.errors.push_back(error{context.prev_line(), E_SHOULD_BE_SEMICOLON});
                    return nullptr;
                }
            }
        }
        
        for (token_type_t t : {acceptable_tokens...}) {
            if (t == context.current->type) {
                context.debug_output << context.current->pretty_print();
                auto ret = context.current;
                context.advance();
                return &*ret;
            }
        }
        throw parsing_failure("Invalid token at line " + std::to_string(context.line()));
    }
};

#endif //COMPILER_TOKEN_PARSER_H
