# Lv8+ 扩展方向分析

## 方向 1：添加更多内置过程

### 依赖知识
- C++ 标准库（`<cmath>`, `<string>`, `<cctype>`）
- 你的现有 `BuiltinFuncType` + `FuncArgs` 接口已经很成熟，加新函数就是照猫画虎

### 实现路线

**1A. 数学函数（30 分钟搞定一批）**

直接映射 C++ 标准库，每个函数 ~5 行：

```
sqrt sin cos tan asin acos atan exp log
floor ceiling truncate round
gcd lcm max min
```

都在 `builtins.cpp` 里加函数 + `builtins.h` 声明 + builtins map 注册。

**1B. 字符串操作（2~3 小时）**

| 函数 | 实现要点 |
|------|---------|
| `(string-length s)` | `asString()->size()` |
| `(string-ref s k)` | `(*s)[k]` |
| `(substring s start end)` | `.substr()` |
| `(string-append s1 s2 ...)` | 拼接，类似 `append` |
| `(string=? s1 s2)` | `==` 比较 |
| `(number->string n)` | `std::to_string()` |
| `(string->number s)` | `std::stod()` |
| `(string<? s1 s2)` | `<` 比较 |

每个函数 5~10 行，全在 `builtins.cpp` 中。

**1C. `read` / `readline`（半天）**

`readline` 最简实现：`std::getline(std::cin, line)` 然后 `Tokenizer::tokenize(line)` + `Parser::parse()` + 返回 ValuePtr。但 Parser 目前不支持从 stdin 读取多行表达式，需要处理 EOF。

### 评价
**难度：★☆☆☆☆** | **收益：高** | 最安全的选择，不需要改架构，零风险。

---

## 方向 2：改善用户体验（多行 REPL + 行编辑 + 高亮 + 历史）

### 目标功能
- 多行输入（括号未闭合时自动续行）
- 光标定位（左右移动、Home/End）
- 输入历史（↑↓ 翻历史记录）
- 代码高亮（关键字、字符串、注释着色）
- Tab 自动补全（符号名）

### 依赖知识

**从零写**：需要终端控制序列（ANSI escape codes）、raw mode（`termios.h` / Windows Console API）、Unicode 宽度计算——这是 C 语言底层编程，和 Lisp 解释器毫无关系，改动量极大且极易出 bug。

**用库**：`replxx` 一个头文件 + 一个 `.cpp`，CMake 引入只需 3 行，跨平台（Linux/macOS/Windows）。

### 实现路线（用 replxx）

**Step 1：引入 replxx**

```cmake
# CMakeLists.txt
include(FetchContent)
FetchContent_Declare(replxx GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git)
FetchContent_MakeAvailable(replxx)
target_link_libraries(mini_lisp replxx)
```

**Step 2：替换 main.cpp 的 REPL 循环**

```cpp
#include <replxx.hxx>

int main() {
    auto env = EvalEnv::createGlobal();
    replxx::Replxx rx;

    // 语法高亮回调：关键字蓝色、字符串绿色
    rx.set_highlighter_callback([](const std::string& input, ...) {
        // 用 tokenizer 分词后按类型着色
        replxx::Replxx::colors_t colors(input.size(), replxx::Replxx::Color::DEFAULT);
        // ... 设置各 token 的颜色
        return colors;
    });

    while (true) {
        std::string line;
        std::string prompt = ">>> ";
        std::string full_input;

        while (true) {
            auto result = rx.input(prompt);
            if (result.empty()) { std::exit(0); }
            full_input += result + "\n";
            // 括号计数判断是否完整
            if (括号平衡) break;
            prompt = "... ";
        }

        try {
            auto tokens = Tokenizer::tokenize(full_input);
            Parser parser(std::move(tokens));
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (...) { ... }
    }
}
```

**Step 3：历史记录**

```cpp
rx.history_add(full_input);  // 每次成功求值后加到历史
rx.history_save("~/.mini_lisp_history");  // 程序退出时持久化
rx.history_load("~/.mini_lisp_history");  // 启动时加载
```

**Step 4：Tab 自动补全**

```cpp
rx.set_completion_callback([](const std::string& input, int& contextLen) {
    // 从当前环境的 symbolTable 收集所有符号名
    // 匹配 input 前缀，返回候选项列表
    replxx::Replxx::completions_t completions;
    for (auto& [name, _] : env->allSymbols()) {
        if (name.starts_with(input)) completions.push_back(name);
    }
    return completions;
});
```

### 评价
**难度：★★☆☆☆（用库）/ ★★★★★（手写）** | **收益：高**

改动集中在 `main.cpp` 和 `CMakeLists.txt`，不碰 eval/parse/special forms 任何逻辑。replxx 能直接给出比肩 ipython 的交互体验。


---

## 方向 3：添加 Scheme 规范语法（排除宏）

以下特性均来自 [R5RS/R7RS-small](https://standards.scheme.org/)，按实现难度和有趣程度分类。

### 一档：一行/几行就能搞定 ★☆☆☆☆

| 特性 | 来源 | 说明 | 有趣度 |
|------|------|------|--------|
| `(positive? x)` `(negative? x)` | R5RS 6.2.5 | `x > 0` / `x < 0` | ⭐ |
| `(max x ...)` `(min x ...)` | R7RS 6.2.6 | 遍历取最大/最小 | ⭐ |
| `(square z)` | R7RS 6.2.6 | `z * z` | ⭐ |
| `(sqrt z)` `(sin z)` `(cos z)` 等 | R5RS 6.2.5 | 直接映射 `<cmath>` | ⭐ |
| `(floor x)` `(ceiling x)` `(truncate x)` `(round x)` | R5RS 6.2.5 | `<cmath>` | ⭐ |
| `(gcd n ...)` `(lcm n ...)` | R5RS 6.2.5 | `<numeric>` 或手写欧几里得 | ⭐⭐ |
| cxr 组合：`caar` `cadr` `cdar` `cddr` 等 | R5RS 6.3.2 | `(car (cdr x))` 的组合宏生成 | ⭐ |
| `(when test body...)` `(unless test body...)` | R7RS 4.2 | 语法糖，展开成 `if` | ⭐⭐ |
| `(case key clause...)` | R5RS 4.2.1 | 多分支 `eq?` 匹配 | ⭐⭐ |
| 多行注释 `#| ... |#` | R7RS 2.2 | 在 Tokenizer 跳过 | ⭐⭐ |

### 二档：1~3 小时，有实际用途 ★★☆☆☆

| 特性 | 说明 | 依赖 | 有趣度 |
|------|------|------|--------|
| **字符串操作族** `string-length` `string-ref` `string-set!` `substring` `string-append` `string=?` `string<?` `string->list` `list->string` | 需要 `StringValue` 暴露原始 string，已有的 `asString()` 正好用上 | 无 | ⭐⭐⭐ |
| **字符类型 + 操作** `char?` `char=?` `char<?` `char->integer` `integer->char` `char-alphabetic?` `char-numeric?` | 新增 `CharValue` 类型，内部存 `char32_t`（Unicode），约 100 行 | 新类型 | ⭐⭐ |
| **`(let* ((x v)...) body)`** | 嵌套 `let`：`(let ((x1 v1)) (let ((x2 v2)) ... body))`，特殊形式 ~10 行 | 无 | ⭐⭐ |
| **`(letrec ((x v)...) body)`** | 递归绑定，先用 `#f` 占位再逐个 set! | 需要 `set!` | ⭐⭐⭐ |
| **`(do ((var init step)...) (test result) body...)`** | 迭代，展开成命名 let | 中等 | ⭐⭐ |
| **`(set! var val)`** | 修改变量值，`EvalEnv` 加 `modifyVariable`（沿父链查找） | 小改架构 | ⭐⭐ |
| **`(set-car! pair val)` `(set-cdr! pair val)`** | `PairValue` 的 `lptr`/`rptr` 改非 const 加 setter | 小改架构 | ⭐⭐⭐ |
| **`(delay expr)` `(force p)`** | 惰性求值，新增 `PromiseValue` 类型。最"函数式"的特性，可以写无限流 | 新类型 | ⭐⭐⭐⭐⭐ |
| **`(error msg)` 增强** | 支持 `(error "message" irritant...)` | 已大致支持 | ⭐ |
| **`(apply proc list)` 多参数扩展** | R7RS 允许 `(apply proc a1 a2 ... list)` | 小改动 | ⭐ |

### 三档：半天~一天，需要新增数据类型 ★★★☆☆

| 特性 | 说明 | 依赖 | 有趣度 |
|------|------|------|--------|
| **向量 `(scheme base)`** `vector?` `make-vector` `vector` `vector-ref` `vector-set!` `vector-length` `vector->list` `list->vector` `vector-fill!` `vector-copy!` | 新增 `VectorValue` 类型，内部 `std::vector<ValuePtr>`。全套 ~15 个函数，模式跟 list 类似 | 新类型 | ⭐⭐⭐ |
| **`(call-with-current-continuation)` (call/cc)** | "时间旅行"——捕获当前执行状态。需要 CPS 变换或栈复制，是 Scheme 最标志性也是最难实现的功能 | 架构级挑战 | ⭐⭐⭐⭐⭐ |
| **`(values obj ...)` `(call-with-values producer consumer)`** | 多返回值，需要改造 `eval`/`apply` 的返回类型 | 架构挑战 | ⭐⭐ |
| **异常处理** `guard` `raise` `with-exception-handler` | 需要区分异常类型，比当前的 `try/catch` 精细 | 小改架构 | ⭐⭐⭐ |

### 组合推荐

| 方案 | 包含 | 工作量 | 效果 |
|------|------|--------|------|
| **基础包** | string 全族 + `let*` + `case` + `set!` + `delay/force` | 半天 | 语言能力大幅提升 |
| **进阶包** | 基础包 + vector 全族 + `letrec` + `do` | 一天半 | 接近 R5RS 核心 |

### 评价
字符串、`delay/force`、vector 最值得做。call/cc 是陷阱——看起来炫但实现代价极大。宏系统已被排除。

---

## 方向 4：解决内存泄漏（不一定要完整 GC）

### 助教提示解读

> 首先你得理解你的程序在什么情况下会发生内存泄露

**泄漏场景**：LambdaValue 捕获了定义时的环境（parent），该环境又通过主环境的 symbolTable 间接引用回了 LambdaValue，形成 shared_ptr 循环：

```
主环境 (EvalEnv)
  → symbolTable["f"] → LambdaValue
                          → parent (子环境 E_child)
                              → parent (弱引用？不，是 shared_ptr)
                                  → 主环境  ← 成环！
```

> 一个很常见的可能导致内存泄露的输入

助教指的 "devil" 大概是这样的模式：

```scheme
(define (make-counter)
  (let ((count 0))
    (lambda () 
      (set! count (+ count 1)) 
      count)))
(define c (make-counter))
(c)  ; → 1
(c)  ; → 2
```

每次 `(make-counter)` 创建一个子环境 E1（count=0），返回的 lambda 持有 `shared_ptr<EvalEnv>` 指向 E1。lambda 被存在主环境的 symbolTable。E1 的 `parent` 是 `shared_ptr<EvalEnv>` 指向主环境。形成：

```
主环境 → "c" → LambdaValue → E1 → 主环境
 ↑__________________________________|
```

`shared_ptr` 的引用计数永远 ≥ 2，三者都不会被析构。每调一次 `make-counter` 就泄漏一个 E1。

> 只想解决这个问题未必需要实现完整的标记-清除

### 方案 A：最小改动——打破 LambdaValue 这一环 ★★☆☆☆

让 LambdaValue 的 `parent` 用 `std::weak_ptr<EvalEnv>` 代替 `shared_ptr`。

**问题**：`weak_ptr` 不阻止析构。如果 LambdaValue 是 E_child 的唯一持有者，E_child 会被提前析构，调用 lambda 时 `parent.lock()` 返回空。

**解决**：在 `apply` 时 lock，如果过期说明定义环境已被释放，抛错（这在实际使用中不会发生——因为 lambda 总是被存在某个环境中，环境被释放意味着 lambda 也不可用了）。

但实际上这**不解决问题**：E_child 确实需要一个强引用才能存活。weak_ptr 让 E_child 在 lambda 存在的情况下也被释放了，这不符合语义。

### 方案 B：让 EvalEnv 的 parent 用 weak_ptr ★★★☆☆

LambdaValue → E1 保持 shared_ptr。E1 → 主环境改为 weak_ptr。

```
主环境 → "c" → LambdaValue → E1 → 主环境(weak)
 ↑__________________________________|
```

环被打破！主环境释放时：c 被销毁 → LambdaValue 析构 → E1 析构 → E1 的 parent 是 weak_ptr，不影响主环境。

**代价**：`lookupBinding` 每次跨环境查找都要 `parent.lock()`，略增开销。主环境用全局 shared_ptr 持有，lock 永远不会失败。

**代码改动**：
- `EvalEnv::parent`：`shared_ptr<EvalEnv>` → `weak_ptr<EvalEnv>`
- `lookupBinding`：`else if(parent)` → `else if(auto p = parent.lock())`
- `createChild`：`env->set_parent(shared_from_this())` → `env->set_parent(shared_from_this())`（不变，weak_ptr 可从 shared_ptr 隐式构造）
- `set_parent`：参数改为 `shared_ptr<EvalEnv>`→内部存为 weak_ptr

**验证**：写一个循环调用 `make-counter` 的测试，观察内存是否稳定。

### 方案 C：完整标记-清除 GC ★★★★★

真正的 GC 需要全局对象池、根集遍历、标记可达、清除不可达。工作量大但彻底解决所有泄漏。

### 评价
**推荐方案 B**：只改 `EvalEnv::parent` 一个字段的类型 + `lookupBinding` 一处逻辑，~10 行改动，彻底打破 shared_ptr 循环，不引入新复杂度。
