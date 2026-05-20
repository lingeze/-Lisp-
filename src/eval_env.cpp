#include "./eval_env.h"
#include "./error.h"
#include <iostream>
using namespace std::literals;
ValuePtr EvalEnv::eval(ValuePtr expr){
    if(expr->isNil()){
        throw LispError("Evaluating nil is prohibited.");
    }
    else if(expr->isSelfEvaluating()){
        return expr;
    }
    else if(expr->isList()){
        std::vector<ValuePtr> v = expr->toVector();
        if(v[0]->asSymbol() == "define"s){
            if (v.size() != 4) {
                throw LispError("expected 2 arguments, got " + std::to_string(v.size() - 2));
            }
            if(auto name = v[1]->asSymbol()){
                symbolTable[*name] = eval(v[2]);
                return std::make_shared<NilValue>();
            }
        }
        else {
            throw LispError("Malformed define.");    
        }
    }
    else if(auto name = expr->asSymbol()){
        if (symbolTable.find(*name) != symbolTable.end()) {
            auto value = symbolTable[*name];
            return value;
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    }
    else{
        throw LispError("Unimplemented");
    }
    return {};
}