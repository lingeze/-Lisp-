#include "./eval_env.h"
#include "./error.h"
#include "./builtins.h"
#include "./builtin_func_args.h"
#include "./forms.h"
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std::literals;
std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    return std::shared_ptr<EvalEnv>(new EvalEnv());
}
ValuePtr EvalEnv::eval(ValuePtr expr){
    if(expr->isNil()){
        throw LispError("cannot evaluate nil");
    }
    else if(expr->isSelfEvaluating()){
        return expr;
    }
    else if(expr->isPair()){
        auto pair = expr->asPair();
        if (auto name = pair->car()->asSymbol()) {
            if (auto it = SPECIAL_FORMS.find(*name); it != SPECIAL_FORMS.end()) {
                return it->second(pair->cdr()->toVector(), *this);
            }
        }
        ValuePtr proc = this->eval(pair->car());
        std::vector<ValuePtr> args = evalList(pair->cdr());
        return apply(proc, args);
    }
    else if(auto name = expr->asSymbol()){
        return lookupBinding(expr);
    }
    else{
        throw LispError("cannot evaluate expression of this type");
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
    if (auto func = std::dynamic_pointer_cast<BuiltinProcValue>(proc)){
        FuncArgs funcArgs(args, shared_from_this());
        return func->call(funcArgs);
    }
    else if(auto func = std::dynamic_pointer_cast<LambdaValue>(proc)){
        return func->apply(args);
    }
    else {
        throw LispError("cannot apply: expected a procedure");
    }
}
EvalEnv::EvalEnv(){
    for (const auto& [name, func] : getBuiltins()) {
        symbolTable[name] = std::make_shared<BuiltinProcValue>(func);
    }
}
void EvalEnv::set_parent(const std::shared_ptr<EvalEnv>& parentEnv) {
    parent = parentEnv;
}
void EvalEnv::addVariable(const std::string& name, ValuePtr value) {
    symbolTable[name] = value;
}
const std::unordered_map<std::string, ValuePtr>& EvalEnv::getSymbolTable() const {
    return symbolTable;
}
ValuePtr EvalEnv::lookupBinding(ValuePtr expr) {
    auto name = expr->asSymbol();
    if (symbolTable.find(*name) != symbolTable.end()) {
        auto value = symbolTable[*name];
        return value;
    }else if(parent){
        return parent->lookupBinding(expr);
    }
    else{
        throw LispError("variable " + *name + " is not defined");
    }
}
std::shared_ptr<EvalEnv> EvalEnv::createChild
(const std::vector<std::string>& params, const std::vector<ValuePtr>& args){
    if(params.size()!=args.size()){
        throw LispError("cannot create environment: parameter count mismatch");
    }
    auto env = std::shared_ptr<EvalEnv>(new EvalEnv());
    env->set_parent(shared_from_this());
    int len = params.size();
    for (int i = 0; i < len; i ++ ){
        env->addVariable(params[i], args[i]);
    }
    return env;
}
