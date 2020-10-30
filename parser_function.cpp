//
// Created by t123yh on 2020/10/3.
//

#include "parser_function.h"
#include "token_parser.h"
#include "parser_const.h"
#include "parser_variable.h"
#include "parser_statement.h"
#include "parser_expression.h"

decl_header_parser::return_type decl_header_parser::parse(parsing_context &context) const {
    function_signature sign;
    
    auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
    sign.return_type = std::get<0>(idf)->type;
    sign.identifier = std::get<1>(idf);
    
    context.record("声明头部");
    return sign;
}

parameter_list_parser::return_type parameter_list_parser::parse(parsing_context &context) const {
    std::vector<var_def> ret;
    if (!context.match(token_parser<RPARENT>())) {
        do {
            auto idf = context.expect(token_parser<INTTK, CHARTK>(), token_parser<IDENFR>());
            ret.push_back(
                    var_def{.array = var_def::PARAM, .type = std::get<0>(idf)->type, .identifier = std::get<1>(idf)});
            if (context.strategy == FINAL) {
                context.add_symbol(make_unique<variable_symbol>(ret.back()));
            }
        } while (context.parse_if_match(token_parser<COMMA>()));
    }
    context.record("参数表");
    return ret;
}

compound_statement_parser::return_type compound_statement_parser::parse(parsing_context &context) const {
    statement_block ret;
    if (context.match(
            token_parser<CONSTTK>(),
            const_definition_parser()
    )) {
        ret.variables = context.expect_one(const_description_parser());
    }
    if (context.match(var_definition_parser())) {
        auto r = context.expect_one(var_description_parser());
        for (auto &x : r) {
            ret.variables.push_back(x);
        }
    }
    ret.statements = context.expect_one(statements_parser());
    context.record("复合语句");
    return ret;
}

statements_parser::return_type statements_parser::parse(parsing_context &context) const {
    return_type ret;
    
    while (!context.match(token_parser<RBRACE>())) {
        ret.push_back(context.expect_one(statement_parser()));
    }
    
    context.record("语句列");
    return ret;
}

calling_parser::return_type calling_parser::parse(parsing_context &context) const {
    auto info = std::unique_ptr<function_call_info>(new function_call_info);
    info->name = context.expect_one(token_parser<IDENFR>());
    context.expect_one(token_parser<LPARENT>());
    info->arguments = context.expect_one(arguments_parser());
    context.expect_one(token_parser<RPARENT>());
    
    if (context.strategy == FINAL) {
        auto func = dynamic_cast<function_symbol*>(context.symbols.find_symbol(info->name->text));
        if (func == nullptr) {
            context.errors.push_back(error{info->name->line, E_UNDEFINED_SYMBOL});
        } else if (func->sign.return_type != VOIDTK) {
            context.record("有返回值函数调用语句");
        } else {
            context.record("无返回值函数调用语句");
        }
    }
    
    return info;
}

arguments_parser::return_type arguments_parser::parse(parsing_context &context) const {
    std::vector<std::unique_ptr<expression>> arg_list;
    if (!context.match(token_parser<RPARENT>())) {
        do {
            arg_list.push_back(context.expect_one(expression_parser()));
        } while (context.parse_if_match(token_parser<COMMA>()));
    }
    context.record("值参数表");
    return arg_list;
}

void find_all_return(std::vector<return_statement*> &results, const std::unique_ptr<statement> &statement) {
#define CHK(VAR, TYPE) auto VAR = dynamic_cast<TYPE*>(statement.get()); if (VAR)
    CHK(a1, return_statement) {
        results.push_back(a1);
    }
    CHK(a2, block_statement) {
        for (const auto &v : a2->statements) {
            find_all_return(results, v);
        }
    }
    CHK(a3, if_statement) {
        find_all_return(results, a3->if_body);
        find_all_return(results, a3->else_body);
    }
    CHK(a4, while_statement) {
        find_all_return(results, a4->body);
    }
    CHK(a5, switch_statement) {
        for (const auto& v : a5->conditions) {
            find_all_return(results, v.body);
        }
        find_all_return(results, a5->default_body);
    }
#undef CHK
}

void find_all_return(std::vector<return_statement*>& results, const std::vector<std::unique_ptr<statement>> &statements) {
    for (const auto& s : statements) {
        find_all_return(results, s);
    }
}

function_parser::return_type function_parser::parse(parsing_context &context) const {
    std::unique_ptr<function> func = std::unique_ptr<function>(new function);
    function_signature &sign = func->signature;
    if (context.parse_if_match(token_parser<VOIDTK>())) {
        sign.return_type = VOIDTK;
        sign.identifier = context.expect_one(token_parser<IDENFR>());
    } else {
        sign = context.expect_one(decl_header_parser());
    }
    
    if (context.strategy == FINAL) {
        context.add_symbol(make_unique<function_symbol>(sign));
    }
    
    auto s = dynamic_cast<function_symbol*>(context.symbols.symbols.back().item.get());
    
    if (context.strategy == FINAL) {
        context.symbols.enter_layer();
    }
    
    sign.parameters = std::get<1>(
            context.expect(token_parser<LPARENT>(), parameter_list_parser(), token_parser<RPARENT>()));
    s->sign = sign;
    
    func->statements = std::get<1>(
            context.expect(token_parser<LBRACE>(), compound_statement_parser(), token_parser<RBRACE>()));
    
    std::vector<return_statement*> returns;
    find_all_return(returns, func->statements.statements);
    if (func->signature.return_type == VOIDTK) {
        for (auto* r : returns) {
            if (r->is_fucking_return || r->val != nullptr) {
                context.errors.push_back(error{r->line, E_RETURN_MISMATCH_FOR_VOID_FUNCTIONS});
            }
        }
        context.record("无返回值函数定义");
    } else {
        if (returns.empty()) {
            context.errors.push_back(error{context.prev_line(), E_RETURN_MISMATCH_FOR_RETURNING_FUNCTIONS});
        } else for (auto* r : returns) {
            bool type_mismatch = get_expression_type(r->val.get()) != sign.return_type;
            
            if (r->is_fucking_return || r->val == nullptr || type_mismatch) {
                context.errors.push_back(error{r->line, E_RETURN_MISMATCH_FOR_RETURNING_FUNCTIONS});
            }
            
        }
        
        context.record("有返回值函数定义");
    }
    
    if (context.strategy == FINAL) {
        context.symbols.pop_layer();
    }
    return func;
}

main_function_parser::return_type main_function_parser::parse(parsing_context &context) const {
    if (context.strategy == FINAL) {
        context.symbols.enter_layer();
    }
    auto tks = context.expect(token_parser<VOIDTK>(), token_parser<MAINTK>(), token_parser<LPARENT>(),
                              token_parser<RPARENT>());
    function_signature sign{.return_type = VOIDTK, .identifier = std::get<0>(tks), .parameters = {}};
    auto r = std::get<1>(context.expect(token_parser<LBRACE>(), compound_statement_parser(), token_parser<RBRACE>()));
    context.record("主函数");
    
    if (context.strategy == FINAL) {
        context.symbols.pop_layer();
    }
    return r;
}
