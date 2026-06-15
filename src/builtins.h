#ifndef BUILTINS_H
#define BUILTINS_H
#include "./value.h"
#include "./error.h"
#include "./builtin_func_args.h"
#include <vector>
#include <unordered_map>
using BuiltinMap = std::unordered_map<std::string, BuiltinFuncType*>;
ValuePtr add(FuncArgs& args);
ValuePtr print(FuncArgs& args);
ValuePtr my_exit(FuncArgs& args);
ValuePtr display(FuncArgs& args);
ValuePtr newline(FuncArgs& args);
ValuePtr displayln(FuncArgs& args);
ValuePtr error(FuncArgs& args);
ValuePtr atomq(FuncArgs& args);
ValuePtr booleanq(FuncArgs& args);
ValuePtr integerq(FuncArgs& args);
ValuePtr listq(FuncArgs& args);
ValuePtr numberq(FuncArgs& args);
ValuePtr nullq(FuncArgs& args);
ValuePtr pairq(FuncArgs& args);
ValuePtr procedureq(FuncArgs& args);
ValuePtr stringq(FuncArgs& args);
ValuePtr symbolq(FuncArgs& args);
ValuePtr car(FuncArgs& args);
ValuePtr cdr(FuncArgs& args);
ValuePtr cons(FuncArgs& args);
ValuePtr length(FuncArgs& args);
ValuePtr list(FuncArgs& args);
ValuePtr minus(FuncArgs& args);
ValuePtr multiply(FuncArgs& args);
ValuePtr divide(FuncArgs& args);
ValuePtr _abs(FuncArgs& args);
ValuePtr expt(FuncArgs& args);
ValuePtr quotient(FuncArgs& args);
ValuePtr remainder(FuncArgs& args);
ValuePtr modulo(FuncArgs& args);
ValuePtr append(FuncArgs& args);
ValuePtr equal(FuncArgs& args);
ValuePtr eqq(FuncArgs& args);
ValuePtr equalq(FuncArgs& args);
ValuePtr _not(FuncArgs& args);
ValuePtr less(FuncArgs& args);
ValuePtr greater(FuncArgs& args);
ValuePtr greaterEqual(FuncArgs& args);
ValuePtr lessEqual(FuncArgs& args);
ValuePtr evenq(FuncArgs& args);
ValuePtr oddq(FuncArgs& args);
ValuePtr zeroq(FuncArgs& args);
ValuePtr eval(FuncArgs& args);
ValuePtr apply(FuncArgs& args);
ValuePtr map(FuncArgs& args);
ValuePtr filter(FuncArgs& args);
ValuePtr reduce(FuncArgs& args);
const BuiltinMap& getBuiltins();
#endif
