#include "./value.h"
#include "./error.h"
#include "./eval_env.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>
#include "value.h"
bool Value::isNil(){
    return typeid(*this) == typeid(NilValue);
}
bool Value::isSelfEvaluating(){
    return (typeid(*this) == typeid(StringValue))
    || (typeid(*this) == typeid(NumericValue)) 
    || (typeid(*this) == typeid(BooleanValue))
    || (typeid(*this) == typeid(BuiltinProcValue));
}
bool Value::isPair(){
    return typeid(*this) == typeid(PairValue);
}
bool Value::isList(){
    if (isNil()) return true;
    if (!isPair()) return false;
    ValuePtr cur = shared_from_this();
    while (cur->isPair())
        cur = cur->asPair()->cdr();
    return cur->isNil();
}
bool Value::isString(){
    return typeid(*this) == typeid(StringValue);
}
bool Value::isNumber(){
    return typeid(*this) == typeid(NumericValue);
}
bool Value::isSymbol(){
    return typeid(*this) == typeid(SymbolValue);
}
bool Value::isInteger() {
    if(!isNumber())return false;
    return std::floor(asNumber()) == asNumber();
}
bool Value::isPromise() {
    return typeid(*this) == typeid(PromiseValue);
}
bool Value::isBool() {
    return typeid(*this) == typeid(BooleanValue);
}
bool Value::isProcedure() {
    return typeid(*this) == typeid(BuiltinProcValue) || typeid(*this) == typeid(LambdaValue);
}
std::optional<std::string> Value::tryAsSymbol(){
    if(!isSymbol())return std::nullopt;
    return std::dynamic_pointer_cast<SymbolValue>(shared_from_this())->toString();
}
double Value::asNumber(){
    if(!isNumber())throw(LispError("Not a number"));
    return std::dynamic_pointer_cast<NumericValue>(shared_from_this())->getValue();
}
bool Value::asBool(){
    if(!isBool())throw(LispError("Not a boolean"));
    return std::dynamic_pointer_cast<BooleanValue>(shared_from_this())->getValue();
}
std::shared_ptr<PairValue> Value::asPair(){
    if(!isPair())throw(LispError("Not a pair"));
    return std::dynamic_pointer_cast<PairValue>(shared_from_this());
}
std::shared_ptr<PromiseValue> Value::asPromise(){
    if(!isPromise())throw(LispError("Not a promise"));
    return std::dynamic_pointer_cast<PromiseValue>(shared_from_this());
}
std::vector<ValuePtr> Value::toVector(){
    if(isNil())return {};
    if(!isPair()){
        return {shared_from_this()};
    }
    std::vector<ValuePtr> v;
    ValuePtr cur = shared_from_this();
    while (true) {
        auto pair = std::dynamic_pointer_cast<PairValue>(cur);
        if (!pair) throw LispError("Invalid list structure");
        v.push_back(pair->car());
        cur = pair->cdr();
        if (!cur->isPair()) {
            if(cur->isNil())break;
            v.push_back(cur);
            break;
        }
    }
    return v;
}
std::string BooleanValue::toString(){
    return value ? "#t" : "#f";
}
std::string NumericValue::toString(){
    if(std::floor(value) == value)return std::to_string((long long)(value));
    return std::to_string(value);
}
std::string StringValue::toString(){
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}
std::string NilValue::toString(){
    return "()";
}
std::string SymbolValue::toString(){
    return value;
}
std::string BuiltinProcValue::toString(){
    return "#<procedure>";
}
ValuePtr BuiltinProcValue::call(FuncArgs& args){
    return value(args);
}
static std::string noquoted(std::string s){
    if(s.empty()) return s;
    int len = s.size();
    if(s[0] == '(' && s[len-1] == ')')s = s.substr(1,len - 2);
    return s;
}
std::string PairValue::toString(){
    std::string s{};
    std::string s1 = lptr->toString();
    std::string s2 = noquoted(rptr->toString());
    if(typeid(*rptr) != typeid(PairValue) && typeid(*rptr) != typeid(NilValue)){
        s2 = ". " + s2;
    }
    std::string connect = s2.size() ? " " : "";
    return "(" + s1 + connect + s2 + ")";
}
ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    auto env = parent->createChild(params, args);
    ValuePtr ret = nullptr;
    for(auto mem:body){
        ret = env->eval(mem);
    }
    return ret;
}
std::string LambdaValue::toString() {
    return "#<procedure>";
}

ValuePtr ToList(const std::vector<ValuePtr> &ptrs){
    int len = ptrs.size();
    if(!len)return std::make_shared<NilValue>();
    auto ret = std::make_shared<PairValue>(ptrs[len-1], std::make_shared<NilValue>());
    for(int i = len - 2; i >= 0; i--){
        ret = std::make_shared<PairValue>(ptrs[i], ret);
    }
    return ret;
}
ValuePtr PromiseValue::force() {
    if(evaluated)return value;
    ValuePtr result{};
    result = env->eval(value);
    if (forceResult) {
        if(result->isPromise()){
            result = result->asPromise()->force();
        }
    }
    value = result;
    evaluated = true;
    return value;
}
std::string PromiseValue::toString() {
    return "#<promise>";
}