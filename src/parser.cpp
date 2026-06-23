#include "parser.h"
#include "./error.h"
#include <iostream>
void Parser::print(){
    for (auto& token : tokens) {
        std::cout << *token << std::endl;
    }
    std::cout << std::endl;
}
ValuePtr Parser::parse(){
    if (tokens.empty()) throw SyntaxError("unexpected end of input");
    auto &token = tokens.front();
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<NumericValue>(value);
    }
    else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<BooleanValue>(value);
    }
    else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<StringValue>(value);
    }
    else if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        tokens.pop_front();
        return std::make_shared<SymbolValue>(value);
    }
    else if (token->getType() == TokenType::LEFT_PAREN){
        tokens.pop_front();
        return parseTails();
    }
    else if (token->getType() == TokenType::QUOTE) {
        tokens.pop_front();
        return ToList({std::make_shared<SymbolValue>("quote"), this->parse()});
    }
    else if (token->getType() == TokenType::QUASIQUOTE) {
        tokens.pop_front();
        return ToList({std::make_shared<SymbolValue>("quasiquote"), this->parse()});
    }
    else if (token->getType() == TokenType::UNQUOTE) {
        tokens.pop_front();
        return ToList({std::make_shared<SymbolValue>("unquote"), this->parse()});
    }
    else throw SyntaxError("unexpected token in expression");
}
ValuePtr Parser::parseTails(){
    if (tokens.empty()) throw SyntaxError("unmatched parenthesis");
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = this->parse();
    if (tokens.empty()) throw SyntaxError("unmatched parenthesis");
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        if(tokens.empty() || tokens.front()->getType() != TokenType::RIGHT_PAREN){
            throw SyntaxError("unmatched parenthesis");
        }
        tokens.pop_front();
        return std::make_shared<PairValue> (car, cdr);
    } else {
      auto cdr = this->parseTails();
      return std::make_shared<PairValue> (car, cdr);
    }
    return {};
}