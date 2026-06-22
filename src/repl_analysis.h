#ifndef REPL_ANALYSIS_H
#define REPL_ANALYSIS_H

#include <string>
#include <vector>
enum class HighlightType {
    NORMAL,
    QUOTE,
    STRING,
    COMMENT,
    NUMBER,
    BOOLEAN,
    SPECIAL_FORM,
    BUILTIN,
    USER_PROCEDURE,
    ERROR_MESSAGE
};
class IdentifierRange {
private:
    int begin;
    int end;
    std::string name;

public:
    IdentifierRange(int begin, int end, const std::string& name)
        : begin(begin), end(end), name(name) {
    }

    int getBegin() const {
        return begin;
    }

    int getEnd() const {
        return end;
    }

    const std::string& getName() const {
        return name;
    }
};

class ReplAnalysis {
private:
    std::vector<int> matchingParens;
    std::vector<bool> parenPositions;
    std::vector<bool> errorPositions;
    bool incomplete;
    bool invalid;
    void analyze(const std::string& input);
    std::vector<HighlightType> highlightTypes;
    std::vector<IdentifierRange> identifiers;
    void markHighlight(int begin, int end, HighlightType type);
public:
    ReplAnalysis(const std::string& input);
    bool needsMoreInput() const;
    bool hasError() const;
    int getMatchingParen(int position) const;
    bool isParen(int position) const;
    bool isError(int position) const;
    HighlightType getHighlightType(int position) const;
    const std::vector<IdentifierRange>& getIdentifiers() const;
    int findLeftParen(int cursorPosition) const;
    int findRightParen(int cursorPosition) const;
};

#endif
