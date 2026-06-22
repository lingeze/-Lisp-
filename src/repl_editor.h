#ifndef REPL_EDITOR_H
#define REPL_EDITOR_H

#include <memory>
#include <string>
#include <unordered_set>
#include <replxx.hxx>
#include "./repl_analysis.h"

class EvalEnv;

class ReplEditor {
private:
    replxx::Replxx& rx;
    std::shared_ptr<EvalEnv> env;
    std::string historyPath;
    std::unordered_set<std::string> builtinNames;

    int getLastLen(const std::string& input);
    replxx::Replxx::ACTION_RESULT handleEnter(char32_t key);

    void installHistory();
    void installCompletion();
    void installMultilineInput();
    void installParenJump();
    void installHighlighter();
    void installExpressionJump();
    int findParenNearCursor(const ReplAnalysis& analysis,int cursorPosition);
    replxx::Replxx::ACTION_RESULT handleParenJump();
    replxx::Replxx::ACTION_RESULT jumpToExpressionStart();
    replxx::Replxx::ACTION_RESULT jumpToExpressionEnd();
public:
    ReplEditor(replxx::Replxx& rx, std::shared_ptr<EvalEnv> env, const std::string& historyPath);
    void install();
    void addHistory(const std::string& input);
    void saveHistory();
};

#endif
