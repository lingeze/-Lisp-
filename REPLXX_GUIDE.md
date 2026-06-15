# replxx 快速入门指南

## 是什么

replxx 是一个跨平台（Windows/Linux/macOS）的行编辑库，相当于 GNU readline 的现代替代品。你的 REPL 目前用 `std::getline(std::cin, line)`，换成 replxx 后自动获得：

- 光标左右移动（← → Home End Ctrl+A Ctrl+E）
- 输入历史（↑ ↓ 翻历史、Ctrl+R 搜索）
- Tab 自动补全
- 语法高亮（关键字着色）
- 多行编辑
- UTF-8 支持

## 怎么装

你的项目用 CMake，只需在 `CMakeLists.txt` 加 3 行：

```cmake
include(FetchContent)
FetchContent_Declare(replxx
    GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
    GIT_TAG release-0.0.4)
FetchContent_MakeAvailable(replxx)
target_link_libraries(mini_lisp replxx)
```

CMake 会自动下载、编译、链接 replxx，你不需要手动安装任何东西。

## 核心 API（7 个方法就够用）

### 头文件

```cpp
#include <replxx.hxx>
```

### 1. 创建实例 + 读输入

```cpp
replxx::Replxx rx;

// 读取一行输入，显示 ">>> " 提示符
std::string line = rx.input(">>> ");
// 用户按 Ctrl+D 返回空字符串
```

### 2. 历史记录

```cpp
// 启动时加载上次的历史
rx.history_load("~/.mini_lisp_history");

// 每次成功求值后存一行
rx.history_add(line);

// 退出前保存（可选，也可以每行后立刻 save）
rx.history_save("~/.mini_lisp_history");
```

### 3. Tab 补全

```cpp
rx.set_completion_callback(
    [&env](const std::string& input, int& contextLen) {
        replxx::Replxx::completions_t completions;
        // 从当前环境的符号表搜集所有候选项
        for (auto& [name, _] : your_get_all_symbols(env)) {
            if (name.starts_with(input))
                completions.emplace_back(name);
        }
        return completions;
    }
);
```

### 4. 语法高亮

```cpp
rx.set_highlighter_callback(
    [](const std::string& input, replxx::Replxx::colors_t& colors) {
        // input: 当前输入行
        // colors: 和 input 等长的数组，每个位置设置颜色
        // 用 tokenizer 分词后按 token 类型着色
        // 返回一个颜色数组
    }
);
```

颜色常量（以 `Replxx::Color::` 为前缀）：
```
DEFAULT, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
BRIGHTBLACK, BRIGHTRED, ...
BOLD, UNDERLINE
```

### 5. 多行续行提示

```cpp
// 括号没闭合时继续读
std::string full_input;
std::string prompt = ">>> ";
while (true) {
    std::string line = rx.input(prompt);
    if (line.empty()) exit(0);
    full_input += line + "\n";
    if (括号平衡(full_input)) break;
    prompt = "... ";
}
```

### 6. 清屏

```cpp
rx.clear_screen();  // Ctrl+L 效果
```

### 7. 打印（带高亮）

```cpp
rx.print("Hello\n", replxx::Replxx::Color::GREEN);
```

## 最小可用模板

把你的 `main.cpp` 的 while 循环替换成：

```cpp
#include <replxx.hxx>

int main() {
    auto env = EvalEnv::createGlobal();
    replxx::Replxx rx;
    rx.history_load(".mini_lisp_hist");

    while (true) {
        std::string line = rx.input(">>> ");
        if (line.empty()) { std::exit(0); }

        rx.history_add(line);

        try {
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));
            auto result = env->eval(parser.parse());
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
```

这样就获得了**光标编辑 + ↑↓ 历史**。补全和高亮是后续可选增强。

## 学习路线

1. **第 1 步（10 分钟）**：按上面模板替换 REPL 循环，跑起来试试
2. **第 2 步（20 分钟）**：加历史持久化（load/save）
3. **第 3 步（20 分钟）**：加多行续行（括号计数判断是否完整）
4. **第 4 步（30 分钟）**：加 Tab 补全（回调里遍历 symbolTable）
5. **第 5 步（1 小时）**：加语法高亮（回调里用你的 Tokenizer 分词按类型着色）

前两步做完就已经比现在的 `getline` 好 10 倍了。后面几步是锦上添花。

## 完整 API 参考

GitHub: https://github.com/AmokHuginnsson/replxx
头文件即文档：`replxx.hxx` 里的注释非常详尽，每个方法都有说明。
