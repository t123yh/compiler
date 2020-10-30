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
    
    error_container errs;
    
    if (!t) {
        std::cerr << "Failed to open file!" << std::endl;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    auto result = tokenize(str, errs);
    
    parsing_context ctx{.start = result.begin(), .current = result.begin(), .end = result.end(), .debug_output = fout, .strategy = FINAL, .errors = errs};
    try {
        volatile auto prog = ctx.expect_one(program_parser());
    } catch (parsing_failure& pf) {
        std::cout << "Failed to parse. reason: " << pf.reason << ". original program: " << std::endl;
        for (auto& x : result) {
            fout << x.pretty_print();
        }
    }
    
    std::sort(errs.begin(), errs.end());
    for (auto& err : errs) {
        std::cout << err.line << " " << (char)err.type << std::endl;
    }
    
    return 0;
}
