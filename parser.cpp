//
// Created by t123yh on 2020/9/29.
//

#include <fstream>
#include "parser.h"
#include "tokenizer.h"
#include "parsing_failure.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <array>

std::ofstream nullstream;
error_container garbage;

parsing_context parsing_context::fork_trial() {
    nullstream.setstate(std::ios_base::badbit);
    return parsing_context{current, end, nullstream, TRIAL, garbage};
}

void parsing_context::advance() {
    if (current != end)
        current++;
    else
        throw parsing_failure("failed to advance: end reached");
}

void parsing_context::record(const char* text) {
    debug_output << "<" << text << ">" << std::endl;
}

const char* parser::get_name() const {
    return "";
}
