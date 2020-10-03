//
// Created by t123yh on 2020/10/3.
//

#include "parser_complex_statement.h"
#include "token_parser.h"
#include "parser_expression.h"
#include "parser_statement.h"
#include "parser_const.h"
#include "parser_variable.h"

if_parser::return_type if_parser::parse(parsing_context &context) const {
    auto ret = std::unique_ptr<if_statement>(new if_statement);
    auto s1 = context.expect(
            token_parser<IFTK>(),
            token_parser<LPARENT>(),
            condition_parser(),
            token_parser<RPARENT>(),
            statement_parser()
    );
    ret->cond = std::move(std::get<2>(s1));
    ret->if_body = std::move(std::get<4>(s1));
    if (context.parse_if_match(token_parser<ELSETK>())) {
        ret->else_body = context.expect_one(statement_parser());
    }
    
    context.record("条件语句");
    return ret;
}

condition_parser::return_type condition_parser::parse(parsing_context &context) const {
    auto s = context.expect(expression_parser(), token_parser<GEQ, GRE, LEQ, LSS, EQL, NEQ>(), expression_parser());
    
    condition cond{.exp1 = std::move(std::get<0>(s)), .exp2 = std::move(std::get<2>(s)), .comparator = std::get<1>(
            s)->type};
    
    context.record("条件");
    return std::move(cond);
}

while_parser::return_type while_parser::parse(parsing_context &context) const {
    auto ret = std::unique_ptr<while_statement>(new while_statement);
    auto s1 = context.expect(
            token_parser<WHILETK>(),
            token_parser<LPARENT>(),
            condition_parser(),
            token_parser<RPARENT>(),
            statement_parser()
    );
    ret->cond = std::move(std::get<2>(s1));
    ret->body = std::move(std::get<4>(s1));
    context.record("循环语句");
    return ret;
}

for_parser::return_type for_parser::parse(parsing_context &context) const {
    auto s1 = context.expect(
            token_parser<FORTK>(),      // 0
            token_parser<LPARENT>(),    // 1
            token_parser<IDENFR>(),     // 2
            token_parser<ASSIGN>(),     // 3
            expression_parser(),        // 4
            token_parser<SEMICN>(),     // 5
            condition_parser(),         // 6
            token_parser<SEMICN>(),     // 7
            token_parser<IDENFR>(),     // 8
            token_parser<ASSIGN>(),     // 9
            token_parser<IDENFR>(),     // 10
            token_parser<PLUS, MINU>(), // 11
            step_len_parser(),          // 12
            token_parser<RPARENT>(),    // 13
            statement_parser()          // 14
    );
    auto ret = std::unique_ptr<for_statement>(new for_statement);
    ret->initial_var = std::get<2>(s1);
    ret->initial_exp = std::move(std::get<4>(s1));
    ret->cond = std::move(std::get<6>(s1));
    ret->step_set_var = std::get<8>(s1);
    ret->step_get_var = std::get<10>(s1);
    bool inv = std::get<11>(s1)->type == MINU;
    ret->step_len = std::get<12>(s1);
    if (inv) {
        ret->step_len = -ret->step_len;
    }
    ret->body = std::move(std::get<14>(s1));
    
    context.record("循环语句");
    return ret;
}

step_len_parser::return_type step_len_parser::parse(parsing_context &context) const {
    auto ret = context.expect_one(unsigned_parser());
    context.record("步长");
    return ret;
}

switch_parser::return_type switch_parser::parse(parsing_context &context) const {
    auto u = std::unique_ptr<switch_statement>(new switch_statement);
    auto par = context.expect(
            token_parser<SWITCHTK>(),    // 0
            token_parser<LPARENT>(),     // 1
            expression_parser(),         // 2
            token_parser<RPARENT>(),     // 3
            token_parser<LBRACE>(),      // 4
            switch_cond_table_parser(),  // 5
            switch_default_parser(),     // 6
            token_parser<RBRACE>()       // 7
    );
    
    u->exp = std::move(std::get<2>(par));
    u->conditions = std::move(std::get<5>(par));
    u->default_body = std::move(std::get<6>(par));
    
    context.record("情况语句");
    return u;
}

switch_default_parser::return_type switch_default_parser::parse(parsing_context &context) const {
    auto r = context.expect(token_parser<DEFAULTTK>(), token_parser<COLON>(), statement_parser());
    context.record("缺省");
    return std::move(std::get<2>(r));
}

switch_cond_table_parser::return_type switch_cond_table_parser::parse(parsing_context &context) const {
    std::vector<switch_cond> ret;
    ret.push_back(context.expect_one(switch_cond_parser()));
    while (context.match(token_parser<CASETK>())) {
        ret.push_back(context.expect_one(switch_cond_parser()));
    }
    
    context.record("情况表");
    return ret;
}

struct constant_parser : public parser
{
    typedef int64_t return_type;
    
    return_type parse(parsing_context &context) const {
        int64_t val = 0;
        if (context.match(integer_parser())) {
            val = context.expect_one(integer_parser());
        } else if (context.match(token_parser<CHARCON>())) {
            val = (unsigned char) context.expect_one(token_parser<CHARCON>())->text[0];
        }
        
        context.record("常量");
        return val;
    }
};

switch_cond_parser::return_type switch_cond_parser::parse(parsing_context &context) const {
    switch_cond c;
    auto r = context.expect(token_parser<CASETK>(), constant_parser(), token_parser<COLON>(), statement_parser());
    c.val = std::get<1>(r);
    c.body = std::move(std::get<3>(r));
    context.record("情况子语句");
    return c;
}