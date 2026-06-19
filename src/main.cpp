#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <replxx.hxx>
#include <cstdlib>
#include "./tokenizer.h"
#include "./parser.h"
#include "rjsj_test.hpp"
#include "./eval_env.h"
#include "./error.h"

struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};

void printUsage() {
    std::cerr << "Usage: mini_lisp [file] [--repl]\n"
              << "  file       input .scm file\n"
              << "  --repl     force REPL mode\n";
}

static struct option longOpts[] = {
    {"output", required_argument, nullptr, 'o'},
    {"repl",   no_argument,       nullptr, 'r'},
    {"help",   no_argument,       nullptr, 'h'},
    {nullptr,  0,                 nullptr,  0 }
};

void runFile(const std::string& path, std::shared_ptr<EvalEnv> env) {
    std::ifstream file(path);
    if (!file.is_open())
        throw LispError("cannot open file: " + path);
    std::string content, line;
    while (std::getline(file, line))
        content += line + "\n";
    auto tokens = Tokenizer::tokenize(content);
    Parser parser(std::move(tokens));
    while (!parser.empty()) {
        env->eval(parser.parse());
    }
}
int countParens(const std::string &line){
    auto tokens = Tokenizer::tokenize(line);
    int num = 0;
    for(auto &token : tokens){
        if(token->getType() == TokenType::LEFT_PAREN) num ++;
        else if(token->getType() == TokenType::RIGHT_PAREN) num --;
    }
    return num;
}
std::string getHistoryPath() {
    const char* home = std::getenv("USERPROFILE");
    if (!home) home = std::getenv("HOME");   
    if (!home) return "mini_lisp_history.txt"; 
    return std::string(home) + "/.mini_lisp_history";
}
int getLastLen(const std::string& input){
    int ctxLen = 0;
    for (int i = (int)input.length() - 1; i >= 0; i--) {
        char c = input[i];
        if (c == ' ' || c == '(' || c == ')' || c == '\n') break;
        ctxLen++;
    }
    return ctxLen;
}
int main(int argc, char* argv[]) {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp, Edge1Math, EdgeLogical, Edge3Hard);

    std::string inputPath;
    std::string historyPath = getHistoryPath();
    bool replMode = false;
    int opt;
    while ((opt = getopt_long(argc, argv, "o:rh", longOpts, nullptr)) != -1) {
        switch (opt) {
            case 'r': replMode = true;     break;
            case 'h': printUsage(); return 0;
            default:  printUsage(); return 1;
        }
    }

    if (optind < argc)
        inputPath = argv[optind++];
    if (optind < argc) {
        std::cerr << "Error: too many input files\n";
        return 1;
    }


    auto env = EvalEnv::createGlobal();
    replxx::Replxx rx;
    rx.bind_key_internal(
        replxx::Replxx::KEY::TAB,
        "complete_next"
    );
    rx.history_load(historyPath);
    rx.set_completion_callback([&env](const std::string& input, int &contextLen){
        contextLen = getLastLen(input);
        std::string word = input.substr(input.size() - contextLen, contextLen);
        replxx::Replxx::completions_t cands;
        auto table = env->getSymbolTable();
        for(auto &[name, _]: table){
            if(name.starts_with(word))
                cands.emplace_back(name);
        }
        return cands;
    });
    if (!inputPath.empty()) {
        try {
            runFile(inputPath, env);
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    while (true) {
        try {
            std::string prompt = ">>> ";
            std::string full{};
            int num = 0;
            while (true){
                const char* rawLine = rx.input(prompt);
                if (rawLine == nullptr) {
                    rx.history_save(historyPath);
                    return 0;
                }
                std::string line(rawLine);
                num += countParens(line);
                full += line + "\n";
                prompt = "    ";
                if(num < 0){
                    std::cerr << "------------------------------------------------------------------------------------" << std::endl;
                    std::cerr << "read-syntax: unexpected `)`" << std::endl;
                    std::cerr << "------------------------------------------------------------------------------------" << std::endl;
                    num = 0;
                    full.clear();
                    prompt = "<<< ";
                    continue;
                }
                if(num <= 0)break;
            }
            rx.history_add(full);
            rx.history_save(historyPath);
            if (std::cin.eof()) {
                std::exit(0);
            }
            if (full.empty()) continue;
            auto tokens = Tokenizer::tokenize(full);
            if (tokens.empty()) continue;
            Parser parser(std::move(tokens)); // TokenPtr 不支持复制
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
