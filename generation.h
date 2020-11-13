//
// Created by t123yh on 2020/11/12.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include "symbol_table.h"
#include "intermediate.h"
#include "base_defs.h"

struct global_generation_context {
    int string_id;
    std::vector<std::tuple<std::string, std::string>> string_table;
    std::string new_string(std::string content);
    symbol_table symbols;
};

struct generation_context
{
    global_generation_context& glob;
    explicit generation_context(global_generation_context& ggc): glob(ggc){
        new_block();
    }
    std::vector<std::shared_ptr<quadruple_block>> blocks;
    std::shared_ptr<quadruple_block> current_block;
    void new_block();
};


#endif //COMPILER_GENERATION_H
