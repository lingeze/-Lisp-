#ifndef EVALENV_H
#define EVALENV_H
#include "./value.h"
#include <unordered_map>
class EvalEnv{
private:
    std::unordered_map<std::string, ValuePtr> symbolTable;
public:
    ValuePtr eval(ValuePtr expr);
    
};
#endif