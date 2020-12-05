#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include "tokenizer.h"
#include "parser.h"
#include "parser_program.h"
#include "errors.h"

std::string str_replace(
        std::string& s,
        const std::string& f,
        const std::string& r)
{
    std::size_t pos = s.find(f);
    if (pos == std::string::npos) return s;
    return s.replace(pos, f.length(), r);
}


int main() {
    std::ifstream t("testfile.txt");
    std::ofstream code("mips.txt");
    
    error_container errs;
    
    if (!t) {
        std::cerr << "Failed to open file!" << std::endl;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    auto result = tokenize(str, errs);
    
    parsing_context ctx{.start = result.begin(), .current = result.begin(), .end = result.end(), .debug_output = nullstream, .strategy = FINAL, .errors = errs};
    try {
        program p = ctx.expect_one(program_parser());
        std::sort(errs.begin(), errs.end());
        /*
        for (auto& err : errs) {
            eout << err.line << " " << (char)err.type << std::endl;
        }
         */
    
        if (errs.empty()) {
            std::vector<std::string> shit;
            global_generation_context ggc{};
            p.popluate_global_variables(ggc);
            
            for (auto& f : p.functions) {
                generation_context func_ctx(ggc, f->signature.identifier->text);
                f->populate_parameters(func_ctx);
                f->statements.populate_variables(func_ctx);
                f->statements.generate_statements(func_ctx);
                
                func_ctx.assign_stack_space();
                func_ctx.generate_mips(shit);
            }
            
            generation_context main_ctx(ggc, "main");
            
            p.main_function.populate_variables(main_ctx);
            p.main_function.generate_statements(main_ctx);
            main_ctx.assign_stack_space();
            main_ctx.generate_mips(shit);
    
    
            code << ".data" << std::endl;
            for (auto& s : ggc.string_table) {
                
                code << std::get<0>(s) << ": .asciiz \"" <<
                        str_replace(std::get<1>(s), "\\", "\\\\") << "\"" << std::endl;
            }
            for (auto& v : ggc.variables) {
                if (v.second.array == var_def::SCALAR_VAR) {
                    code << v.first << ":.align 2 \n.word " << v.second.value << std::endl;
                } else if (v.second.array == var_def::ARRAY_1D || v.second.array == var_def::ARRAY_2D) {
                    if (v.second.initialization) {
                        code << v.first << ":.align 2\n";
                        for (auto init : *v.second.initialization) {
                            code << ".word " << std::to_string(init) << std::endl;
                        }
                        code << std::endl;
                    } else {
                        code << v.first << ": .align 2 \n.space " << v.second.get_size() * 4 << std::endl;
                    }
                } else if (v.second.array != var_def::CONST) {
                    throw std::logic_error("Fucked");
                }
            }
            
            code << ".text" << std::endl;
            code << "jal func_main" << std::endl;
            code << "li $v0, 10" << std::endl;
            code << "syscall" << std::endl;
            
            for (auto& x : shit) {
                code << x << std::endl;
            }
            asm("nop");
            // generation_context ctx;
        } else {
            throw parsing_failure("has " + std::to_string(errs.size()) + " errors!");
        }
        
    } catch (parsing_failure& pf) {
        std::cout << "Failed to parse. reason: " << pf.reason << ". original program: " << std::endl;
        kill(getpid(), SIGTRAP);
    }
    
    
    
    return 0;
}
