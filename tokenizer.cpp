//
// Created by t123yh on 2020/9/23.
//

#include <iostream>
#include <regex>
#include <cassert>
#include "tokenizer.h"
#include "utils.h"

#pragma clang diagnostic ignored "-Wc99-designator"
const char* const symbol_names[] = {
        [ERROR]="<unrecognized token>",
        [IDENFR]="IDENFR",
        [INTCON]="INTCON",
        [CHARCON]="CHARCON",
        [STRCON]="STRCON",
        [CONSTTK]="CONSTTK",
        [INTTK]="INTTK",
        [CHARTK]="CHARTK",
        [VOIDTK]="VOIDTK",
        [MAINTK]="MAINTK",
        [IFTK]="IFTK",
        [ELSETK]="ELSETK",
        [SWITCHTK]="SWITCHTK",
        [CASETK]="CASETK",
        [DEFAULTTK]="DEFAULTTK",
        [WHILETK]="WHILETK",
        [FORTK]="FORTK",
        [SCANFTK]="SCANFTK",
        [PRINTFTK]="PRINTFTK",
        [RETURNTK]="RETURNTK",
        [PLUS]="PLUS",
        [MINU]="MINU",
        [MULT]="MULT",
        [DIV]="DIV",
        [LSS]="LSS",
        [LEQ]="LEQ",
        [GRE]="GRE",
        [GEQ]="GEQ",
        [EQL]="EQL",
        [NEQ]="NEQ",
        [COLON]="COLON",
        [ASSIGN]="ASSIGN",
        [SEMICN]="SEMICN",
        [COMMA]="COMMA",
        [LPARENT]="LPARENT",
        [RPARENT]="RPARENT",
        [LBRACK]="LBRACK",
        [RBRACK]="RBRACK",
        [LBRACE]="LBRACE",
        [RBRACE]="RBRACE",
};

std::string token::pretty_print() const {
    return std::string(token_name(this->type)) + " " + this->text + "\n";
}

std::string token::type_name() {
    return std::string(symbol_names[this->type]);
}

static token_type_t get_token_type(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "-") {
        return MINU;
    } else if (s == "+") {
        return PLUS;
    } else if (s == "/") {
        return DIV;
    } else if (s == "*") {
        return MULT;
    } else if (s == "<") {
        return LSS;
    } else if (s == "<=") {
        return LEQ;
    } else if (s == ">") {
        return GRE;
    } else if (s == ">=") {
        return GEQ;
    } else if (s == "!=") {
        return NEQ;
    } else if (s == ":") {
        return COLON;
    } else if (s == "=") {
        return ASSIGN;
    } else if (s == "==") {
        return EQL;
    } else if (s == ";") {
        return SEMICN;
    } else if (s == ",") {
        return COMMA;
    } else if (s == "(") {
        return LPARENT;
    } else if (s == ")") {
        return RPARENT;
    } else if (s == "[") {
        return LBRACK;
    } else if (s == "]") {
        return RBRACK;
    } else if (s == "{") {
        return LBRACE;
    } else if (s == "}") {
        return RBRACE;
    } else if (s == "const") {
        return CONSTTK;
    } else if (s == "int") {
        return INTTK;
    } else if (s == "void") {
        return VOIDTK;
    } else if (s == "main") {
        return MAINTK;
    } else if (s == "if") {
        return IFTK;
    } else if (s == "else") {
        return ELSETK;
    } else if (s == "switch") {
        return SWITCHTK;
    } else if (s == "case") {
        return CASETK;
    } else if (s == "default") {
        return DEFAULTTK;
    } else if (s == "while") {
        return WHILETK;
    } else if (s == "for") {
        return FORTK;
    } else if (s == "char") {
        return CHARTK;
    } else if (s == "printf") {
        return PRINTFTK;
    } else if (s == "return") {
        return RETURNTK;
    } else if (s == "chartk") {
        return CHARTK;
    } else if (s == "scanf") {
        return SCANFTK;
    } else if (std::regex_match(s, std::regex("\".*\""))) {
        return STRCON;
    } else if (std::regex_match(s, std::regex("'.'"))) {
        return CHARCON;
    } else if (std::regex_match(s, std::regex("\\d+"))) {
        return INTCON;
    } else if (std::regex_match(s, std::regex("\\w+"))) {
        return IDENFR;
    } else {
        return ERROR;
    }
}

std::vector<token> tokenize(std::string orig) {
    std::regex re(R"(\".*?\"|'.'|\*|\+|-|\d+|\w+|\/|<=|<|>=|>|==|!=|:|=|;|,|\(|\)|\[|\]|\{|\}|\S+?)");
    
    
    std::sregex_iterator iter(orig.begin(), orig.end(), re);
    std::sregex_iterator end;
    
    std::vector<token> result;
    int line = 1, lastpos = 0, linestart = 0;
    while (iter != end) {
        std::string txt = (*iter)[0];
        token_type_t type = get_token_type(txt);
        int pos = iter->position(0);
        while (lastpos < pos) {
            lastpos++;
            if (orig[lastpos] == '\n') {
                linestart = lastpos;
                line++;
            }
        }
        
        if (type == STRCON || type == CHARCON) {
            txt = txt.substr(1, txt.size() - 2);
        }
        
        result.push_back(token{.type = type, .text = txt, .line = line, .column = pos - linestart});
        iter++;
    }
    
    return result;
}

const char* token_name(token_type_t type) {
    assert(type < COUNT_OF(symbol_names));
    return symbol_names[type];
}
