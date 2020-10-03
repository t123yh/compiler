#include <iostream>
#include <string>
#include <fstream>
#include "tokenizer.h"
#include "parser.h"
#include "parser_program.h"

int main() {
    std::ifstream t("testfile.txt");
    std::ofstream fout("output.txt");
    if (!t) {
        std::cerr << "Failed to open file!" << std::endl;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    auto result = tokenize(str);
    
    parsing_context ctx {.current = result.begin(), .end = result.end(), .debug_output = fout, .strategy = FINAL};
    volatile auto prog = ctx.expect_one(program_parser());
    
    return 0;
}
