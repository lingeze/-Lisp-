#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <memory>
#include <vector>
#include <optional>
class Value;
using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);
class Value: public std::enable_shared_from_this<Value>{
public:
    virtual ~Value() = default;
    virtual std::string toString() = 0;
    bool isSelfEvaluating();
    bool isNil();
    bool isPair();
    bool isNumber();
    bool isString();
    bool isSymbol();
    bool isInteger();
    double asNumber();
    bool isBool();
    bool isProcedure();
    std::vector<ValuePtr> toVector();
    virtual std::optional<std::string> asSymbol(){
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
};
class NumericValue: public Value{
private:
    double value{0};
public:
    NumericValue(double v):value{v}{

    }
    std::string toString() override;
};
class StringValue: public Value{
private:
    std::string value;
public:
    StringValue(std::string s):value{s}{

    }
    std::string toString() override;
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
    ValuePtr call(std::vector<ValuePtr> args);
};
class LambdaValue : public Value {
private:
    std::vector<std::string> params{};
    std::vector<ValuePtr> body{};
public:
    LambdaValue(std::vector<std::string> params, std::vector<ValuePtr> body):params(params), body(body){

    }
    LambdaValue(){

    }
    std::string toString() override; // 如前所述，返回 #<procedure> 即可
};
ValuePtr ToList(const std::vector<ValuePtr>& ptrs);
#endif
