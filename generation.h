//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include <list>
#include "symbol_table.h"
#include "intermediate.h"
#include "base_defs.h"

struct global_generation_context {
    int string_id;
    std::vector<std::tuple<std::string, std::string>> string_table;
    std::string new_string(std::string content);
    std::map<std::string, var_def> variables;
    void insert_variable(var_def v);
};

struct generation_context
{
    std::string name;
    
    int block_id = 0;
    std::list<std::shared_ptr<quadruple_block>> blocks;
    std::shared_ptr<quadruple_block> current_block;
    
    std::list<std::shared_ptr<intermediate_variable>> temp_vars;
    std::shared_ptr<intermediate_variable> new_temp_var();
    std::map<std::string, std::tuple<var_def, std::shared_ptr<intermediate_variable>>> variables;
    void insert_variable(var_def v);
    void insert_parameter(var_def v);
    
    void new_block();
    int assign_stack_space();
    
    void generate_mips(std::vector<std::string>& result);
    
    global_generation_context& glob;
    explicit generation_context(global_generation_context& ggc, std::string name): glob(ggc), name(std::move(name)){
        new_block();
    }
};


#endif //COMPILER_GENERATION_H
