#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <memory>
class Value{
public:
    virtual ~Value() = default;
    virtual std::string toString() = 0;
};
using ValuePtr = std::shared_ptr<Value>; 
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
};
class PairValue: public Value{
private:
    ValuePtr lptr, rptr;
public:
    PairValue(ValuePtr l, ValuePtr r):lptr{l}, rptr{r}{

    }
    std::string toString() override;
};
#endif
