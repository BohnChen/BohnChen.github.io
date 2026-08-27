/*
 * 时间复杂度O(n^2),遇到大数据量的运算超出时间限制
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

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  Solution s;
  int target = 7;
  vector<int> nums{2, 3, 1, 2, 4, 3};
  cout << s.minSubArrayLen(target, nums) << endl;
  return 0;
}
