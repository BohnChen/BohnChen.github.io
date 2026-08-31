#pragma once

class Widget {
private:
  // 当然 struct 也是可以的
  // 这里只做前置声明，实现放进 cpp 实现文件
  class WidgetPimpl;
  WidgetPimpl *_pimpl;

public:
  Widget();
  ~Widget();
  Widget(const Widget &other);            // 拷贝构造
  Widget &operator=(const Widget &other); // 拷贝赋值
  void print();
};
