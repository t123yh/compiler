//
// Created by t123yh on 2020/10/29.
//

#include "symbol_table.h"

void symbol_table::add_symbol(std::unique_ptr<symbol> item) {
    int l_ptr = -1;
    auto it{lookup.find(item->get_name())};
    if (it != lookup.end()) {
        l_ptr = it->second;
    }
    symbols.push_back(symbol_table_entry{std::move(item), l_ptr, current_layer});
    lookup[symbols.back().item->get_name()] = symbols.size() - 1;
}

void symbol_table::enter_layer() {
    current_layer++;
}

void symbol_table::pop_layer() {
    current_layer--;
    while (!symbols.empty()) {
        if (symbols.back().layer > current_layer) {
            int nxt = symbols.back().ptr_next_same_name;
            if (nxt == -1) {
                lookup.erase(lookup.find(symbols.back().item->get_name()));
            } else {
                lookup[symbols.back().item->get_name()] = nxt;
            }
            symbols.pop_back();
        } else {
            break;
        }
    }
}

symbol* symbol_table::find_symbol(const std::string& name) {
    auto it{lookup.find(name)};
    if (it != lookup.end()) {
        return symbols[it->second].item.get();
    }
    return nullptr;
}

bool symbol_table::check_duplicate(const std::string &name) {
    auto it{lookup.find(name)};
    if (it != lookup.end()) {
        return symbols[it->second].layer == current_layer;
    }
    return false;
}


const std::string &variable_symbol::get_name() const {
    return definition.identifier->text;
}

const std::string &function_symbol::get_name() const {
    return sign.identifier->text;
}
