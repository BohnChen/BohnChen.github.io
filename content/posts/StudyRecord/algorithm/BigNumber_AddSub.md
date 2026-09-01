---
title: "3_String_大数加减法"
date: 2026-09-01T10:00:00+08:00
draft: false
categories: ["算法"]
tags: ["string", "技术学习"]
---

## 题目引入
在 C++ 中，内置整数类型（`int`、`long long`）都有固定的位数上限。当两个几十位甚至上百位的数字相加、相减时，计算结果根本存不进任何内置类型，直接运算就会发生**溢出**，得到错误答案。

这时就需要我们自己模拟"手算"的过程：**把大数按位拆开存储，用数组/字符串一位一位地运算，并处理进位与借位**。这就是大数加减法的核心思想。

## 数据存储
最直观的方式是用 `string` 存大数，字符串的每一位 `'0' ~ '9'` 就代表一位十进制数字。**统一约定低位在后**（也就是字符串的最右边是个位），这样进位/借位发生时直接往末尾追加即可，无需移动大量元素。

加法、减法都在"逐位对齐 + 进位/借位"的基础上完成，写之前先有一个比较两个数大小的工具函数：

```cpp
// 比较两个非负大数的大小，返回 a > b 时为 true
bool cmp(string &a, string &b) {
    if (a.size() != b.size()) return a.size() > b.size();
    return a >= b; // 长度相同时，字典序比较即数字大小
}
```

## 加法实现
模拟竖式加法：从低位（末尾）向高位逐位相加，`sum / 10` 是进位，`sum % 10` 是当前位的结果。循环条件包含 `carry`，是为了处理最后一位相加后仍然产生进位的情况。

```cpp
string add(string a, string b) {
    string res;
    int i = a.size() - 1, j = b.size() - 1;
    int carry = 0; // 进位
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += a[i--] - '0';
        if (j >= 0) sum += b[j--] - '0';
        carry = sum / 10;
        res.push_back(sum % 10 + '0');
    }
    reverse(res.begin(), res.end()); // 低位在后，翻转回正常顺序
    return res;
}
```

关键点在于：
1. `i`、`j` 都从末尾开始，模拟从低位逐位对齐相加；
2. 某一方先遍历完时，只累加另一方剩余位；
3. 最后一位也可能进位，所以循环条件里必须有 `carry`。

## 减法实现
减法比加法多一个"借位"概念，而且前提是**被减数不小于减数**。所以先约定 `sub(a, b)` 只处理 `a >= b` 的情况；若 `a < b`，就计算 `sub(b, a)` 并加负号。这和现实中"小的减大的"要借负号一个道理。

```cpp
// 前提：a >= b，且 a、b 都是非负大数
string sub(string a, string b) {
    string res;
    int i = a.size() - 1, j = b.size() - 1;
    int borrow = 0; // 借位
    while (i >= 0) {
        int d = (a[i--] - '0') - borrow; // 先扣掉上次的借位
        if (j >= 0) d -= (b[j--] - '0');
        if (d < 0) {       // 当前位不够减，向高位借 1
            d += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res.push_back(d + '0');
    }
    // 去掉前导零，但至少保留一位（结果为 0 时输出 "0"）
    while (res.size() > 1 && res.back() == '0') res.pop_back();
    reverse(res.begin(), res.end());
    return res;
}
```

减法需要处理两个细节：
- **借位**：当前位不够减时，`d += 10` 表示向高位借了 1，并把 `borrow` 置 1；
- **前导零**：比如 `100 - 99 = 001`，结果要去掉开头的 0，只剩 `1`。

## 带符号的加减法
把两个工具函数组合起来，就能支持正负号，覆盖完整的加减法：

```cpp
string big_add(string a, string b) {
    int sa = (a[0] == '-'), sb = (b[0] == '-');
    if (sa) a.erase(a.begin());
    if (sb) b.erase(b.begin());
    if (sa == sb) {                    // 同号相加
        string t = add(a, b);
        return (sa && t != "0") ? "-" + t : t;
    }
    // 异号：转换成减法，绝对值大的符号为准
    if (cmp(a, b)) {
        string t = sub(a, b);
        return sa ? "-" + t : t;
    } else {
        string t = sub(b, a);
        return sb ? "-" + t : t;
    }
}

string big_sub(string a, string b) {
    if (b[0] == '-') b.erase(b.begin());   // 减负数等于加正数
    else b = "-" + b;
    return big_add(a, b);
}
```

组合逻辑简单说就是：**"同号相加，异号转减法"、"减去一个数等于加上它的相反数"**，这样加减法就统一了。

## 总结
1. 大数存不下是因为内置类型位数有限，解决办法是**用字符串/数组按位存储**；
2. 加法核心是**进位**（`sum / 10`），减法是**借位**（`d += 10`）；
3. 从低位往高位逐位运算，最后注意**前导零**和**末位进位**两个边界；
4. 带符号时，统一成"同号相加、异号转减"，符号取绝对值大的一方。

上面的做法是十进制逐位模拟，正确易懂，是算法题的标准解法。如果需要更高性能，可以把多位数字压进一个数组元素（如 `vector<int>` 每格存 0~9 亿，即 `10^9` 进制），并配合 Karatsuba、FFT 等算法加速乘法，但加减法的借位/进位思路是完全一致的。
