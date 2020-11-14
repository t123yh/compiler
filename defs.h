//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_CODE_DEFS_H
#define COMPILER_CODE_DEFS_H

#include "base_defs.h"
#include "generation.h"

struct expression {
    virtual ~expression() = default;
    virtual std::shared_ptr<intermediate_variable> write_intermediate(generation_context& ctx) = 0;
};

struct variable_access_expression : public expression {
    token_ptr name;
    std::shared_ptr<expression> idx1, idx2;
    std::shared_ptr<intermediate_variable> write_intermediate(generation_context& ctx) override;
};

struct constant_expression : public expression {
    constant_expression(int64_t v, token_type_t type, int line) : val(v), type(type), line(line) {}
    int64_t val;
    token_type_t type; // CHARCON or INTCON
    int line;
    std::shared_ptr<intermediate_variable> write_intermediate(generation_context& ctx) override;
};

struct calculate_expression : public expression {
    typedef token_type_t op_t;
    std::shared_ptr<expression> a, b;
    op_t op;
    std::shared_ptr<intermediate_variable> write_intermediate(generation_context& ctx) override;
};

struct function_call_info {
    token_ptr name;
    std::vector<std::shared_ptr<expression>> arguments;
};

struct calling_expression : public expression {
    std::shared_ptr<function_call_info> call_info;
    std::shared_ptr<intermediate_variable> write_intermediate(generation_context& ctx) override;
};


struct statement {
    int line;
    virtual ~statement() = default;
    virtual void write_intermediate(generation_context& ctx) = 0;
};

struct assignment_statement : public statement {
    token_ptr identifier;
    std::shared_ptr<expression> da, db, val;
    void write_intermediate(generation_context &ctx) override;
};

struct calling_statement : public statement {
    std::shared_ptr<function_call_info> call_info;
    explicit calling_statement(std::shared_ptr<function_call_info>&& call_info) : call_info(std::move(call_info)) {}
    void write_intermediate(generation_context &ctx) override;
};

struct empty_statement : public statement {
    void write_intermediate(generation_context &ctx) override;
};

struct return_statement: public statement {
    std::shared_ptr<expression> val;
    void write_intermediate(generation_context &ctx) override;
    bool is_fucking_return; // return(); is invalid in any case
};

struct print_statement: public statement {
    bool has_string, has_val;
    token_ptr print_content;
    token_type_t val_type;
    std::shared_ptr<expression> print_val;
    void write_intermediate(generation_context &ctx) override;
};

struct scan_statement : public statement {
    token_ptr identifier;
    token_type_t val_type;
    void write_intermediate(generation_context &ctx) override;
};

struct block_statement : public statement {
    std::vector<std::shared_ptr<statement>> statements;
    block_statement(std::vector<std::shared_ptr<statement>> && st) : statements(std::move(st)) {}
    
    void write_intermediate(generation_context &ctx) override;
};

struct statement_block {
    std::vector<var_def> variables;
    void populate_variables(generation_context& ctx);
    std::vector<std::shared_ptr<statement>> statements;
    void generate_statements(generation_context& ctx);
};

struct function {
    function_signature signature;
    void populate_variables(generation_context& ctx);
    statement_block statements;
};

struct condition {
    std::shared_ptr<expression> exp1, exp2;
    token_type_t comparator;
};

struct if_statement : public statement {
    condition cond;
    std::shared_ptr<statement> if_body, else_body;
    void write_intermediate(generation_context &ctx) override;
};

struct while_statement : public statement {
    condition cond;
    std::shared_ptr<statement> body;
    void write_intermediate(generation_context &ctx) override;
};

struct for_statement : public statement {
    token_ptr initial_var;
    std::shared_ptr<expression> initial_exp;
    condition cond;
    
    token_ptr step_set_var;
    token_ptr step_get_var;
    int64_t step_len;
    
    std::shared_ptr<statement> body;
    void write_intermediate(generation_context &ctx) override;
};

struct switch_cond {
    int64_t val;
    std::shared_ptr<statement> body;
};

struct switch_statement : public statement {
    std::shared_ptr<expression> exp;
    std::vector<switch_cond> conditions;
    std::shared_ptr<statement> default_body;
    void write_intermediate(generation_context &ctx) override;
};

struct program {
    void popluate_global_variables(global_generation_context& ggc);
    std::vector<var_def> symbols;
    std::vector<std::shared_ptr<function>> functions;
    statement_block main_function;
};
#endif //COMPILER_CODE_DEFS_H
