#include "./value.h"
#include "./error.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>
bool Value::isNil(){
    return typeid(*this) == typeid(NilValue);
}
bool Value::isSelfEvaluating(){
    return (typeid(*this) == typeid(StringValue))
    || (typeid(*this) == typeid(NumericValue)) 
    || (typeid(*this) == typeid(BooleanValue))
    || (typeid(*this) == typeid(BuiltinProcValue));
}
bool Value::isList(){
    return typeid(*this) == typeid(PairValue);
}
bool Value::isNumber(){
    return typeid(*this) == typeid(NumericValue);
}
int Value::asNumber(){
    if(!isNumber())throw(LispError("Not a number"));
    return std::stoi(toString());
}
std::vector<ValuePtr> Value::toVector(){
    if(!isList()){
        throw(LispError("Cannot transform to vector"));
    }
    std::vector<ValuePtr> v;
    ValuePtr cur = shared_from_this();
    while (true) {
        auto pair = std::dynamic_pointer_cast<PairValue>(cur);
        if (!pair) throw LispError("Invalid list structure");
        v.push_back(pair->car());
        cur = pair->cdr();
        if (!cur->isList()) {
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
ValuePtr BuiltinProcValue::call(std::vector<ValuePtr> args){
    return value(args);
}
static std::string noquoted(std::string s){
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
ValuePtr ToList(std::vector<ValuePtr> ptrs){
    int len = ptrs.size();
    if(!len)return std::make_shared<NilValue>();
    auto ret = std::make_shared<PairValue>(ptrs[len-1], std::make_shared<NilValue>());
    for(int i = len - 2; i >= 0; i--){
        ret = std::make_shared<PairValue>(ptrs[i], ret);
    }
    return ret;
}