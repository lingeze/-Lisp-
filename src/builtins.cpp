#include "./builtins.h"
#include <iostream>

ValuePtr add(const std::vector<ValuePtr>& params) {
    auto result = 0.0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr print(const std::vector<ValuePtr>& params) {
    if(params.size() > 1){
        throw(LispError("Print too much"));
    }
    for(auto p:params){
        std::cout << p->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}