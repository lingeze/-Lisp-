#include "./repl_editor.h"
#include "./eval_env.h"
#include "./forms.h"
#include <iostream>
static replxx::Replxx::Color getHighlightColor(HighlightType type) {
    using Color = replxx::Replxx::Color;
    switch (type) {
        case HighlightType::QUOTE:          return Color::BRIGHTMAGENTA;
        case HighlightType::STRING:         return Color::BRIGHTGREEN;
        case HighlightType::COMMENT:        return Color::GRAY;
        case HighlightType::NUMBER:         return Color::YELLOW;
        case HighlightType::BOOLEAN:        return Color::BRIGHTMAGENTA;
        case HighlightType::SPECIAL_FORM:   return replxx::color::bold(Color::BRIGHTMAGENTA);
        case HighlightType::BUILTIN:        return replxx::color::bold(Color::BRIGHTBLUE);
        case HighlightType::USER_PROCEDURE: return replxx::color::bold(Color::BRIGHTBLUE);
        case HighlightType::ERROR_MESSAGE:  return replxx::color::underline(Color::BRIGHTRED);
        default:                            return Color::DEFAULT;
    }
}

static void paintRange(replxx::Replxx::colors_t& colors, int begin, int end,
                       replxx::Replxx::Color color) {
    for (int i = begin; i < end && i < (int)colors.size(); i++)
        colors[i] = color;
}

ReplEditor::ReplEditor(replxx::Replxx& rx, std::shared_ptr<EvalEnv> env, const std::string& historyPath)
    : rx(rx), env(env), historyPath(historyPath) {
    const auto& table = this->env->getSymbolTable();
    for (const auto& [name, value] : table) {
        if (value->isProcedure()) builtinNames.insert(name);
    }
}

void ReplEditor::install() {
    installHistory();
    installCompletion();
    installMultilineInput();
    installParenJump();
    installHighlighter();
    installExpressionJump();
}


void ReplEditor::installHistory() {
    rx.history_load(historyPath);
}

void ReplEditor::addHistory(const std::string& input) {
    rx.history_add(input);
}

void ReplEditor::saveHistory() {
    rx.history_save(historyPath);
}


int ReplEditor::getLastLen(const std::string& input) {
    int length = 0;
    for (int i = (int)input.length() - 1; i >= 0; i--) {
        char c = input[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '(' || c == ')') break;
        length++;
    }
    return length;
}

void ReplEditor::installCompletion() {
    rx.bind_key_internal(replxx::Replxx::KEY::TAB, "complete_next");
    rx.set_completion_callback([this](const std::string& input, int& contextLen) {
        contextLen = getLastLen(input);
        std::string word = input.substr(input.size() - contextLen, contextLen);
        replxx::Replxx::completions_t cands;
        const auto& table = env->getSymbolTable();
        for (const auto& [name, _] : table) {
            if (name.starts_with(word))
                cands.emplace_back(name);
        }
        return cands;
    });
}

replxx::Replxx::ACTION_RESULT ReplEditor::handleEnter(char32_t key) {
    replxx::Replxx::State state = rx.get_state();
    std::string input = state.text();
    ReplAnalysis analysis(input);
    if (analysis.needsMoreInput())
        return rx.invoke(replxx::Replxx::ACTION::NEW_LINE, key);
    return rx.invoke(replxx::Replxx::ACTION::COMMIT_LINE, key);
}

void ReplEditor::installMultilineInput() {
    rx.bind_key(replxx::Replxx::KEY::ENTER, [this](char32_t key) {
        return handleEnter(key);
    });
}
int ReplEditor::findParenNearCursor(const ReplAnalysis& analysis,int cursorPosition) {
    if (analysis.isParen(cursorPosition)) {
        return cursorPosition;
    }
    return -1;
}
replxx::Replxx::ACTION_RESULT ReplEditor::handleParenJump() {
    replxx::Replxx::State state = rx.get_state();
    std::string input = state.text();
    int cursorPosition = state.cursor_position();
    ReplAnalysis analysis(input);
    int currentParen = findParenNearCursor(analysis, cursorPosition);
    if (currentParen == -1) {
        return replxx::Replxx::ACTION_RESULT::CONTINUE;
    }
    int matchingParen = analysis.getMatchingParen(currentParen);
    if (matchingParen == -1) {
        return replxx::Replxx::ACTION_RESULT::CONTINUE;
    }
    int newCursorPosition = matchingParen;
    rx.set_state(replxx::Replxx::State(input.c_str(), newCursorPosition));
    return replxx::Replxx::ACTION_RESULT::CONTINUE;
}
void ReplEditor::installParenJump() {
    rx.bind_key(replxx::Replxx::KEY::meta('m'), [this](char32_t) {return handleParenJump();});
}
void ReplEditor::installHighlighter() {
    rx.set_highlighter_callback(
        [this](const std::string& input, replxx::Replxx::colors_t& colors) {   
            ReplAnalysis analysis(input);
            for (int i = 0; i < (int)colors.size(); i++) {
                colors[i] = getHighlightColor(analysis.getHighlightType(i));
            }
            const auto& table = env->getSymbolTable();
            for (const IdentifierRange& id : analysis.getIdentifiers()) {
                const std::string& name = id.getName();
                HighlightType type = HighlightType::NORMAL;
                if (SPECIAL_FORMS.contains(name) || name == "else") {
                    type = HighlightType::SPECIAL_FORM;
                } else if (builtinNames.contains(name)) {
                    type = HighlightType::BUILTIN;
                } else if (table.contains(name) && table.at(name)->isProcedure()) {
                    type = HighlightType::USER_PROCEDURE;
                }
                paintRange(colors, id.getBegin(), id.getEnd(), getHighlightColor(type));
            }
        }
    );
}
replxx::Replxx::ACTION_RESULT ReplEditor::jumpToExpressionStart() {
    replxx::Replxx::State state = rx.get_state();
    std::string input = state.text();
    int cursorPosition = state.cursor_position();
    ReplAnalysis analysis(input);
    int target = analysis.findLeftParen(cursorPosition);
    if (target == -1) {
        return replxx::Replxx::ACTION_RESULT::CONTINUE;
    }
    rx.set_state(replxx::Replxx::State(input.c_str(), target));
    return replxx::Replxx::ACTION_RESULT::CONTINUE;
}
replxx::Replxx::ACTION_RESULT ReplEditor::jumpToExpressionEnd() {
    replxx::Replxx::State state = rx.get_state();
    std::string input = state.text();
    int cursorPosition = state.cursor_position();
    ReplAnalysis analysis(input);
    int target = analysis.findRightParen(cursorPosition);
    if (target == -1) {
        return replxx::Replxx::ACTION_RESULT::CONTINUE;
    }
    rx.set_state(replxx::Replxx::State(input.c_str(), target + 1));
    return replxx::Replxx::ACTION_RESULT::CONTINUE;
}
void ReplEditor::installExpressionJump() {
    rx.bind_key(
        replxx::Replxx::KEY::meta('a'),
        [this](char32_t) {
            return jumpToExpressionStart();
        }
    );
    rx.bind_key(
        replxx::Replxx::KEY::meta('d'),
        [this](char32_t) {
            return jumpToExpressionEnd();
        }
    );
}
