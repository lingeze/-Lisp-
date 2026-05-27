#include "forms.h"
#include "./error.h"
#include <iostream>
#include <ranges>
#include <algorithm>
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) {
        throw LispError("expected 2 arguments, got " + std::to_string(args.size()));
    }
    if (auto name = args[0]->asSymbol()) {
        env.addVariable(*name, env.eval(args[1]));
        return std::make_shared<NilValue>();
    } 
    else if(auto pair = static_pointer_cast<PairValue>(args[0])){
        if(auto name = pair->car()->asSymbol()){
            std::vector<ValuePtr> _params = pair->cdr()->toVector();
            for(auto param:_params){
                if(!param->isSymbol()){
                    throw LispError("define: not a symbol");
                }
                //std::cout << param->toString() << std::endl;
            }
            std::vector<std::string> params{};
            std::ranges::transform(_params, std::back_inserter(params), [](const ValuePtr &x){return x->toString();});
            std::vector<ValuePtr> body{args[1]->toVector()};
            /*for(auto p :body){
                std::cout << p->toString() << std::endl;
            }*/
            env.addVariable(*name, std::make_shared<LambdaValue>(params, body));
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("define: first argument must be a symbol");
        }
    }
    else throw LispError("define: first argument must be a symbol");
}
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("expected 1 arguments, got " + std::to_string(args.size()));
    }
    return args[0];
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2 && args.size() != 3) {
        throw LispError("expected 2 ot 3 arguments, got " + std::to_string(args.size()));
    }
    ValuePtr val = env.eval(args[0]);
    bool ok = 0;
    if(auto boolean = static_pointer_cast<BooleanValue>(val)){
        if(boolean->toString() == "#f"){
            if(args.size() == 2)return std::make_shared<NilValue>();
            ok = 0;
        }
        else ok = 1;
    }
    else {
        ok = 1;
    }
    return ok ? env.eval(args[1]) : env.eval(args[2]);
}
ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(1);
    }
    for(auto arg:args){
        ValuePtr it = env.eval(arg);
        if(auto boolean = static_pointer_cast<BooleanValue>(it)){
            if(boolean->toString() == "#f")return it;
        }
    }
    int s = args.size();
    return env.eval(args[s-1]);
}
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(0);
    }
    for(auto arg:args){
        ValuePtr it = env.eval(arg);
        bool ok = 1;
        if(auto boolean = static_pointer_cast<BooleanValue>(it)){
            if(boolean->toString() == "#f")ok = 0;
        }
        if(ok)return it;
    }
    int s = args.size();
    return env.eval(args[s-1]);
}
ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    return std::make_shared<LambdaValue>();
}
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm},
    {"lambda", lambdaForm}
};