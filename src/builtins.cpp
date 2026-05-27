#include "./builtins.h"
#include <cstdlib>
#include <iostream>
#include "builtins.h"
#include <cmath>
ValuePtr add(const std::vector<ValuePtr>& params) {
    auto result = 0.0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr minus(const std::vector<ValuePtr>& params) {
    if(params.size() == 0){
        throw LispError("no arguments");
    }
    if(params.size() == 1){
        if(!params[0]->isNumber())throw LispError("argument is a non-numeric value.");
        else return std::make_shared<NumericValue>(-params[0]->asNumber());
    }
    if(params.size() == 2){
        if(!params[0]->isNumber() || !params[1]->isNumber())
            throw LispError("argument is a non-numeric value.");
        else return std::make_shared<NumericValue>(params[0]->asNumber() - params[1]->asNumber() );
    }
    if(params.size() > 2){
        throw LispError("too much arguments");
    }
}
ValuePtr multiply(const std::vector<ValuePtr>& params) {
    auto result = 1.0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("cannot multiply a non-numeric value.");
        }
        result *= i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr divide(const std::vector<ValuePtr>& params) {
    if(params.size() == 0){
        throw LispError("no arguments");
    }
    if(params.size() == 1){
        if(!params[0]->isNumber())throw LispError("argument is a non-numeric value.");
        else {
            double num = params[0]->asNumber();
            if(num == 0)throw LispError("cannot divide 0");
            return std::make_shared<NumericValue>(1.0/params[0]->asNumber());
        }
    }
    if(params.size() == 2){
        if(!params[0]->isNumber() || !params[1]->isNumber())
            throw LispError("argument is a non-numeric value.");
        else {
            double num = params[1]->asNumber();
            if(num == 0)throw LispError("cannot divide 0");
            return std::make_shared<NumericValue>(params[0]->asNumber() / params[1]->asNumber() );
        }
    }
    if(params.size() > 2){
        throw LispError("too much arguments");
    }
}
ValuePtr _abs(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber()){
        throw(LispError("argument is a non-numeric value."));
    }
    return std::make_shared<NumericValue>(abs(params[0]->asNumber()));
}
ValuePtr expt(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    if(params[0]->asNumber() == 0 && params[1]->asNumber() == 0)
        throw LispError("input is invalid");
    return std::make_shared<NumericValue>(std::pow(params[0]->asNumber(), params[1]->asNumber()));
}

ValuePtr quotient(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    if(params[1]->asNumber() == 0)
        throw LispError("cannot divide 0");
    return std::make_shared<NumericValue>(std::trunc(params[0]->asNumber()/params[1]->asNumber()));
}
ValuePtr equal(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() == params[1]->asNumber());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr greater(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() > params[1]->asNumber());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr less(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() < params[1]->asNumber());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr greaterEqual(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() >= params[1]->asNumber());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr lessEqual(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() <= params[1]->asNumber());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr evenq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isInteger())
        throw LispError("argument is not an integer.");
    bool valid = ((static_cast<int>(params[0]->asNumber()) % 2) == 0);
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr oddq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isInteger())
        throw LispError("argument is not an integer.");
    bool valid = ((static_cast<int>(params[0]->asNumber()) % 2) != 0);
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}

ValuePtr zeroq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber())
        throw LispError("argument is a non-numeric value.");
    bool valid = (params[0]->asNumber() == 0);
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr remainder(const std::vector<ValuePtr>& params){
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isNumber() || !params[1]->isNumber())
        throw LispError("argument is a non-numeric value.");
    double x = params[0]->asNumber(), y = params[1]->asNumber();
    return std::make_shared<NumericValue>(x - std::trunc(x/y) * y);
}
ValuePtr my_exit(const std::vector<ValuePtr>& params) {
    int code = 0;
    if (params.size() > 1) {
        throw LispError("expected 0 or 1 argument, got " + std::to_string(params.size()));
    }
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("argument must be a number");
        }
        code = static_cast<int>(params[0]->asNumber());
    }
    std::exit(code);
}
ValuePtr print(const std::vector<ValuePtr>& params) {
    if(params.size() > 1){
        throw(LispError("too much params"));
    }
    for(auto p:params){
        std::cout << p->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}
ValuePtr display(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(params[0]->isString())std::cout << params[0]->toString() << std::endl;
    else std::cout << "'" << params[0]->toString() << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr newline(const std::vector<ValuePtr>& params){
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr atomq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = (params[0]->isNil() || params[0]->isNumber() || params[0]->isString()||
        params[0]->isSymbol()||params[0]->isBool());
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr booleanq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isBool();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr listq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isNil() || params[0]->isPair();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr numberq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isNumber();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr nullq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isNil();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr pairq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isPair();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr procedureq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isProcedure();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr stringq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isString();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr symbolq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isSymbol();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr integerq(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    bool valid = params[0]->isInteger();
    return (valid) ? std::make_shared<BooleanValue>(1) : std::make_shared<BooleanValue>(0);
}
ValuePtr car(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isPair()){
        throw(LispError("argument is not a pair"));
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
    return pair->car();
}
ValuePtr cdr(const std::vector<ValuePtr>& params){
    if(params.size() != 1){
        throw(LispError("expected 1 arguments, got " + std::to_string(params.size())));
    }
    if(!params[0]->isPair()){
        throw(LispError("argument is not a pair"));
    }
    auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
    return pair->cdr();
}
ValuePtr cons(const std::vector<ValuePtr>& params) {
    if(params.size() != 2){
        throw(LispError("expected 2 arguments, got " + std::to_string(params.size())));
    }
    return std::make_shared<PairValue>(params[0], params[1]);
}
ValuePtr length(const std::vector<ValuePtr>& params) {
    return std::make_shared<NumericValue>((int)params.size());
}
ValuePtr list(const std::vector<ValuePtr>& params) {
    return ToList(params);
}

static const BuiltinMap builtins = {                                                         
    {"exit", &my_exit},
    {"+", &add},
    {"-", &minus},
    {"*", &multiply},
    {"/", &divide},
    {"abs", &_abs}, 
    {"expt", &expt},
    {"quotient", &quotient},
    {"=", &equal},
    {">", &greater},
    {"<", &less},
    {">=", &greaterEqual},
    {"<=", &lessEqual},
    {"even?", &evenq},
    {"odd?", &oddq},
    {"zero?", &zeroq},                                                                            
    {"print", &print},                                                                       
    {"display", &display},
    {"newline", &newline},
    {"atom?", &atomq},
    {"boolean?", &booleanq},
    {"number?", &numberq},
    {"list?", &listq},
    {"string?", &stringq},
    {"symbol?", &symbolq},
    {"procedure?", &procedureq},
    {"pair?", &pairq},
    {"null?", &nullq},
    {"integer?",&integerq},
    {"car", &car},
    {"cdr", &cdr},
    {"cons", &cons},
    {"length", &length},
    {"list", &list},
    {"remainder", &remainder}
};                                                                                                       
const BuiltinMap& getBuiltins() {                                                            
    return builtins;
}