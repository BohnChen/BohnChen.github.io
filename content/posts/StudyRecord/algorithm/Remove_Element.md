---
title: "2_Array_LC27_双指针"
date: 2026-08-24T09:12:55+08:00
draft: false
categories: ["算法"]
tags: ["leetcode", "技术学习"]
---

这是《代码随想录》[^1]，数组章节的第二道题目。
[^1]:[代码随想录官网](https://programmercarl.com/)

## 题目描述
给你一个数组 `nums` 和一个值 `val`，你需要 **原地** 移除所有数值等于 `val` 的元素。元素的顺序可能发生改变。然后返回 `nums` 中与 `val` 不同的元素的数量。

假设 `nums` 中不等于 `val` 的元素数量为 `k`，要通过此题，您需要执行以下操作：

- 更改 `nums` 数组，使 `nums` 的前 `k` 个元素包含不等于 `val` 的元素。`nums` 的其余元素和 `nums` 的大小并不重要。
- 返回 `k`。

**用户评测：**

评测机将使用以下代码测试您的解决方案：

```java
int[] nums = [...]; // 输入数组
int val = ...; // 要移除的值
int[] expectedNums = [...]; // 长度正确的预期答案。
                            // 它以不等于 val 的值排序。

int k = removeElement(nums, val); // 调用你的实现

assert k == expectedNums.length;
sort(nums, 0, k); // 排序 nums 的前 k 个元素
for (int i = 0; i < k; i++) {
    assert nums[i] == expectedNums[i];
}
```

如果所有的断言都通过，你的解决方案将会 **通过**。

**示例 1:**

**输入:** nums = [3,2,2,3], val = 3

**输出:** 2, nums = [2,2,_,_]

**解释:** 你的函数应该返回 `k = 2`，并且 `nums` 中的前两个元素均为 2。你在返回的 `k` 个元素之外留下了什么并不重要（因此它们并不计入评测）。

**示例 2:**

**输入:** nums = [0,1,2,2,3,0,4,2], val = 2

**输出:** 5, nums = [0,1,4,0,3,_,_,_]

**解释:** 你的函数应该返回 `k = 5`，并且 `nums` 中的前五个元素为 0,0,1,3,4。注意这五个元素可以任意顺序返回。你在返回的 `k` 个元素之外留下了什么并不重要（因此它们并不计入评测）。

**提示：**

- `0 <= nums.length <= 100`
- `0 <= nums[i] <= 50`
- `0 <= val <= 100`

## 我的代码
实现的思路是慢指针找到数组中第一个是目标值的索引，快指针找到第一个不是目标值的索引。然后在满足条件后，用快指针的值覆盖慢指针的值，慢指针自增。

```c++
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int slow = 0;
        int fast = 0;
        for (; fast < nums.size(); ++fast) {
            if(nums[slow] == val) {
                if (nums[fast] != val) {
                    nums[slow++] = nums[fast];
                    nums[fast] = val; 
                }
            }else {
                ++slow;
            }
        }
        return slow;
    }
};
```

## 优化代码
这次的题目是一次性通过的，顺着思路走的话，只能把快指针放入 `for` 中，用来优化。所以直接看讲解。

## 书中范例代码
由于对双指针思路体会较深，所以已经可以使用这个方法了，但是看到书中的短短几行，我立马觉得作者写的真好，言简意赅。

作者把我用慢指针找第一个应该被覆盖的值的思路，改为先考虑用快指针找第一个不是目标值的索引。起初保持快慢指针在一个位置，如果快指针不为目标值，那么慢指针一样不是目标值，一起自增即可；如果快指针是目标值，那么留下慢指针，快指针自己自增直到找到第一个不是目标值的索引，进行赋值操作即可。

无需判断，慢指针所有的值，都会被快指针的非目标值覆盖一遍。

```c++
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int slowIndex = 0;
        for (int fastIndex = 0; fastIndex < nums.size(); ++fastIndex) {
            if(val != nums[fastIndex]) {
                nums[slowIndex++] = nums[fastIndex]; 
            }
        }
        return slowIndex;
    }
};
```
