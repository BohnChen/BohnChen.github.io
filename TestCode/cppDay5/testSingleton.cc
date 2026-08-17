/*
                        当你有"必须在特定时刻精确销毁、然后还能重建"的硬需求（比如单测里隔离重置、模拟重启）时，堆版本才有意义。
                        那种情况建议：getInstance() 里加 std::mutex（或
   std::call_once）修竞态，destroy() 后手动把 _pInstance 置空。

                        否则，MeyerSingleton.cc的 Meyer 版最为合适。

 */
#include <iostream>

class TestSingleton {

public:
  static TestSingleton &getInstance() {
    // 两个线程同时调用可能会破坏单例模式
    // 两个线程同时判断可能会有竞态
    if (_pInstance == nullptr) {
      _pInstance = new TestSingleton();
    }
    return *_pInstance;
  }

  void TestFunc() { std::cout << "Test Func" << std::endl; }

  static void destroy() {
    if (_pInstance) {
      delete _pInstance;
      _pInstance = nullptr;
    }
  }

  TestSingleton(const TestSingleton &) = delete;
  TestSingleton &operator=(const TestSingleton &) = delete;

private:
  TestSingleton();
  ~TestSingleton();

  // 必须是静态，才能让所有对象共享一个内容。
  static TestSingleton *_pInstance;
};

// 如果没有这句定义，只有类中的声明，编译器只知道有这个变量
// 只有遇见这句之后，编译器才会为 _pInstance 分配空间
// 因此，当缺少这句时，会产生链接错误
TestSingleton *TestSingleton::_pInstance = nullptr;

TestSingleton::TestSingleton() { std::cout << "TestSingleton()" << std::endl; }
TestSingleton::~TestSingleton() {
  std::cout << "~TestSingleton()" << std::endl;
}

int main(int argc, char *argv[]) {
  // 如果 getInstance 返回类型是 * 而不是 &，那么
  // 下面的三个指针在各自调用了 destroy 后，会悬空，
  // 重新访问就崩溃。所以用完需要置空。
  TestSingleton &ps1 = TestSingleton::getInstance();
  TestSingleton &ps2 = TestSingleton::getInstance();
  TestSingleton &ps3 = TestSingleton::getInstance();
  ps1.TestSingleton::destroy();
  ps2.TestSingleton::destroy();
  ps3.TestSingleton::destroy();

  return 0;
}
