#ifndef BUILTIN_FUNC_ARGS_H
#define BUILTIN_FUNC_ARGS_H

#include "./value.h"
#include <vector>
#include <memory>
#include <string>

class EvalEnv;

class FuncArgs {
public:
    FuncArgs(const std::vector<ValuePtr>& args, std::shared_ptr<EvalEnv> env);

    size_t argCount() const;
    ValuePtr arg(size_t index) const;
    ValuePtr operator[](size_t index) const;

    void requireArgCount(size_t n) const;
    void requireArgCount(size_t min, size_t max) const;

    const std::vector<ValuePtr>& allArgs() const;
    EvalEnv& env() const;
    std::shared_ptr<EvalEnv> envPtr() const;

private:
    const std::vector<ValuePtr>& args_;
    std::shared_ptr<EvalEnv> env_;
};

#endif
