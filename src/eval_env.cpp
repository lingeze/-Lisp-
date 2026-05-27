#include "./eval_env.h"
#include "./error.h"
#include "./builtins.h"
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std::literals;
ValuePtr EvalEnv::eval(ValuePtr expr){
    //std::cout << expr->toString() << std::endl; 
    //std::cout << "eval:" << expr->isNil() << std::endl;
    if(expr->isNil()){
        throw LispError("Evaluating nil is prohibited.");
    }
    else if(expr->isSelfEvaluating()){
        return expr;
    }
    else if(expr->isPair()){
        std::vector<ValuePtr> v = expr->toVector();
        if(v[0]->asSymbol() == "define"s){
            if (v.size() != 3) {
                throw LispError("expected 2 arguments, got " + std::to_string(v.size() - 1));
            }
            if(auto name = v[1]->asSymbol()){
                symbolTable[*name] = eval(v[2]);
                return std::make_shared<NilValue>();
            }
        }
        else {
            ValuePtr proc = this->eval(v[0]);
            //std::cout << "proc:" << proc->toString() << std::endl;
            std::vector<ValuePtr> args = evalList(std::dynamic_pointer_cast<PairValue>(expr)->cdr());
            //std::cout << "evallist end" << std::endl;
            return apply(proc, args);
            //throw LispError("Malformed define.");    
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
std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    if (expr->isNil()) {
        return {};
    }
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        auto func = std::dynamic_pointer_cast<BuiltinProcValue>(proc);
        return func->call(args);
    } else {
        throw LispError("Unimplemented");
    }
}
EvalEnv::EvalEnv(){
    for (const auto& [name, func] : getBuiltins()) {
        symbolTable[name] = std::make_shared<BuiltinProcValue>(func);
    }
}