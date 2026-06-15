#include "./builtin_func_args.h"
#include "./eval_env.h"
#include "./error.h"
#include <string>

FuncArgs::FuncArgs(const std::vector<ValuePtr>& args, std::shared_ptr<EvalEnv> env)
    : args_(args), env_(env) {}

size_t FuncArgs::argCount() const {
    return args_.size();
}

static std::string plural(size_t n) { return n == 1 ? " argument" : " arguments"; }

ValuePtr FuncArgs::arg(size_t index) const {
    if (index >= args_.size())
        throw LispError("expected at least " + std::to_string(index + 1)
                        + plural(index + 1) + ", got " + std::to_string(args_.size()));
    return args_[index];
}

ValuePtr FuncArgs::operator[](size_t index) const {
    return arg(index);
}

void FuncArgs::requireArgCount(size_t n) const {
    if (args_.size() != n)
        throw LispError("expected " + std::to_string(n)
                        + plural(n) + ", got " + std::to_string(args_.size()));
}

void FuncArgs::requireArgCount(size_t min, size_t max) const {
    if (args_.size() < min || args_.size() > max)
        throw LispError("expected " + std::to_string(min) + " to " + std::to_string(max)
                        + " arguments, got " + std::to_string(args_.size()));
}

const std::vector<ValuePtr>& FuncArgs::allArgs() const {
    return args_;
}

EvalEnv& FuncArgs::env() const {
    return *env_;
}

std::shared_ptr<EvalEnv> FuncArgs::envPtr() const {
    return env_;
}
