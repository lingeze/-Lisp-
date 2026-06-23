#include "./repl_analysis.h"
#include <cctype>
#include <stdexcept>

static bool isDelimiter(char c) {
    return std::isspace(static_cast<unsigned char>(c)) ||
           c == '(' || c == ')' || c == '"' || c == ';' ||
           c == '\'' || c == '`' || c == ',';
}

static bool isNumber(const std::string& text) {
    if (text.empty() || text == "+" || text == "-" || text == ".") return false;
    try {
        std::size_t parsedLength = 0;
        std::stod(text, &parsedLength);
        return parsedLength == text.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

ReplAnalysis::ReplAnalysis(const std::string& input) {
    incomplete = false;
    invalid = false;
    matchingParens.assign(input.size(), -1);
    parenPositions.assign(input.size(), false);
    errorPositions.assign(input.size(), false);
    highlightTypes.assign(input.size(), HighlightType::NORMAL);
    analyze(input);
}

void ReplAnalysis::markHighlight(int begin, int end, HighlightType type) {
    for (int i = begin; i < end && i < (int)highlightTypes.size(); i++) {
        highlightTypes[i] = type;
    }
}

void ReplAnalysis::analyze(const std::string& input) {
    std::vector<int> stack;
    int i = 0;

    while (i < (int)input.size()) {
        char c = input[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        if (c == ';') {
            int begin = i;
            while (i < (int)input.size() && input[i] != '\n') i++;
            markHighlight(begin, i, HighlightType::COMMENT);
            continue;
        }

        if (c == '"') {
            int begin = i;
            bool escaped = false;
            bool closed = false;
            i++;
            while (i < (int)input.size()) {
                if (escaped) {
                    escaped = false;
                    i++;
                    continue;
                }
                if (input[i] == '\\') {
                    escaped = true;
                    i++;
                    continue;
                }
                if (input[i] == '"') {
                    i++;
                    closed = true;
                    break;
                }
                i++;
            }
            markHighlight(begin, i, HighlightType::STRING);
            if (!closed) incomplete = true;
            continue;
        }

        if (c == '(') {
            parenPositions[i] = true;
            stack.push_back(i);
            i++;
            continue;
        }

        if (c == ')') {
            parenPositions[i] = true;
            if (stack.empty()) {
                invalid = true;
                errorPositions[i] = true;
                markHighlight(i, i + 1, HighlightType::ERROR_MESSAGE);
            } else {
                int left = stack.back();
                stack.pop_back();
                matchingParens[left] = i;
                matchingParens[i] = left;
            }
            i++;
            continue;
        }

        if (c == '\'' || c == '`' || c == ',') {
            int begin = i;
            i++;
            markHighlight(begin, i, HighlightType::QUOTE);
            continue;
        }

        int begin = i;
        while (i < (int)input.size() && !isDelimiter(input[i])) i++;
        std::string text = input.substr(begin, i - begin);

        if (isNumber(text)) {
            markHighlight(begin, i, HighlightType::NUMBER);
        } else if (text == "#t" || text == "#f") {
            markHighlight(begin, i, HighlightType::BOOLEAN);
        } else if (text == ".") {
            markHighlight(begin, i, HighlightType::QUOTE);
        } else {
            identifiers.emplace_back(begin, i, text);
        }
    }
    if (!stack.empty()) incomplete = true;
}

bool ReplAnalysis::needsMoreInput() const {
    return incomplete && !invalid;
}

bool ReplAnalysis::hasError() const {
    return invalid;
}

int ReplAnalysis::getMatchingParen(int position) const {
    if (position < 0 || position >= (int)matchingParens.size()) {
        return -1;
    }
    return matchingParens[position];
}

bool ReplAnalysis::isParen(int position) const {
    if (position < 0 || position >= (int)parenPositions.size()) {
        return false;
    }
    return parenPositions[position];
}

bool ReplAnalysis::isError(int position) const {
    if (position < 0 || position >= (int)errorPositions.size()) {
        return false;
    }
    return errorPositions[position];
}

HighlightType ReplAnalysis::getHighlightType(int position) const {
    if (position < 0 || position >= (int)highlightTypes.size()) {
        return HighlightType::NORMAL;
    }
    return highlightTypes[position];
}

const std::vector<IdentifierRange>& ReplAnalysis::getIdentifiers() const {
    return identifiers;
}
int ReplAnalysis::findLeftParen(int cursorPosition) const {
    int size = (int)matchingParens.size();
    if (cursorPosition < 0) cursorPosition = 0;
    if (cursorPosition > size) cursorPosition = size;
    for (int i = cursorPosition - 1; i >= 0; i--) {
        if (!isParen(i)) continue;
        int right = matchingParens[i];
        if (right > i && right + 1 >= cursorPosition) {
            return i;
        }
    }
    return -1;
}
int ReplAnalysis::findRightParen(int cursorPosition) const {
    int size = (int)matchingParens.size();
    if (cursorPosition < 0) cursorPosition = 0;
    for (int i = cursorPosition; i < size; i++) {
        if (!isParen(i)) continue;
        if (matchingParens[i] < i) {
            return i;
        }
    }
    return -1;
}
