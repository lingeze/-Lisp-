#include "forms.h"
#include "./error.h"
#include <iostream>

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if (args.size() < 2){
        throw LispError("define: expected at least 2 arguments, got " + std::to_string(args.size()));
    }
    if (auto name = args[0]->asSymbol()) {
        env.addVariable(*name, env.eval(args[1]));
        return std::make_shared<NilValue>();
    }
    else if(args[0]->isPair()) {
        auto pair = args[0]->asPair();
        if(auto name = pair->car()->asSymbol()) {
            std::vector<ValuePtr> lambdaArgs;
            lambdaArgs.push_back(pair->cdr());
            lambdaArgs.insert(
                lambdaArgs.end(),
                args.begin() + 1,
                args.end()
            );
            env.addVariable(*name,lambdaForm(lambdaArgs, env));
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("define: first argument must be a symbol");
        }
    }
    else {
        throw LispError("define: first argument must be a symbol");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quote: expected 1 argument, got " + std::to_string(args.size()));
    }
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2 && args.size() != 3) {
        throw LispError("if: expected 2 or 3 arguments, got " + std::to_string(args.size()));
    }
    ValuePtr val = env.eval(args[0]);
    if (val->isBool() && !val->asBool()) {
        return args.size() == 2 ? std::make_shared<NilValue>() : env.eval(args[2]);
    }
    return env.eval(args[1]);
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(true);
    }
    for(auto arg:args){
        ValuePtr it = env.eval(arg);
        if (it->isBool() && !it->asBool()) return it;
    }
    int s = args.size();
    return env.eval(args[s-1]);
}

ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(false);
    }
    for(auto arg:args){
        ValuePtr it = env.eval(arg);
        bool ok = true;
        if (it->isBool() && !it->asBool()) ok = false;
        if(ok) return it;
    }
    int s = args.size();
    return env.eval(args[s-1]);
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        throw LispError("begin: expected at least 1 argument");
    }
    int s = args.size();
    for (int i = 0; i < s - 1; i++)
        env.eval(args[i]);
    return env.eval(args[s-1]);
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("lambda: expected at least 2 arguments (params + body)");
    }
    auto paramsExpr = args[0];
    std::vector<std::string> params;
    if (!paramsExpr->isNil()) {
        for (auto& p : paramsExpr->toVector()) {
            if (auto sym = p->asSymbol()) {
                params.push_back(*sym);
            } else {
                throw LispError("lambda: parameter must be a symbol");
            }
        }
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    auto lambda = std::make_shared<LambdaValue>(params, body, env.shared_from_this());
    return lambda;
}

ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) throw LispError("cond: expected at least 1 clause");
    int s_args = args.size();
    for (int k = 0; k < s_args-1; k++){
        ValuePtr arg = args[k];
        auto v = arg->toVector();
        if (v.size() == 0){
            throw LispError("cond: expected at least 1 expression in clause");
        }
        ValuePtr eval_cond = env.eval(v[0]);
        if (eval_cond->isBool() && !eval_cond->asBool()) continue;
        ValuePtr res = eval_cond;
        int s = v.size();
        for (int i = 1; i <= s-1; i++){
            res = env.eval(v[i]);
        }
        return res;
    }
    ValuePtr lst = args[s_args - 1];
    auto v = lst->toVector();
    if (v.size() == 0){
       throw LispError("cond: expected at least 1 expression in clause");
    }
    if (auto name = v[0]->asSymbol()){
        if(*name == "else"){
            if(v.size() == 1) throw LispError("cond: else clause requires at least 1 expression");
            ValuePtr res{};
            int s = v.size();
            for (int i = 1; i <= s-1; i++){
                res = env.eval(v[i]);
            }
            return res;
       }
    }
    ValuePtr eval_cond = env.eval(v[0]);
    if (eval_cond->isBool() && !eval_cond->asBool())
        return std::make_shared<NilValue>();
    ValuePtr res = eval_cond;
    int s = v.size();
    for (int i = 1; i <= s-1; i++){
        res = env.eval(v[i]);
    }
    return res;
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size() < 2) throw LispError("let: expected at least 2 arguments");
    auto params = args[0]->toVector();
    std::vector<std::string> lambdaParams;
    std::vector<ValuePtr> lambdaArgs;
    for (ValuePtr it : params){
        if(!it->isPair()){
            throw LispError("let: each binding must be a pair");
        }
        auto param = it->toVector();
        if(param.size() != 2){
            throw LispError("let: each binding must have exactly 2 elements");
        }
        if(auto name = param[0]->asSymbol()){
            lambdaParams.push_back(*name);
            lambdaArgs.push_back(env.eval(param[1]));
        }
        else throw LispError("let: binding name must be a symbol");
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    auto lambda = std::make_shared<LambdaValue>(lambdaParams, body, env.shared_from_this());
    return lambda->apply(lambdaArgs);
}

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1)
        throw LispError("quasiquote: expected 1 argument");
    auto tmpl = args[0];
    if (tmpl->isNil()) return std::make_shared<NilValue>();
    if (!tmpl->isPair()) return tmpl;
    auto v = tmpl->toVector();
    std::vector<ValuePtr> res{};
    for (ValuePtr elem : v) {
        if (elem->isPair() && elem->asPair()->car()->isSymbol()) {
            auto sym = elem->asPair()->car()->asSymbol();
            if (*sym == "unquote") {
                auto q = elem->toVector();
                if (q.size() != 2)
                    throw LispError("unquote: expected exactly 1 argument");
                res.push_back(env.eval(q[1]));
                continue;
            }
        }
        res.push_back(elem);
    }
    return ToList(res);
}

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm},
    {"lambda", lambdaForm},
    {"begin", beginForm},
    {"cond", condForm},
    {"let", letForm},
    {"quasiquote", quasiquoteForm}
};
