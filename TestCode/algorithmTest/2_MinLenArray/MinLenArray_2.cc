/*
 *  滑动窗口实现
 *
 *
 *
 * */
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

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
      // 当总和小于目标值就退出
      // 对每一块满足 sum >= target 的起点，都进行滑动操作
      while (sum >= target) {
        subLenth = i - numsbeg + 1;
        res = subLenth < res ? subLenth : subLenth;
        // 开始滑动,
        sum -= nums[numsbeg++];
      }
    }

    return res == INT32_MAX ? 0 : res;
  }
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  Solution s;
  int target = 7;
  vector<int> nums{2, 3, 1, 2, 4, 3};
  cout << s.minSubArrayLen(target, nums) << endl;
  return 0;
}
