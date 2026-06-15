# Scheme 语法特性分级实现指南

基于 [R5RS](https://schemers.org/Documents/Standards/R5RS/HTML/) 和 [R7RS-small](https://small.r7rs.org/) 标准，按照当前项目已有基础设施，按实现难度分为五档。

---

## 已经是现的（Lv2~Lv7）

| 分类 | 项目 |
|------|------|
| 特殊形式 | `quote` `'` `quasiquote` `` ` `` `unquote` `,` `define` `lambda` `if` `cond` `and` `or` `begin` `let` |
| 类型检查 | `boolean?` `number?` `symbol?` `string?` `pair?` `null?` `procedure?` `integer?` `list?` `atom?` |
| 算术 | `+` `-` `*` `/` `abs` `expt` `quotient` `modulo` `remainder` |
| 比较 | `=` `<` `>` `<=` `>=` `even?` `odd?` `zero?` `eq?` `equal?` `not` |
| 列表 | `car` `cdr` `cons` `list` `length` `append` `map` `filter` `reduce` |
| IO | `print` `display` `newline` `displayln` `error` |
| 控制 | `apply` `eval` `exit` |

---

## Lv1：一行 C++ 就能搞定（20 分钟以内）

这些对应到 `<cmath>` 或简单单行逻辑，不需要新增数据结构。

| 函数 | 实现 |
|------|------|
| `(positive? x)` | `x > 0` |
| `(negative? x)` | `x < 0` |
| `(max x ...)` | 遍历取最大 |
| `(min x ...)` | 遍历取最小 |
| `(square x)` | `x * x` |
| `(sqrt x)` | `std::sqrt(x)` |
| `(floor x)` | `std::floor(x)` |
| `(ceiling x)` | `std::ceil(x)` |
| `(truncate x)` | `std::trunc(x)` |
| `(round x)` | `std::round(x)` |
| `(sin x)` `(cos x)` `(tan x)` | `std::sin` / `cos` / `tan` |
| `(exp x)` `(log x)` | `std::exp` / `std::log` |
| `(caar x)` `(cadr x)` 等 cxr | `(car (car x))` 组合，代码生成即可 |

---

## Lv2：需要写短函数但有现成 infrastructure（30 分钟~1 小时）

| 功能 | 难度 | 说明 |
|------|------|------|
| `(gcd a b)` `(lcm a b)` | 简单 | `std::gcd` / 辗转相除 |
| `(exact-integer-sqrt n)` | 简单 | `std::sqrt` 取整数部分 |
| `(reverse lst)` | 简单 | `toVector` → 反转 → `ToList` |
| `(list-ref lst k)` | 简单 | 沿 cdr 走 k 步取 car |
| `(list-tail lst k)` | 简单 | 沿 cdr 走 k 步 |
| `(make-list k fill)` | 简单 | 创建 k 个 fill 的列表 |
| `(for-each proc lst)` | 简单 | 和 `map` 一样但不收集结果，返回 nil |
| `(member x lst)` | 简单 | 用 `equal?` 比较，返回匹配位置 |
| `(memq x lst)` | 简单 | 用 `eq?` 比较 |
| `(assq key alist)` `(assoc key alist)` | 简单 | 关联表查找 |
| `(symbol->string sym)` | 简单 | `SymbolValue::toString()` |
| `(string->symbol str)` | 简单 | 构造 `SymbolValue` |
| `(not x)` | 已实现 | — |
| `(when test body...)` | 极简 | `(if test (begin body...) #f)` 的宏展开 |
| `(unless test body...)` | 极简 | `(if test #f (begin body...))` 的宏展开 |
| `(case key clause...)` | 简单 | 逐条 `eq?` 比较，类似 cond |
| `(let* ((x v) ...) body)` | 简单 | 嵌套 `let`：`(let ((x1 v1)) (let ((x2 v2)) ... body))` |
| `write` / `read` | 中等 | `write` 类似 `display` 但要处理字符串引号；`read` 需要把 stdin tokenize+parse |

---

## Lv3：需要小改架构（1~3 小时）

| 功能 | 难点 | 需要改的地方 |
|------|------|-------------|
| `(set! var val)` | 修改已有变量的值 | `EvalEnv` 需要 `modifyVariable`（在父环境找绑定）而不是当前环境的 `addVariable` |
| `(set-car! pair val)` | pair 的可变性 | `PairValue` 的 `lptr` 改为非 const，加 setter |
| `(set-cdr! pair val)` | pair 的可变性 | 同上 |
| `(letrec ((x v) ...) body)` | 递归绑定 | 先用空值占位再赋值，和 `letrec*` 类似 |
| `(do ((var init step) ...) (test result) body...)` | 迭代 | 展开成命名 let，不难但逻辑繁琐 |
| `(delay expr)` `(force p)` | 惰性求值 | 加 `PromiseValue` 类型，存 thunk 和已求值结果 |
| 字符串操作族 | 新增方法 | `string-length` `string-ref` `string-set!` `substring` `string-append` `string=?` 等，需要给 `StringValue` 加字符级接口 |
| `(number->string n)` `(string->number s)` | 转换 | 已有 `toString` / `std::stod` |

---

## Lv4：需要新建数据类型（半天~一天）

| 功能 | 难点 |
|------|------|
| **向量**：`vector?` `make-vector` `vector` `vector-ref` `vector-set!` `vector-length` `vector->list` `list->vector` | 新增 `VectorValue` 类型，内部存 `std::vector<ValuePtr>` |
| **字符**：`char?` `char=?` `char<?` `char->integer` `integer->char` | 新增 `CharValue` 类型，内部存 `char` |
| **`case-lambda`** | 多参数版本的 lambda，选择匹配的形参个数 |
| **`let-values` `let*-values`** | 依赖多返回值 `values` / `call-with-values` |
| **`call-with-input-file` `call-with-output-file`** | 已具备文件 IO 基础（`ifstream`/`ofstream`），封装即可 |
| **`guard`** | 异常处理，需要能区分异常类型 |

---

## Lv5：架构级挑战（不推荐课上做）

| 功能 | 为什么难 |
|------|---------|
| **`call-with-current-continuation` (call/cc)** | 需要捕获调用栈，要么做 CPS 变换，要么复制整个 C++ 运行时栈——在有异常和 RAII 的 C++ 里几乎不可能正经做 |
| **`dynamic-wind`** | 依赖 call/cc |
| **`define-record-type`** | 需要编译期代码生成或运行时类型构造 |
| **宏系统 (`define-syntax` + `syntax-rules`)** | 需要 parser 之后加一个宏展开阶段，模式匹配 + 模板替换 |
| **`define-library` / `import` / `export`** | 需要模块系统，跨文件符号管理 |
| **字节向量 (bytevectors)** | 新类型 + 全套操作 |
| **完整数值塔**（complex / rational / exact-inexact） | 需要大整数、分数、复数三种新数值类型 |
| **`define-values`** / `values` / `call-with-values` | 需要语言内部支持多返回值 |

---

## 总结

| 等级 | 项目数（估计） | 工作量 | 对分数的帮助 |
|------|:---:|:---:|------|
| Lv1（一行搞定） | ~30 | 半天 | 补全基本数学库和 cxr |
| Lv2（短函数） | ~25 | 1 天 | 列表操作、简单 IO、`let*` `case` `when` |
| Lv3（小改架构） | ~15 | 2~3 天 | `set!`、`set-car!/cdr!`、字符串、延迟求值 |
| Lv4（新数据类型） | ~20 | 1 周 | 向量、字符、case-lambda、文件 IO |
| Lv5（架构级） | ~10 | 不建议 | macro、call/cc、模块、数值塔 |
