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
#include "./repl_editor.h"

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
std::string getHistoryPath() {
    const char* home = std::getenv("USERPROFILE");
    if (!home) home = std::getenv("HOME");   
    if (!home) return "mini_lisp_history.txt"; 
    return std::string(home) + "/.mini_lisp_history";
}
int main(int argc, char* argv[]) {
    RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
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
    ReplEditor editor(rx, env, historyPath);
    if (!inputPath.empty()) {
        try {
            runFile(inputPath, env);
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }
    editor.install();
    while (true) {
        try {
            const char* rawLine = rx.input(">>> ");
            if (rawLine == nullptr) {
                editor.saveHistory();
                return 0;
            }
            std::string full(rawLine);
            if (full.empty()) continue;
            editor.addHistory(full);
            editor.saveHistory();
            auto tokens = Tokenizer::tokenize(full);
            if (tokens.empty()) continue;
            Parser parser(std::move(tokens));
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
