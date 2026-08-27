---
title: "2_Array_LC209_滑动窗口"
date: 2026-08-27T14:47:12+08:00
draft: false
categories: ["算法"]
tags: ["leetcode", "技术学习"]
---


## 我的代码
我起初的思路就是将从每一个起点开始所有可能子串全部计算并与目标值进行比较，然后得出最短的复合要求的子串长度值。

由于复杂度较高，为 O(n^2)的复杂度，所以当进行大数据量的计算时，超出了运行时间限制
```c++

class Solution {
public:
  int minSubArrayLen(int target, vector<int> &nums) {
    int minlen = 0;
    int tmplen = nums.size();
    for (int x = 0; x < nums.size(); ++x) {
      int tmp = nums[x];
      if (tmp >= target) {
        minlen = 1;
        break;
      }
      for (int y = x + 1; y < nums.size(); ++y) {
        tmp += nums[y];
        if (tmp < target) {
          continue;
        } else {
          tmplen = y - x + 1;
          // cout << "Test Info " << minlen << endl;
          if (0 == minlen) {
            minlen = tmplen;
          } else if (minlen > tmplen) {
            minlen = tmplen;
          }
          break;
        }
      }
    }
    return minlen;
  }
};


```


## 优化的代码

考虑滑动窗口的方式进行。
固定同一个起点，开始增加子串长度，直到子串和大于目标值，从起点位置开始减短子串长度。

核心思想在于，如果一个更短的子串和也大于目标值，那么它一定在符合要求的长串中。所以找到一段复合要求的长串之后，只需要更改起点，直到不满足 sum >= target 后，再移动终点坐标。

```c++
class Solution {
public:
  int minSubArrayLen(int target, vector<int> &nums) {
    // 最终结果
    int res = INT32_MAX;

    // 保存子串和
    int sum = 0;

    // 保存子串长度
    int subLenth = 0;

    // 定义起点
    int numsbeg = 0;

    // 控制滑动窗口长度
    for (int i = 0; i < nums.size(); ++i) {
      // 计算总和
      sum += nums[i];
     
      // 对每一块满足 sum >= target 的起点，都进行滑动操作
      while (sum >= target) {
        subLenth = i - numsbeg + 1;
        res = res < subLenth ? res : subLenth;
        // 开始滑动，当总和小于目标值就退出
        sum -= nums[numsbeg++];
      }
    }

    return res == INT32_MAX ? 0 : res;
  }
};


```


## 总结
用上算法来操作后，效率确实会提升很多。
