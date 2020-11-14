#include <iostream>
#include <string>
#include <fstream>
#include "tokenizer.h"
#include "parser.h"
#include "parser_program.h"
#include "errors.h"

int main() {
    std::ifstream t("testfile.txt");
    std::ofstream fout("output.txt");
    std::ofstream eout("error.txt");
    
    error_container errs;
    
    if (!t) {
        std::cerr << "Failed to open file!" << std::endl;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    auto result = tokenize(str, errs);
    
    parsing_context ctx{.start = result.begin(), .current = result.begin(), .end = result.end(), .debug_output = fout, .strategy = FINAL, .errors = errs};
    try {
        program p = ctx.expect_one(program_parser());
        std::sort(errs.begin(), errs.end());
        for (auto& err : errs) {
            eout << err.line << " " << (char)err.type << std::endl;
        }
    
        if (errs.empty()) {
            global_generation_context ggc{};
            p.popluate_global_variables(ggc);
            generation_context main_ctx(ggc, "main");
            
            p.main_function.populate_variables(main_ctx);
            p.main_function.generate_statements(main_ctx);
            std::vector<std::string> shit;
            main_ctx.assign_stack_space();
            main_ctx.generate_mips(shit);
    
    
            std::cout<< ".data" << std::endl;
            for (auto& s : ggc.string_table) {
                std::cout << std::get<0>(s) << ": .asciiz \"" << std::get<1>(s) << "\"" << std::endl;
            }
            for (auto& v : ggc.variables) {
                std::cout << v.first << ": .word " << v.second.value << std::endl;
            }
            
            std::cout<< ".text" << std::endl;
            for (auto& x : shit) {
                std::cout << x << std::endl;
            }
            asm("nop");
            // generation_context ctx;
        }
        
    } catch (parsing_failure& pf) {
        std::cout << "Failed to parse. reason: " << pf.reason << ". original program: " << std::endl;
        for (auto& x : result) {
            // fout << x.pretty_print();
        }
    }
    
    
    
    return 0;
}
