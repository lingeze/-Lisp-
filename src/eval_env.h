#ifndef EVALENV_H
#define EVALENV_H
#include "./value.h"
#include <unordered_map>
class EvalEnv: public std::enable_shared_from_this<EvalEnv>{
private:
    std::unordered_map<std::string, ValuePtr> symbolTable{};
    std::shared_ptr<EvalEnv> parent{nullptr};
    EvalEnv();
public:
    static std::shared_ptr<EvalEnv> createGlobal();
    void set_parent(const std::shared_ptr<EvalEnv>& parentEnv);
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    void addVariable(const std::string& name, ValuePtr value);
    void delVariable(const std::string& name);
    ValuePtr lookupBinding(ValuePtr expr);
    std::unordered_map<std::string, ValuePtr> saveSymbolTable() const;
    void restoreSymbolTable(const std::unordered_map<std::string, ValuePtr>& tbl);
    std::shared_ptr<EvalEnv> createChild
    (const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
};
#endif