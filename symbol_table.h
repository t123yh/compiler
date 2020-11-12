//
// Created by t123yh on 2020/10/29.
//

#ifndef COMPILER_SYMBOL_TABLE_H
#define COMPILER_SYMBOL_TABLE_H

#include "defs.h"
#include <utility>
#include <vector>
#include <map>
#include <memory>

struct symbol {
    virtual ~symbol() = default;
    
    virtual std::string get_name() const = 0;
};

struct symbol_table_entry {
    std::shared_ptr<symbol> item;
    int ptr_next_same_name;
    int layer;
};

struct variable_symbol : public symbol {
    var_def definition;
    std::string get_name() const override;
    explicit variable_symbol(var_def def) : definition(std::move(def)) {}
};

struct function_symbol : public symbol {
    function_signature sign;
    
    std::string get_name() const override;
    
    explicit function_symbol(function_signature sign) : sign(std::move(sign)) {}
};

struct symbol_table
{
    std::map<std::string, int> lookup;
    std::vector<symbol_table_entry> symbols;
    
    int current_layer;
    void add_symbol(std::shared_ptr<symbol> item);
    
    void enter_layer();
    void pop_layer();
    
    symbol* find_symbol(const std::string& name);
    bool check_duplicate(const std::string& name);
};

#endif //COMPILER_SYMBOL_TABLE_H
