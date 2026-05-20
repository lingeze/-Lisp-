#ifndef BUILTINS_H
#define BUILTINS_H
#include "./value.h"
#include "./error.h"
#include <vector>
ValuePtr add(const std::vector<ValuePtr>& params) {
    auto result = 0.0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
#endif