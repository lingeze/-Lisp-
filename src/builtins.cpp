#include "./builtins.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "./builtin_func_args.h"
#include "./eval_env.h"
#include "builtins.h"

ValuePtr add(FuncArgs& args) {
    double result = 0.0;
    for (size_t i = 0; i < args.argCount(); ++i) result += args[i]->asNumber();
    return std::make_shared<NumericValue>(result);
}

ValuePtr minus(FuncArgs& args) {
    if (args.argCount() == 0)
        throw LispError("-: expected at least 1 argument");
    if (args.argCount() == 1)
        return std::make_shared<NumericValue>(-args[0]->asNumber());
    if (args.argCount() == 2)
        return std::make_shared<NumericValue>(args[0]->asNumber() -
                                              args[1]->asNumber());
    throw LispError("-: expected at most 2 arguments, got " +
                    std::to_string(args.argCount()));
}

ValuePtr multiply(FuncArgs& args) {
    double result = 1.0;
    for (size_t i = 0; i < args.argCount(); ++i) result *= args[i]->asNumber();
    return std::make_shared<NumericValue>(result);
}

ValuePtr divide(FuncArgs& args) {
    if (args.argCount() == 0)
        throw LispError("/: expected at least 1 argument");
    if (args.argCount() == 1) {
        double num = args[0]->asNumber();
        if (num == 0) throw LispError("/: cannot divide by 0");
        return std::make_shared<NumericValue>(1.0 / num);
    }
    if (args.argCount() == 2) {
        double num = args[1]->asNumber();
        if (num == 0) throw LispError("/: cannot divide by 0");
        return std::make_shared<NumericValue>(args[0]->asNumber() / num);
    }
    throw LispError("/: expected at most 2 arguments, got " + std::to_string(args.argCount()));
}

ValuePtr _abs(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<NumericValue>(std::abs(args[0]->asNumber()));
}

ValuePtr expt(FuncArgs& args) {
    args.requireArgCount(2);
    if (args[0]->asNumber() == 0 && args[1]->asNumber() == 0)
        throw LispError("expt: 0^0 is undefined");
    return std::make_shared<NumericValue>(
        std::pow(args[0]->asNumber(), args[1]->asNumber()));
}

ValuePtr quotient(FuncArgs& args) {
    args.requireArgCount(2);
    if (args[1]->asNumber() == 0)
        throw LispError("quotient: cannot divide by 0");
    return std::make_shared<NumericValue>(
        std::trunc(args[0]->asNumber() / args[1]->asNumber()));
}

ValuePtr remainder(FuncArgs& args) {
    args.requireArgCount(2);
    double x = args[0]->asNumber(), y = args[1]->asNumber();
    if (y == 0) throw LispError("remainder: cannot divide by 0");
    return std::make_shared<NumericValue>(x - std::trunc(x / y) * y);
}

ValuePtr modulo(FuncArgs& args) {
    args.requireArgCount(2);
    double x = args[0]->asNumber(), y = args[1]->asNumber();
    if (y == 0) throw LispError("modulo: cannot divide by 0");
    double r = std::fmod(x, y);
    if (r * y < 0) r += y;
    return std::make_shared<NumericValue>(r);
}

ValuePtr eqq(FuncArgs& args) {
    args.requireArgCount(2);
    auto a = args[0], b = args[1];
    if (typeid(*a) != typeid(*b)) return std::make_shared<BooleanValue>(false);
    if (a->isBool() || a->isNumber() || a->isNil() || a->isSymbol())
        return std::make_shared<BooleanValue>(a->toString() == b->toString());
    return std::make_shared<BooleanValue>(a == b);
}

static bool equalHelper(ValuePtr a, ValuePtr b) {
    if (typeid(*a) != typeid(*b)) return false;
    if (a->isBool() || a->isNumber() || a->isNil() || a->isSymbol() || a->isString())
        return a->toString() == b->toString();
    if (a->isPair()) {
        auto pa = std::dynamic_pointer_cast<PairValue>(a);
        auto pb = std::dynamic_pointer_cast<PairValue>(b);
        return equalHelper(pa->car(), pb->car()) && equalHelper(pa->cdr(), pb->cdr());
    }
    return a == b;
}

ValuePtr equalq(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(equalHelper(args[0], args[1]));
}

ValuePtr _not(FuncArgs& args) {
    args.requireArgCount(1);
    auto v = args[0];
    bool isFalse = (v->isBool() && !v->asBool());
    return std::make_shared<BooleanValue>(isFalse);
}

ValuePtr equal(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(args[0]->asNumber() == args[1]->asNumber());
}

ValuePtr greater(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(args[0]->asNumber() > args[1]->asNumber());
}

ValuePtr less(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(args[0]->asNumber() < args[1]->asNumber());
}

ValuePtr greaterEqual(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(args[0]->asNumber() >= args[1]->asNumber());
}

ValuePtr lessEqual(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<BooleanValue>(args[0]->asNumber() <= args[1]->asNumber());
}

ValuePtr evenq(FuncArgs& args) {
    args.requireArgCount(1);
    if (!args[0]->isInteger())
        throw LispError("even?: argument must be an integer");
    return std::make_shared<BooleanValue>(std::fmod(args[0]->asNumber(), 2.0) == 0);
}

ValuePtr oddq(FuncArgs& args) {
    args.requireArgCount(1);
    if (!args[0]->isInteger())
        throw LispError("odd?: argument must be an integer");
    return std::make_shared<BooleanValue>(std::fmod(args[0]->asNumber(), 2.0) != 0);
}

ValuePtr zeroq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->asNumber() == 0);
}

ValuePtr my_exit(FuncArgs& args) {
    if (args.argCount() > 1)
        throw LispError("expected 0 or 1 argument, got " + std::to_string(args.argCount()));
    int code = 0;
    if (args.argCount() == 1) code = static_cast<int>(args[0]->asNumber());
    std::exit(code);
}

ValuePtr print(FuncArgs& args) {
    args.requireArgCount(1);
    std::cout << args[0]->toString() << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr display(FuncArgs& args) {
    args.requireArgCount(1);
    if (args[0]->isString())
        std::cout << args[0]->toString();
    else
        std::cout << "'" << args[0]->toString();
    return std::make_shared<NilValue>();
}

ValuePtr newline(FuncArgs& args) {
    args.requireArgCount(0);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr displayln(FuncArgs& args) {
    args.requireArgCount(1);
    display(args);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr error(FuncArgs& args) {
    if (args.argCount() > 1)
        throw LispError("error: expected at most 1 argument, got " + std::to_string(args.argCount()));
    if (args.argCount() == 0) throw LispError("error: no message");
    throw LispError(args[0]->toString());
}

ValuePtr atomq(FuncArgs& args) {
    args.requireArgCount(1);
    auto v = args[0];
    bool ok = v->isNil() || v->isNumber() || v->isString() || v->isSymbol() || v->isBool();
    return std::make_shared<BooleanValue>(ok);
}

ValuePtr booleanq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isBool());
}

ValuePtr listq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isList());
}

ValuePtr numberq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isNumber());
}

ValuePtr nullq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isNil());
}

ValuePtr pairq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isPair());
}

ValuePtr procedureq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isProcedure());
}

ValuePtr stringq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isString());
}

ValuePtr symbolq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isSymbol());
}

ValuePtr integerq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isInteger());
}

ValuePtr car(FuncArgs& args) {
    args.requireArgCount(1);
    return args[0]->asPair()->car();
}

ValuePtr cdr(FuncArgs& args) {
    args.requireArgCount(1);
    return args[0]->asPair()->cdr();
}

ValuePtr cons(FuncArgs& args) {
    args.requireArgCount(2);
    return std::make_shared<PairValue>(args[0], args[1]);
}

ValuePtr length(FuncArgs& args) {
    args.requireArgCount(1);
    if (!args[0]->isList()) throw LispError("length: expected a proper list");
    return std::make_shared<NumericValue>((int)args[0]->toVector().size());
}

ValuePtr list(FuncArgs& args) {
    return ToList(args.allArgs());
}

ValuePtr eval(FuncArgs& args) {
    args.requireArgCount(1);
    return args.env().eval(args[0]);
}

ValuePtr apply(FuncArgs& args) {
    args.requireArgCount(2);
    if (!args[1]->isList()) throw LispError("apply: expected a proper list");
    return args.env().apply(args[0], args[1]->toVector());
}

ValuePtr append(FuncArgs& args) {
    std::vector<ValuePtr> finalArg{};
    for (auto arg : args.allArgs()) {
        if (!arg->isList()) throw LispError("append: expected a proper list");
        auto _arg = arg->toVector();
        finalArg.insert(finalArg.end(), _arg.begin(), _arg.end());
    }
    return ToList(finalArg);
}

ValuePtr map(FuncArgs& args) {
    args.requireArgCount(2);
    if (!args[1]->isList()) throw LispError("map: expected a proper list");
    if (args[1]->isNil()) return std::make_shared<NilValue>();
    std::vector<ValuePtr> res{};
    auto v = args[1]->toVector();
    for (auto arg : v) {
        res.push_back(args.env().apply(args[0], {arg}));
    }
    return ToList(res);
}

ValuePtr filter(FuncArgs& args) {
    args.requireArgCount(2);
    if (!args[1]->isList()) throw LispError("filter: expected a proper list");
    if (args[1]->isNil()) return std::make_shared<NilValue>();
    std::vector<ValuePtr> res{};
    auto v = args[1]->toVector();
    for (auto arg : v) {
        auto val{args.env().apply(args[0], {arg})};
        if (!val->isBool() || val->asBool()) res.push_back(arg);
    }
    return ToList(res);
}

ValuePtr reduce(FuncArgs& args) {
    args.requireArgCount(2);
    if (!args[1]->isList()) throw LispError("reduce: expected a proper list");
    if (args[1]->isNil()) throw LispError("reduce: expected a non-empty list");
    auto v = args[1]->toVector();
    if (v.size() == 1) return v[0];
    ValuePtr result = v.back();
    for (int i = (int)v.size() - 2; i >= 0; i--) {
        result = args.env().apply(args[0], {v[i], result});
    }
    return result;
}

ValuePtr force(FuncArgs& args) {
    args.requireArgCount(1);
    if (!args[0]->isPromise()) {
        return args[0];
    }
    return args[0]->asPromise()->force();
}
ValuePtr promiseq(FuncArgs& args) {
    args.requireArgCount(1);
    return std::make_shared<BooleanValue>(args[0]->isPromise());
}
ValuePtr make_promise(FuncArgs& args) {
    args.requireArgCount(1);
    if (args[0]->isPromise()) {
        return args[0]->asPromise();
    }
    return std::make_shared<PromiseValue>(args[0]);
}
static const BuiltinMap builtins = {{"exit", &my_exit},
                                    {"+", &add},
                                    {"-", &minus},
                                    {"*", &multiply},
                                    {"/", &divide},
                                    {"abs", &_abs},
                                    {"expt", &expt},
                                    {"quotient", &quotient},
                                    {"modulo", &modulo},
                                    {"remainder", &remainder},
                                    {"=", &equal},
                                    {">", &greater},
                                    {"<", &less},
                                    {">=", &greaterEqual},
                                    {"<=", &lessEqual},
                                    {"eq?", &eqq},
                                    {"equal?", &equalq},
                                    {"not", &_not},
                                    {"even?", &evenq},
                                    {"odd?", &oddq},
                                    {"zero?", &zeroq},
                                    {"print", &print},
                                    {"display", &display},
                                    {"newline", &newline},
                                    {"displayln", &displayln},
                                    {"error", &error},
                                    {"atom?", &atomq},
                                    {"boolean?", &booleanq},
                                    {"number?", &numberq},
                                    {"list?", &listq},
                                    {"string?", &stringq},
                                    {"symbol?", &symbolq},
                                    {"procedure?", &procedureq},
                                    {"pair?", &pairq},
                                    {"null?", &nullq},
                                    {"integer?", &integerq},
                                    {"car", &car},
                                    {"cdr", &cdr},
                                    {"cons", &cons},
                                    {"length", &length},
                                    {"list", &list},
                                    {"eval", &eval},
                                    {"apply", &apply},
                                    {"append", &append},
                                    {"map", &map},
                                    {"filter", &filter},
                                    {"reduce", &reduce},
                                    {"force", &force},
                                    {"promise?", &promiseq},
                                    {"make-promise", &make_promise}};
const BuiltinMap& getBuiltins() {
    return builtins;
}
