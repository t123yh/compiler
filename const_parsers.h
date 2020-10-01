//
// Created by t123yh on 2020/9/30.
//

#ifndef COMPILER_CONST_PARSERS_H
#define COMPILER_CONST_PARSERS_H

#include "parser.h"
#include "token_parser.h"

struct unsigned_parser : public parser
{
    typedef uint64_t return_type;
    
    return_type parse(parsing_context &context) const {
        auto numtk = context.expect_one(token_parser<INTCON>());
        context.debug_output << "<无符号整数>" << std::endl;
        return stoi(numtk->text);
    }
};

struct integer_parser : public parser
{
    typedef int64_t return_type;
    
    return_type parse(parsing_context &context) const {
        bool sign = false;
        token_parser<PLUS, MINU> sign_parser;
        if (context.match(sign_parser)) {
            auto signtk = context.expect_one(sign_parser);
            if (signtk->type == MINU) {
                sign = true;
            }
        }
        
        int64_t num = context.expect_one(unsigned_parser());
        context.debug_output << "<整数>" << std::endl;
        if (sign)
            num = -num;
        return num;
    }
};

struct constant_definition
{
    enum type_t
    {
        INT, CHAR
    };
    type_t type;
    std::string identifier;
    int64_t value;
};

struct const_definition_parser : public parser
{
    typedef std::vector<constant_definition> return_type;
    
    constant_definition parse_single(parsing_context& context, token_type_t type) const {
        constant_definition def;
    
    
        def.identifier = context.expect_one(token_parser<IDENFR>())->text;
        context.expect(token_parser<ASSIGN>());
    
        if (type == INTTK) {
            def.type = constant_definition::INT;
            def.value = context.expect_one(integer_parser());
        } else if (type == CHARTK) {
            def.type = constant_definition::CHAR;
            def.value = (unsigned char) context.expect_one(token_parser<CHARCON>())->text[0];
        }
        
        return def;
    }
    
    return_type parse(parsing_context &context) const {
        std::vector<constant_definition> ret;
        token_type_t type = context.expect_one(token_parser<INTTK, CHARTK>())->type;
        
        do {
            ret.push_back(parse_single(context, type));
        }
        while (context.parse_if_match(token_parser<COMMA>()));
        context.debug_output << "<常量定义>" << std::endl;
        return ret;
    }
};

struct const_description_parser : public parser
{
    typedef std::vector<constant_definition> return_type;
    
    return_type parse(parsing_context &context) const {
        std::vector<constant_definition> ret;
    
        if (context.strategy == TRIAL) {
            token_parser<CONSTTK>();
            return ret;
        }
        
        ret = std::get<1>(context.expect(
                token_parser<CONSTTK>(),
                const_definition_parser(),
                token_parser<SEMICN>()
        ));
        
        while (context.parse_if_match(token_parser<CONSTTK>())) {
            std::vector<constant_definition> d = std::get<0>(context.expect(
                    const_definition_parser(),
                    token_parser<SEMICN>()
            ));
            
            for (auto& item : d) {
                ret.push_back(item);
            }
        }
    
        context.debug_output << "<常量说明>" << std::endl;
        return ret;
    }
};

#endif //COMPILER_CONST_PARSERS_H
