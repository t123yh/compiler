//
// Created by t123yh on 2020/10/3.
//

#include "parser_expression.h"
#include "token_parser.h"
#include "parser_const.h"
#include "parser_function.h"

expression_parser::return_type expression_parser::parse(parsing_context &context) const {
    std::shared_ptr<expression> current;
    if (context.parse_if_match(token_parser<MINU>())) {
        std::shared_ptr<calculate_expression> c = std::make_shared<calculate_expression>();
        c->a = std::make_shared<constant_expression>(0, INTTK, -1);
        c->op = MINU;
        c->b = context.expect_one(term_parser());
        current = std::move(c);
    } else {
        context.parse_if_match(token_parser<PLUS>());
        current = context.expect_one(term_parser());
    }
    
    while (context.match(token_parser<PLUS, MINU>())) {
        std::shared_ptr<calculate_expression> new_exp = std::shared_ptr<calculate_expression>(new calculate_expression);
        new_exp->line = context.line();
        new_exp->a = std::move(current);
        auto t = context.expect_one(token_parser<PLUS, MINU>());
        new_exp->op = t->type;
        new_exp->b = context.expect_one(term_parser());
        current = std::move(new_exp);
    }
    
    context.record("表达式");
    return current;
}

term_parser::return_type term_parser::parse(parsing_context &context) const {
    std::shared_ptr<expression> current;
    current = context.expect_one(factor_parser());
    while (true) {
        std::shared_ptr<calculate_expression> new_exp = std::shared_ptr<calculate_expression>(new calculate_expression);
        new_exp->line = context.line();
        if (context.match(token_parser<MULT, DIV>())) {
            new_exp->op = context.expect_one(token_parser<MULT, DIV>())->type;
        } else {
            break;
        }
        new_exp->a = std::move(current);
        new_exp->b = context.expect_one(factor_parser());
        current = std::move(new_exp);
    }
    context.record("项");
    return current;
}

factor_parser::return_type factor_parser::parse(parsing_context &context) const {
    std::shared_ptr<expression> r;
    if (context.match(token_parser<IDENFR>(), token_parser<LPARENT>())) {
        auto e = std::shared_ptr<calling_expression>(new calling_expression);
        e->line = context.line();
        e->call_info = context.expect_one(calling_parser());
        r = std::move(e);
    } else if (context.match(token_parser<IDENFR>())) {
        auto ret = std::shared_ptr<variable_access_expression>(new variable_access_expression);
        ret->line = context.line();
        ret->name = context.expect_one(token_parser<IDENFR>());
        if (context.strategy == FINAL) {
            auto* symb = dynamic_cast<variable_symbol*>(context.symbols.find_symbol(ret->name->text));
            if (symb == nullptr) {
                context.errors.push_back(error{ret->name->line, E_UNDEFINED_SYMBOL});
            }
        }
#define ARRAY_IDX token_parser<LBRACK>(), expression_parser(), token_parser<RBRACK>(true)
        if (context.match(ARRAY_IDX)) {
            ret->idx1 = std::get<1>(context.expect(ARRAY_IDX));
            is_charcon(ret->idx1, context, E_INDEX_N_CHAR);
        }
        if (context.match(ARRAY_IDX)) {
            ret->idx2 = std::get<1>(context.expect(ARRAY_IDX));
            is_charcon(ret->idx2, context, E_INDEX_N_CHAR);
        }
        r = std::move(ret);
    } else if (context.match(token_parser<CHARCON>())) {
        r = std::shared_ptr<constant_expression>(
                new constant_expression(context.expect_one(token_parser<CHARCON>())->text[0], CHARCON, context.line()));
        r->line = context.line();
    } else if (context.match(integer_parser())) {
        r = std::shared_ptr<constant_expression>(
                new constant_expression(context.expect_one(integer_parser()), INTCON, context.line()));
        r->line = context.line();
    } else if (context.parse_if_match(token_parser<LPARENT>())) {
        r = std::get<0>(context.expect(expression_parser(), token_parser<RPARENT>(true)));
        r->line = context.line();
    } else {
        throw parsing_failure("Invalid factor");
    }
    context.record("因子");
    return r;
}
