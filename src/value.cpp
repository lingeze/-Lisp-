#include "./value.h"
#include <iomanip>
#include <sstream>
#include <cmath>
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