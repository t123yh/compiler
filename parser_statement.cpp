//
// Created by t123yh on 2020/10/3.
//

#include <memory>
#include "parser_statement.h"
#include "token_parser.h"
#include "parser_expression.h"
#include "parser_function.h"
#include "parser_complex_statement.h"

statement_parser::return_type statement_parser::parse(parsing_context &context) const {
    statement_parser::return_type ret;
    if (context.match(assignment_parser())) {
        ret = context.expect_one(assignment_parser());
        context.expect_one(token_parser<SEMICN>());
    } else if (context.match(calling_parser())) {
        ret = std::unique_ptr<calling_statement>(new calling_statement(context.expect_one(calling_parser())));
        context.expect_one(token_parser<SEMICN>());
    } else if (context.parse_if_match(token_parser<SEMICN>())) {
        ret = std::unique_ptr<empty_statement>(new empty_statement);
    } else if (context.parse_if_match(token_parser<LBRACE>())) {
        ret = std::unique_ptr<block_statement>(new block_statement(context.expect_one(statements_parser())));
        context.expect_one(token_parser<RBRACE>());
    } else if (context.match(return_parser())) {
        ret = context.expect_one(return_parser());
        context.expect_one(token_parser<SEMICN>());
    // } else if (context.match(print_parser())) {
    } else if (context.match(token_parser<PRINTFTK>())) {
        ret = context.expect_one(print_parser());
        context.expect_one(token_parser<SEMICN>());
    // } else if (context.match(scan_parser())) {
    } else if (context.match(token_parser<SCANFTK>())) {
        ret = context.expect_one(scan_parser());
        context.expect_one(token_parser<SEMICN>());
    } else if (context.match(token_parser<IFTK>())) {
        ret = context.expect_one(if_parser());
    } else if (context.match(token_parser<WHILETK>())) {
        ret = context.expect_one(while_parser());
    } else if (context.match(token_parser<FORTK>())) {
        ret = context.expect_one(for_parser());
    } else if (context.match(token_parser<SWITCHTK>())) {
        ret = context.expect_one(switch_parser());
    } else {
        throw parsing_failure("Unable to parse statement");
    }
    context.record("语句");
    return ret;
}


assignment_parser::return_type assignment_parser::parse(parsing_context &context) const {
    std::unique_ptr<assignment_statement> ass = std::unique_ptr<assignment_statement>(new assignment_statement);
#define SCALAR token_parser<IDENFR>(), token_parser<ASSIGN>(), expression_parser()
#define VEC1D token_parser<IDENFR>(), token_parser<LBRACK>(), expression_parser(), token_parser<RBRACK>(true), token_parser<ASSIGN>(), expression_parser()
#define VEC2D token_parser<IDENFR>(), token_parser<LBRACK>(), expression_parser(), token_parser<RBRACK>(true), token_parser<LBRACK>(), expression_parser(), token_parser<RBRACK>(true), token_parser<ASSIGN>(), expression_parser()
    if (context.match(SCALAR)) {
        auto s = context.expect(SCALAR);
        ass->line = std::get<1>(s)->line;
        ass->identifier = std::get<0>(s);
        ass->val = std::move(std::get<2>(s));
    } else if (context.match(VEC1D)) {
        auto s = context.expect(VEC1D);
        ass->line = std::get<4>(s)->line;
        ass->identifier = std::get<0>(s);
        ass->da = std::move(std::get<2>(s));
        is_charcon(ass->da, context, E_INDEX_N_CHAR);
        ass->val = std::move(std::get<5>(s));
    } else if (context.match(VEC2D)) {
        auto s = context.expect(VEC2D);
        ass->identifier = std::get<0>(s);
        ass->line = std::get<7>(s)->line;
        ass->da = std::move(std::get<2>(s));
        is_charcon(ass->da, context, E_INDEX_N_CHAR);
        ass->db = std::move(std::get<5>(s));
        is_charcon(ass->db, context, E_INDEX_N_CHAR);
        ass->val = std::move(std::get<8>(s));
    } else {
        throw parsing_failure("Un");
    }
#undef SCALAR
#undef VEC1D
#undef VEC2D
    
    if (context.strategy == FINAL) {
        symbol* sss = context.symbols.find_symbol(ass->identifier->text);
        auto* symb = dynamic_cast<variable_symbol*>(sss);
        if (symb == nullptr) {
            context.errors.push_back(error{ass->identifier->line, E_UNDEFINED_SYMBOL});
        } else if (symb->definition.array == var_def::CONST) {
            context.errors.push_back(error{ass->identifier->line, E_ASSIGN_TO_CONST});
        }
    }
    
    context.record("赋值语句");
    return ass;
}

return_parser::return_type return_parser::parse(parsing_context &context) const {
    context.expect_one(token_parser<RETURNTK>());
    auto r = std::unique_ptr<return_statement>(new return_statement);
    r->is_fucking_return = false;
    r->line = context.line();
    if (context.parse_if_match(token_parser<LPARENT>())) {
        if (context.parse_if_match(token_parser<RPARENT>())) {
            // return();
            r->is_fucking_return = true;
            r->val = std::unique_ptr<expression>(nullptr);
        } else {
            r->val = context.expect_one(expression_parser());
            context.expect_one(token_parser<RPARENT>());
        }
    } else {
        r->val = std::unique_ptr<expression>(nullptr);
    }
    context.record("返回语句");
    return r;
}

print_parser::return_type print_parser::parse(parsing_context &context) const {
    auto pt = std::unique_ptr<print_statement>(new print_statement);
    pt->line = context.line();
    context.expect(token_parser<PRINTFTK>(), token_parser<LPARENT>());
    if (context.match(token_parser<STRCON>(), token_parser<COMMA>(), expression_parser())) {
        pt->print_content = context.expect_one(token_parser<STRCON>());
        context.record("字符串");
        context.expect(token_parser<COMMA>());
        pt->print_val = context.expect_one(expression_parser());
        pt->has_string = true;
        pt->has_val = true;
    } else if (context.match(token_parser<STRCON>())) {
        pt->has_val = false;
        pt->has_string = true;
        pt->print_content = context.expect_one(token_parser<STRCON>());
        context.record("字符串");
    } else if (context.match(expression_parser())) {
        pt->has_val = true;
        pt->has_string = false;
        pt->print_val = context.expect_one(expression_parser());
    } else {
        throw parsing_failure("Malformed print statement");
    }
    
    context.expect(token_parser<RPARENT>());
    context.record("写语句");
    return pt;
}

scan_parser::return_type scan_parser::parse(parsing_context &context) const {
    auto s = std::unique_ptr<scan_statement>(new scan_statement);
    s->line = context.line();
    s->identifier = std::get<2>(context.expect(token_parser<SCANFTK>(), token_parser<LPARENT>(), token_parser<IDENFR>(),
                                               token_parser<RPARENT>()));
    auto* sb = dynamic_cast<variable_symbol*>(context.symbols.find_symbol(s->identifier->text));
    if (sb == nullptr) {
        context.errors.push_back(error{s->identifier->line, E_UNDEFINED_SYMBOL});
    } else if (sb->definition.array== var_def::CONST) {
        context.errors.push_back(error{s->identifier->line, E_ASSIGN_TO_CONST});
    }
    context.record("读语句");
    return s;
}
