//
// Created by t123yh on 2020/9/23.
//

#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#include <string>
#include <cstdint>
#include <vector>

enum token_type_t
{
    ERROR = 0, IDENFR = 1, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK,
    VOIDTK, MAINTK, IFTK, ELSETK, SWITCHTK, CASETK, DEFAULTTK, WHILETK,
    FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS, MINU, MULT, DIV, LSS, LEQ,
    GRE, GEQ, EQL, NEQ, COLON, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT,
    LBRACK, RBRACK, LBRACE, RBRACE
};

const char* token_name(token_type_t);

struct token
{
    token_type_t type;
    std::string text;
    int line;
    int column;
    
    std::string type_name();
    
    std::string pretty_print() const;
};

std::vector<token> tokenize(std::string orig);

#endif //COMPILER_TOKENIZER_H
