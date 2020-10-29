//
// Created by t123yh on 2020/10/1.
//

#include <vector>
#include "parser_const.h"

std::vector <var_def> const_description_parser::parse(parsing_context &context) const {
    std::vector<var_def> ret;
    
    ret = std::get<1>(context.expect(
            token_parser<CONSTTK>(),
            const_definition_parser(),
            token_parser<SEMICN>()
    ));
    
    while (context.parse_if_match(token_parser<CONSTTK>())) {
        std::vector<var_def> d = std::get<0>(context.expect(
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

var_def const_definition_parser::parse_single(parsing_context &context, token_type_t type) const {
    var_def def;
    def.identifier = context.expect_one(token_parser<IDENFR>());
    def.array = var_def::CONST;
    def.type = type;
    
    context.expect(token_parser<ASSIGN>());
    
    if (type == INTTK) {
        def.value = context.expect_one(integer_parser());
    } else if (type == CHARTK) {
        def.value = (unsigned char) context.expect_one(token_parser<CHARCON>())->text[0];
    }
    
    return def;
}

const_definition_parser::return_type const_definition_parser::parse(parsing_context &context) const {
    std::vector<var_def> ret;
    token_type_t type = context.expect_one(token_parser<INTTK, CHARTK>())->type;
    
    do {
        ret.push_back(parse_single(context, type));
    
        if (context.strategy == FINAL) {
            context.add_symbol(make_unique<variable_symbol>(ret.back()));
        }
    }
    while (context.parse_if_match(token_parser<COMMA>()));
    context.debug_output << "<常量定义>" << std::endl;
    return ret;
}

integer_parser::return_type integer_parser::parse(parsing_context &context) const {
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

unsigned_parser::return_type unsigned_parser::parse(parsing_context &context) const {
    auto numtk = context.expect_one(token_parser<INTCON>());
    context.debug_output << "<无符号整数>" << std::endl;
    return stoi(numtk->text);
}
