---
title: "1_Array_Leetcode704_Binary Search"
date: 2026-08-18T15:10:49+08:00
draft: false
categories: ["算法"]
tags: ["leetcode", "技术学习"]
---

# 1_Array_leetcode704_Binary Search

算法的刷题顺序，我按照 《代码随想录》[^1]这本书的刷题顺序进行，文章题目按照序号标定，`1_Array_leetcode704_Binary Search` 中`1`代表第一个模块，名称是`Array`，后面是`leetcode`的题号和题目名称。
[^1]:[代码随想录官网](https://programmercarl.com/)

我刷题的方式是首先自己去`leetcode`写一下题目，通过运行之后，再看讲解，这样进行，收获最大。除非自己实在写不出来，此时，也可以对题目难点有很好的体会，再看书也容易加深印象。

## 题目描述
给定一个 `n` 个元素有序的（升序）整型数组 `nums` 和一个目标值 `target`  ，写一个函数搜索 `nums` 中的 `target`，如果 `target` 存在返回下标，否则返回 `-1`。

你必须编写一个具有 `O(log n)` 时间复杂度的算法。


**示例 1:**

**输入:** nums = [-1,0,3,5,9,12], target = 9

**输出:** 4

**解释:** 9 出现在 nums 中并且下标为 4

**示例 **2:****

**输入:** nums = [-1,0,3,5,9,12], target = 2

**输出:** -1

**解释:** 2 不存在 nums 中因此返回 -1
 

提示：

你可以假设 nums 中的所有元素是不重复的。
n 将在 [1, 10000]之间。
nums 的每个元素都将在 [-9999, 9999]之间。

## 我的代码
代码实现的思想是，将目标值与中间值进行比较，目标值大就改变左边边界，目标值小就改变右边边界，直到找完所有操作数。

```c++
class Solution {
public:
    int search(vector<int>& nums, int target) {
        
        int mid = nums.size() / 2;
        int rindex = nums.size() - 1;
        int lindex = 0;

        // 起初，我将 rindex == lindex 作为跳出条件
        // 但是当测试用例在 nums = [5],target = 5处运行失败后
        // 我增加了上面两行if-else判断，企图将边界值直接输出。
        if (nums[rindex] < target || nums[lindex] > target) {
            return -1;
        }else if (nums[rindex] == target) {
            return rindex;
        }else if (nums[lindex] == target) {
            return lindex;
        }

        // 但是测试用例 nums = [-1,0,3,5,9,12], taget = 3重新让我思考
        // 通过测试用例的方式，只能是将 nums[mid] == target单独拿出来，不与
        // 前两个判断并列
        // 此时成功通过
        while(rindex > lindex) {
            if (target < nums[mid]) {
                rindex = mid - 1;
                mid = lindex + (rindex - lindex) / 2;
            }else if(target > nums[mid]){
                lindex = mid + 1;
                mid = lindex + (rindex - lindex) / 2;
            }
            if(nums[mid] == target){
                return mid;
            }
        }
        return -1;
    }
};
```
## 优化代码
接着，我开始对代码进行优化，考虑能否去掉开头的判断条件，我发现当`rindex == lindex`，但`nums[rindex] == target`时，无法进入循环。如果我想去掉上面的判断，那么就要将`while`的判断条件改成`rindex >= lindex`。此时，循环内部逻辑仍然自洽，即使`target`超出两端边界，循环内最终对`rindex`和`lindex`的运算将使`rindex < lindex`。因此，代码写成了
```c++
class Solution {
public:
    int search(vector<int>& nums, int target) {
        
        int mid = nums.size() / 2;
        int rindex = nums.size() - 1;
        int lindex = 0;

        while(rindex >= lindex) {
            if (target < nums[mid]) {
                rindex = mid - 1;
                mid = lindex + (rindex - lindex) / 2;
            }else if(target > nums[mid]){
                lindex = mid + 1;
                mid = lindex + (rindex - lindex) / 2;
            }

            if(nums[mid] == target){
                return mid;
            }
        }
        return -1;
    }
};

```

## 书中范例代码
翻看讲解，发现作者指出了题目的难点是“到底应该写while(left < right)还是while(left <= right)”，与我遇见问题时的优化思路一样。最终，我的代码，去掉冗余代码，就是范例代码：
```c++
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int rindex = nums.size() - 1;
        int lindex = 0;

        while(rindex >= lindex) {
            int mid = lindex + (rindex - lindex) / 2;
            if (target < nums[mid]) {
                rindex = mid - 1;
            } else if(target > nums[mid]){
                lindex = mid + 1;
            } else { // (nums[mid] == target){
                return mid;
            }
        }
        return -1;
    }
};

```

