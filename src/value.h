#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <memory>
#include <vector>
#include <optional>
class EvalEnv;
class PairValue;
class PromiseValue;
class Value;
using ValuePtr = std::shared_ptr<Value>;
class FuncArgs;
using BuiltinFuncType = ValuePtr(FuncArgs&);
class Value: public std::enable_shared_from_this<Value>{
public:
    virtual ~Value() = default;
    virtual std::string toString() = 0;
    bool isSelfEvaluating();
    bool isNil();
    bool isPair();
    bool isList();
    bool isNumber();
    bool isString();
    bool isSymbol();
    bool isInteger();
    bool isPromise();
    virtual double asNumber();
    std::shared_ptr<PairValue> asPair();
    std::shared_ptr<PromiseValue> asPromise();
    bool isBool();
    virtual bool asBool();
    bool isProcedure();
    std::vector<ValuePtr> toVector();
    virtual std::optional<std::string> asSymbol(){
        return std::nullopt;
    }
    virtual std::optional<std::string> asString(){
        return std::nullopt;
    }
};
class BooleanValue: public Value{
private:
    bool value{0};
public:
    BooleanValue(bool v):value{v}{

    }
    std::string toString() override;
    bool asBool() override { return value; }
};
class NumericValue: public Value{
private:
    double value{0};
public:
    NumericValue(double v):value{v}{

    }
    std::string toString() override;
    double asNumber() override { return value; }
};
class StringValue: public Value{
private:
    std::string value;
public:
    StringValue(std::string s):value{s}{

    }
    std::string toString() override;
    std::optional<std::string> asString() override{
        return value;
    }
};
class NilValue: public Value{
public:
    std::string toString() override;
};
class SymbolValue: public Value{
private:
    std::string value;
public:
    SymbolValue(std::string s):value{s}{

    }
    std::string toString() override;
    std::optional<std::string> asSymbol()override{
        return toString();
    }
};
class PairValue: public Value{
private:
    ValuePtr lptr, rptr;
public:
    PairValue(ValuePtr l, ValuePtr r):lptr{l}, rptr{r}{

    }
    ValuePtr car()const{return lptr;}
    ValuePtr cdr()const{return rptr;}
    std::string toString() override;
};
class BuiltinProcValue:public Value{ 
private:
    BuiltinFuncType* value{};

public:
    BuiltinProcValue(BuiltinFuncType* v):value{v}{

    }
    std::string toString() override;
    ValuePtr call(FuncArgs& args);
};
class LambdaValue : public Value {
private:
    std::vector<std::string> params{};
    std::vector<ValuePtr> body{};
    std::shared_ptr<EvalEnv> parent{nullptr};
public:
    LambdaValue(std::vector<std::string> params, std::vector<ValuePtr> body, std::shared_ptr<EvalEnv> env):
        params(params), body(body), parent{env}{

    }
    ValuePtr apply(const std::vector<ValuePtr>& args);
    std::string toString() override;
};
class PromiseValue : public Value {
private:
    ValuePtr value;
    std::shared_ptr<EvalEnv> env;
    bool evaluated{false};
    bool forceResult{false};
public:
    PromiseValue(ValuePtr value, std::shared_ptr<EvalEnv> env):value{value}, env{env}{
        
    }
    PromiseValue(ValuePtr value, std::shared_ptr<EvalEnv> env, bool forceResult):
        value{value}, env{env}, forceResult{forceResult}{
        
    }
    PromiseValue(ValuePtr value):value{value}, evaluated{true}{

    }
    ValuePtr force();
    std::string toString()override;
};
ValuePtr ToList(const std::vector<ValuePtr>& ptrs);
#endif
