#ifndef PARSER_H
#define PARSER_H
#include "value.h"
#include <deque>
#include "token.h"
class Parser{
private:
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens):tokens{std::move(tokens)}{

    }
    bool empty() const { return tokens.empty(); }
    ValuePtr parse();
    ValuePtr parseTails();
    void print();
};
#endif