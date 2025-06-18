# sp.cpp 代码性能优化计划

## 1. 现有代码分析 (`sp.cpp`)

当前代码的目标是在一个 6x6 的网格中找出所有选择 16 个格子的方案，要求这些方案满足一个条件：在任何一行或任何一列上，都不能有连续的 3 个被选中的格子。

*   `init()` 函数初始化了一个 `todo` 向量，存储了 6x6 网格的所有 36 个坐标。
*   `isok()` 函数用于检查一个给定的 16 个格子的组合是否满足“无连续三子”的条件。它通过修改全局变量 `pan` 来标记选中的格子，然后检查行和列。
*   `main()` 函数的核心是 **16 层嵌套的 `for` 循环**，尝试遍历所有从 36 个格子中选择 16 个的组合。

## 2. 主要性能瓶颈

最大的性能问题在于 `main` 函数中的 **16 层嵌套 `for` 循环**。这种方式生成组合的计算量极其巨大 (组合数 C(36, 16) 大约是 9.4 x 10^9)，导致程序运行时间非常长。

**其他潜在问题:**

*   **全局变量修改:** `isok` 函数直接修改全局变量 `pan`，是不好的实践。
*   **未使用的参数:** `isok` 函数接收参数 `pa` 但未使用。
*   **打印逻辑错误:** `main` 函数中打印结果的部分存在数组越界访问。

## 3. 优化策略

我们将采用**递归回溯 (Depth First Search, DFS)** 结合**剪枝**的策略来替代暴力枚举。

## 4. 详细实施计划

1.  **移除嵌套循环:** 删除 `main` 函数中所有的嵌套 `for` 循环。
2.  **设计递归函数:** 创建一个递归函数，例如 `findSolutions(int k, int start_index, vector<vector<int>>& board, vector<pair<int, int>>& current_selection)`:
    *   `k`: 当前还需要选择多少个格子。
    *   `start_index`: 在 36 个格子的一维索引中，从哪个索引开始考虑选择。
    *   `board`: 当前的 6x6 棋盘状态（例如，0 表示选中，-1 表示未选中）。
    *   `current_selection`: 存储当前已选中的格子坐标。
3.  **递归逻辑:**
    *   **基本情况:** 如果 `k == 0`，打印 `current_selection` 中的坐标，然后返回。
    *   **递归步骤:** 从 `start_index` 遍历到 35。对于每个索引 `i`：
        *   获取坐标 `(r, c)`。
        *   尝试选择：标记 `board[r][c]=0`，添加 `(r, c)` 到 `current_selection`。
        *   **剪枝检查:** 调用 `check(r, c, board)` 检查新格子是否导致三连。
        *   如果 `check` 返回 `true` (有效)：递归调用 `findSolutions(k - 1, i + 1, board, current_selection)`。
        *   **回溯:** 恢复 `board[r][c]=-1`，从 `current_selection` 移除 `(r, c)`。
4.  **剪枝函数 `check(r, c, board)`:**
    *   接收新格子坐标 `(r, c)` 和棋盘 `board`。
    *   检查 `r` 行和 `c` 列以 `(r, c)` 为中心的小范围是否有连续三个 0。
    *   返回 `false` (无效/剪枝) 或 `true` (有效)。
5.  **初始化:**
    *   在 `main` 中初始化 6x6 `board` (-1) 和空 `selection`。
    *   调用 `findSolutions(16, 0, board, selection)` 开始搜索。
6.  **移除全局变量:** 避免使用全局 `pan` 和 `todo`。通过参数传递状态。
7.  **修复打印:** 在递归的基本情况中正确打印 `current_selection`。

## 5. Mermaid 图 (优化后流程)

```mermaid
graph TD
    A[开始 main 函数] --> B(初始化空棋盘 board, 空选择 selection);
    B --> C{调用 findSolutions(16, 0, board, selection)};
    C --> D{递归: k == 0? (已选16个)};
    D -- 是 --> E(打印 selection 中的坐标);
    D -- 否 --> F{循环 i 从 start_index 到 35};
    F --> G[获取格子 (r, c) 从索引 i];
    G --> H{尝试选择 (r, c)};
    H --> I(标记 board[r][c]=0, 添加 (r,c) 到 selection);
    I --> J{调用 check(r, c, board)};
    J -- 返回 true (有效) --> K{递归调用 findSolutions(k-1, i+1, board, selection)};
    J -- 返回 false (无效/剪枝) --> L(跳过此选择);
    K --> M(回溯: 恢复 board[r][c]=-1, 从 selection 移除 (r,c));
    L --> M;
    M --> F; // 继续循环下一个 i
    E --> C; // 返回上一层递归
```

## 6. 总结

该计划通过 DFS 和剪枝显著减少计算量，提高性能，并修复原代码问题。