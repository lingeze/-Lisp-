#include "./eval_env.h"
#include "./error.h"
#include "./builtins.h"
#include "./forms.h"
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
    /* old hard-coded define:
    else if(expr->isPair()){
        std::vector<ValuePtr> v = expr->toVector();
        if(v[0]->asSymbol() == "define"s){
            ...
        }
        else { ... }
    }
    */
    else if(expr->isPair()){
        auto pair = static_cast<PairValue*>(expr.get());
        if (auto name = pair->car()->asSymbol()) {
            if (auto it = SPECIAL_FORMS.find(*name); it != SPECIAL_FORMS.end()) {
                return it->second(pair->cdr()->toVector(), *this);
            }
        }
        // not a special form: evaluate as regular function call
        ValuePtr proc = this->eval(pair->car());
        std::vector<ValuePtr> args = evalList(pair->cdr());
        return apply(proc, args);
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
void EvalEnv::addVariable(const std::string& name, ValuePtr value) {
    symbolTable[name] = value;
}