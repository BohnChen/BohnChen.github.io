/*

那两个问题分别怎么修的
① 线程不安全 → 利用"magic statics"
instance 是函数内静态变量，C++11 标准保证它的初始化只发生一次且线程安全。new
TestSingleton() 在静态初始化内部执行，所以不再有 if (nullptr) 竞态，连
call_once/mutex 都不用写。

② 指针悬空/泄漏 → 生命周期交给 unique_ptr 堆对象由
unique_ptr 独占持有，程序退出时自动 delete：
        - 没有 destroy() → 不存在"忘了调就泄漏"
        - 对象存活到程序结束 → 引用永远不会悬空（被 delete
的是退出阶段，此时已无人访问）
        - 私有析构 + 自定义 deleter → 外部无法
delete，也无法绕过个版本，而不是裸指针 + 手动 destroy()。

 */
#include <iostream>
#include <memory>

class TestSingleton {
  static void destroyHelper(TestSingleton *p) { delete p; } // 私有 deleter

public:
  static TestSingleton &getInstance() {
    static std::unique_ptr<TestSingleton, void (*)(TestSingleton *)> instance(
        new TestSingleton(), destroyHelper);
    return *instance;
  }

  void TestFunc() { std::cout << "Test Func" << std::endl; }

  TestSingleton(const TestSingleton &) = delete;
  TestSingleton &operator=(const TestSingleton &) = delete;

private:
  TestSingleton() { std::cout << "TestSingleton()" << std::endl; }
  ~TestSingleton() { std::cout << "~TestSingleton()" << std::endl; }
};

int main() {
  TestSingleton::getInstance().TestFunc();
  return 0;
}
