#ifndef EVALENV_H
#define EVALENV_H
#include "./value.h"
#include <unordered_map>
class EvalEnv{
private:
    std::unordered_map<std::string, ValuePtr> symbolTable;
public:
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    void addVariable(const std::string& name, ValuePtr value);
};
#endif