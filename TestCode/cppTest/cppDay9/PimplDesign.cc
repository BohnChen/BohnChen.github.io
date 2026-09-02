#include "Pimpl.h"

#include <iostream>

using std::cout;
using std::endl;

class Widget::WidgetPimpl {
public:
  WidgetPimpl(int x, int y) : _x(x), _y(y) {}
  void printWidgetPimpl() const;
  ~WidgetPimpl() {}

private:
  int _x;
  int _y;
};

void Widget::WidgetPimpl::printWidgetPimpl() const {
  cout << "_x is " << _x << ", _y is " << _y << endl;
}

Widget::Widget() : _pimpl(new WidgetPimpl(1, 2)) {}

Widget::~Widget() {
  if (_pimpl) {
    delete _pimpl;
    _pimpl = nullptr;
  }
}

Widget::Widget(
    const Widget
        &other) // 拷贝构造
                // WidgetPimpl 也要有可用的拷贝构造，因为数据成员是int，所以省略
    : _pimpl(new WidgetPimpl(*other._pimpl)) {}

Widget &Widget::operator=(const Widget &other) { // 拷贝赋值
  if (this != &other) {                          // 防自赋值
    delete _pimpl;                               // 释放旧内存
    _pimpl = new WidgetPimpl(*other._pimpl);
  }
  return *this;
}

void Widget::print() { _pimpl->printWidgetPimpl(); }
