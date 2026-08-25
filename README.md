# 命题逻辑 G' 自动定理证明器

本项目为南京大学2026春季数理逻辑课程的课程大作业，实现了一个基于 Gentzen 系统 G' 的命题逻辑自动定理证明器。输入序贯 `Γ |- Δ`，自动搜索证明树；可证则输出证明树，不可证则输出反例（真值赋值）。

**直接照搬本项目代码可能会导致你的作业因为查重而扣分**

## 功能特性

- 支持命题公式：原子、否定 `~`、合取 `&`、析取 `|`、蕴含 `->`
- 序贯输入：`Γ |- Δ`，左右公式用逗号分隔，允许空格
- 自动证明：输出证明树（无 Cut 规则）
- 反例生成：不可证时输出一组使 Γ 全真、Δ 全假的赋值
- 步数限制：使用 `step` 控制递归搜索深度

## 项目结构

核心文件：

- `include/prop.h` / `src/prop.cpp`：命题公式结构与打印
- `include/sequent.h` / `src/sequent.cpp`：序贯结构与打印
- `include/proof_tree.h` / `src/proof_tree.cpp`：证明树结构与打印
- `include/parser.h` / `src/parser.cpp`：递归下降解析器
- `include/solver.h` / `src/solver.cpp`：证明搜索与规则应用
- `include/counterexample.h` / `src/counterexample.cpp`：反例生成
- `src/main.cpp`：命令行入口
- `src/tests.cpp`：测试入口

## 构建

```bash
cmake -S . -B build
cmake --build build -j
```

生成的可执行文件：

- `build/prover`
- `build/tests`

## 使用方法

程序从标准输入读取一行序贯：

```bash
echo "(0->1), 0 |- 1" | ./build/prover
```

输出格式：

- 可证：
  - `Provable`
  - 证明树
- 不可证：
  - `Unprovable`
  - `Counterexample: {0: true, 1: false}`（示例）
- 资源耗尽：
  - `Unknown (step exhausted)`
- 解析错误：
  - `Parse error: ...`

## 输入格式

- 原子命题：非负整数，如 `0`、`1`、`12`
- 否定：`~0`
- 合取：`0&1`
- 析取：`0|1`
- 蕴含：`0->1`
- 括号：`( ... )`
- 序贯：`Γ |- Δ`，左右公式逗号分隔
- 允许任意空格

示例：

- `0 |- 0`
- `(0->1), 0 |- 1`
- `~(0|1) |- (~0 & ~1)`
- `|- ((0->1)->0)->0`
- `(0|1) |- (0&1)`

## 运行测试

```bash
./build/tests
```

测试覆盖：

- Case1: `0 |- 0`
- Case2: `(0->1), 0 |- 1`
- Case3: `~(0|1) |- (~0 & ~1)`
- Case4: `|- ((0->1)->0)->0`
- Case5: `(0|1) |- (0&1)` + 反例验证

## 注意事项

- 默认步数限制在 `src/main.cpp` 中的 `kDefaultStep`。
- 反例枚举变量上限为 20（位于 `src/counterexample.cpp`），超过会返回 `Counterexample: unknown`。
- 本实现不包含 Cut 规则。

## 常见问题

- 出现 `Unknown (step exhausted)`：提高`main.cpp`中的 `kDefaultStep` 或简化输入。
- 出现解析错误：检查括号是否匹配、`|-` 是否存在、连接词是否拼写正确。
