//
// Created by t123yh on 2020/10/29.
//

#include "symbol_table.h"
#include "utils.h"

void symbol_table::add_symbol(std::unique_ptr<symbol> item) {
    std::string name = str_to_lower(item->get_name());
    int l_ptr = -1;
    auto it{lookup.find(name)};
    if (it != lookup.end()) {
        l_ptr = it->second;
    }
    symbols.push_back(symbol_table_entry{std::move(item), l_ptr, current_layer});
    lookup[name] = symbols.size() - 1;
}

void symbol_table::enter_layer() {
    current_layer++;
}

void symbol_table::pop_layer() {
    current_layer--;
    while (!symbols.empty()) {
        if (symbols.back().layer > current_layer) {
            int nxt = symbols.back().ptr_next_same_name;
            std::string name = str_to_lower(symbols.back().item->get_name());
            if (nxt == -1) {
                lookup.erase(lookup.find(name));
            } else {
                lookup[name] = nxt;
            }
            symbols.pop_back();
        } else {
            break;
        }
    }
}

symbol* symbol_table::find_symbol(const std::string& name) {
    auto it{lookup.find(str_to_lower(name))};
    if (it != lookup.end()) {
        return symbols[it->second].item.get();
    }
    return nullptr;
}

bool symbol_table::check_duplicate(const std::string &name) {
    auto it{lookup.find(str_to_lower(name))};
    if (it != lookup.end()) {
        return symbols[it->second].layer == current_layer;
    }
    return false;
}

std::string variable_symbol::get_name() const {
    return definition.identifier->text;
}

std::string function_symbol::get_name() const {
    return sign.identifier->text;
}
