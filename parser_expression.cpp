//
// Created by t123yh on 2020/10/3.
//

#include "parser_expression.h"
#include "token_parser.h"
#include "parser_const.h"
#include "parser_function.h"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

expression_parser::return_type expression_parser::parse(parsing_context &context) const {
    std::unique_ptr<expression> current;
    if (context.parse_if_match(token_parser<MINU>())) {
        std::unique_ptr<calculate_expression> c = make_unique<calculate_expression>();
        c->a = make_unique<constant_expression>(0);
        c->op = calculate_expression::T_MINUS;
        c->b = context.expect_one(term_parser());
        current = std::move(c);
    } else {
        context.parse_if_match(token_parser<PLUS>());
        current = context.expect_one(term_parser());
    }
    
    while (context.match(token_parser<PLUS, MINU>())) {
        std::unique_ptr<calculate_expression> new_exp = std::unique_ptr<calculate_expression>(new calculate_expression);
        new_exp->a = std::move(current);
        auto t = context.expect_one(token_parser<PLUS, MINU>());
        if (t->type == PLUS) {
            new_exp->op = calculate_expression::T_PLUS;
        } else {
            new_exp->op = calculate_expression::T_MINUS;
        }
        new_exp->b = context.expect_one(term_parser());
        current = std::move(new_exp);
    }
    
    return current;
}

term_parser::return_type term_parser::parse(parsing_context &context) const {
    std::unique_ptr<expression> current;
    current = context.expect_one(factor_parser());
    while (true) {
        std::unique_ptr<calculate_expression> new_exp = std::unique_ptr<calculate_expression>(new calculate_expression);
        new_exp->a = std::move(current);
        if (context.parse_if_match(token_parser<MULT>())) {
            new_exp->op = calculate_expression::T_MUL;
        } else if (context.parse_if_match(token_parser<DIV>())) {
            new_exp->op = calculate_expression::T_DIV;
        } else {
            break;
        }
        new_exp->b = context.expect_one(factor_parser());
        current = std::move(new_exp);
    }
    context.record("项");
    return current;
}

factor_parser::return_type factor_parser::parse(parsing_context &context) const {
    std::unique_ptr<expression> r;
    if (context.match(token_parser<IDENFR>())) {
        auto ret = std::unique_ptr<variable_access_expression>(new variable_access_expression);
        ret->name = context.expect_one(token_parser<IDENFR>());
#define ARRAY_IDX token_parser<LBRACK>(), expression_parser(), token_parser<RBRACK>()
        if (context.match(ARRAY_IDX)) {
            ret->idx1 = std::get<1>(context.expect(ARRAY_IDX));
        }
        if (context.match(ARRAY_IDX)) {
            ret->idx2 = std::get<1>(context.expect(ARRAY_IDX));
        }
        r = std::move(ret);
    } else if (context.match(token_parser<CHARCON>())) {
        r = std::unique_ptr<constant_expression>(
                new constant_expression(context.expect_one(token_parser<CHARCON>())->text[0]));
    } else if (context.match(integer_parser())) {
        r = std::unique_ptr<constant_expression>(
                new constant_expression(context.expect_one(integer_parser())));
    } else if (context.parse_if_match(token_parser<LPARENT>())) {
        r = std::get<0>(context.expect(expression_parser(), token_parser<RPARENT>()));
    } else if (context.match(calling_parser())) {
        auto e = std::unique_ptr<calling_expression>(new calling_expression);
        e->call_info = context.expect_one(calling_parser());
        r = std::move(e);
    } else {
        throw parsing_failure("Invalid factor");
    }
    context.record("因子");
    return r;
}
