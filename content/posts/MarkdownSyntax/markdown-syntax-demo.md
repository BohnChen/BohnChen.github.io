---
title: "Markdown 语法示例"
date: 2026-08-13T15:10:44+08:00
draft: false
categories: ["文档书写"]
tags: ["Blowfish", "Markdown"]
---

这篇文章演示了本站支持的各种 Markdown 语法，本地 `hugo server` 预览可看实际效果。

## 标题

# 一级标题
## 二级标题
### 三级标题
#### 四级标题

## 下划线与颜色

<u>这是下划线文字</u>

<ins>这也是下划线（语义为"插入"）</ins>

<mark>黄色高亮文字</mark>

<span style="color:red">红色文字</span>

<span style="color:#3b82f6">蓝色文字</span>

<span style="background:#ffe066; padding:2px 6px">带背景色的文字</span>

## 文字强调

**加粗**
*斜体*
***加粗斜体***
~~删除线~~
`行内代码`

## 引用

> 这是一段引用。嵌套引用：
> > 第二层引用

## 列表

- 无序列表项 1
- 无序列表项 2
  - 嵌套子项

1. 有序列表项 1
2. 有序列表项 2

## 任务列表

- [x] rst
- [ ] arst
- [x] 已完成的任务
- [ ] 未完成的任务

## 代码块（带语言高亮）

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

```python
def greet(name):
    return f"Hello, {name}"
```

## 表格

| 功能 | 语法 | 说明 |
|------|------|------|
| 加粗 | `**文字**` | 强调 |
| 代码 | `` `代码` `` | 行内代码 |
| 链接 | `[文字](url)` | 超链接 |

## 链接与图片

[这是一个链接](https://bohnchen.github.io/)

![图片描述](/images/1_LiLi_CPP_Overlook.png)

## 上标与下标

圆的面积公式：A = πr<sup>2</sup>

水的化学式：H<sub>2</sub>O

## 脚注

这是一个带脚注的文字[^1]。

[^1]: 这是脚注的内容。

## 分割线

---

## Blowfish 专属 Shortcodes

### 提示框

{{< alert >}}
这是默认提示框。
{{< /alert >}}

{{< alert icon="fire" cardColor="#3b82f6" textColor="#fff" >}}
这是自定义颜色的提示框。
{{< /alert >}}

### 徽章

{{< badge >}}默认徽章{{< /badge >}}
{{< badge >}}蓝色徽章{{< /badge >}}

### 按钮

{{< button href="/posts/" target="_self" >}}查看文章{{< /button >}}

### 图标

{{< icon "github" >}}　{{< icon "star" >}}　{{< icon "heart" >}}

### 数学公式（KaTeX）

块级公式：

$$E = mc^2$$

行内公式：勾股定理 \(a^2 + b^2 = c^2\)

### 流程图（Mermaid）

{{< mermaid >}}
graph TD
    A[开始] --> B{判断}
    B -->|是| C[执行]
    B -->|否| D[结束]
    C --> D
{{< /mermaid >}}

## 总结

以上基本涵盖了本站可用的 Markdown 语法。核心要点：

1. **下划线、颜色**用 HTML 标签（`<u>`、`<mark>`、`<span>`）
2. **提示框、徽章、按钮**等用 Blowfish shortcode
3. **数学公式、流程图**分别用 `math` 和 `mermaid` shortcode
