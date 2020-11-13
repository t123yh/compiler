//
// Created by t123yh on 2020/11/12.
//

#include "generation.h"

void generation_context::new_block() {
    auto nb = std::make_shared<quadruple_block>();
    this->blocks.push_back(nb);
    this->current_block = nb;
}

std::string global_generation_context::new_string(std::string content) {
    string_id++;
    string_table.emplace_back(std::string("str" + std::to_string(string_id)), content);
    return std::get<0>(string_table.back());
}
