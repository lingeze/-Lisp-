#ifndef BUILTINS_H
#define BUILTINS_H
#include "./value.h"
#include "./error.h"
#include <vector>
#include <unordered_map>
using BuiltinMap = std::unordered_map<std::string, BuiltinFuncType*>;
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
ValuePtr my_exit(const std::vector<ValuePtr>& params);
ValuePtr display(const std::vector<ValuePtr>& params);
ValuePtr newline(const std::vector<ValuePtr>& params);
ValuePtr atomq(const std::vector<ValuePtr>& params);
ValuePtr booleanq(const std::vector<ValuePtr>& params);
ValuePtr integerq(const std::vector<ValuePtr>& params);
ValuePtr listq(const std::vector<ValuePtr>& params);
ValuePtr numberq(const std::vector<ValuePtr>& params);
ValuePtr nullq(const std::vector<ValuePtr>& params);
ValuePtr pairq(const std::vector<ValuePtr>& params);
ValuePtr procedureq(const std::vector<ValuePtr>& params);
ValuePtr stringq(const std::vector<ValuePtr>& params);
ValuePtr symbolq(const std::vector<ValuePtr>& params);
ValuePtr car(const std::vector<ValuePtr>& params);
ValuePtr cdr(const std::vector<ValuePtr>& params);
ValuePtr cons(const std::vector<ValuePtr>& params);
ValuePtr length(const std::vector<ValuePtr>& params);
ValuePtr list(const std::vector<ValuePtr>& params);
ValuePtr append(const std::vector<ValuePtr>& params);
ValuePtr minus(const std::vector<ValuePtr>& params);
ValuePtr multiply(const std::vector<ValuePtr>& params);
ValuePtr divide(const std::vector<ValuePtr>& params);
ValuePtr _abs(const std::vector<ValuePtr>& params);
ValuePtr expt(const std::vector<ValuePtr>& params);
ValuePtr quotient(const std::vector<ValuePtr>& params);
ValuePtr remainder(const std::vector<ValuePtr>& params);
ValuePtr modulo(const std::vector<ValuePtr>& params);
ValuePtr equal(const std::vector<ValuePtr>& params);
ValuePtr less(const std::vector<ValuePtr>& params);
ValuePtr greater(const std::vector<ValuePtr>& params);
ValuePtr greaterEqual(const std::vector<ValuePtr>& params);
ValuePtr lessEqual(const std::vector<ValuePtr>& params);
ValuePtr evenq(const std::vector<ValuePtr>& params);
ValuePtr oddq(const std::vector<ValuePtr>& params);
ValuePtr zeroq(const std::vector<ValuePtr>& params);
const BuiltinMap& getBuiltins();
#endif