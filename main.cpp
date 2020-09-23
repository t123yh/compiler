#include <iostream>
#include <string>
#include <fstream>
#include "tokenizer.h"

int main() {
    std::ifstream t("testfile.txt");
    if (!t) {
        std::cerr << "Failed to open file!" << std::endl;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    
    auto result = tokenize(str);
    std::ofstream fout("output.txt");
    for (auto& t : result) {
        t.pretty_print(fout);
    }
    return 0;
}
