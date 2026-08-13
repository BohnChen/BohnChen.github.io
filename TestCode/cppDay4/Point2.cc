#include <iostream>

using std::cout;
using std::endl;

class Point {
public:
  // 构造函数的特点：完成数据成员的初始化
  Point(int ix = 0, int iy = 0) : _ix(ix), _iy(iy) {
    cout << "Point(int = 0, int = 0)" << endl;
  }

  // Q:拷贝构造函数参数中的引用符号可以去掉吗？
  // A:不能去掉，如果去掉引用符号，在满足拷贝构造函数的
  // 情况，会满足拷贝构造函数的调用时机1，然后进行无限循环调用
  // 拷贝构造函数，函数的参数是会入栈，这样的话栈空间肯定会溢出
  // 的，所以引用符号不能去掉
  //
  // Q：拷贝构造函数参数中的const可以去掉吗？
  // A:不能去掉，当传递的是右值（临时对象、临时变量、匿名对象）
  // 的时候，会返回报错（非const左值引用不能绑定到右值），当传递
  // 临时对象的时候，就会报错
  //
  // 默认情况下，编译器会自动生成拷贝构造函数
  /* Point(const Point &rhs)//Point pt2 = pt1 */
  Point(const Point &rhs) // Point pt2 = pt1
      : _ix(rhs._ix), _iy(rhs._iy) {
    cout << "Point(const Point &)" << "拷贝构造" << endl;
  }

  // 默认情况下编译器会自动生成赋值运算符函数
  // pt2 = pt1;
  // Q：引用符号可以去掉吗？
  // A:如果去掉，会多执行一次拷贝构造函数，效率就会降低
  //
  // Q:const可以去掉吗：
  // A：如果如右操作数是右值的时候，就会产生非const左值引用不能绑定到
  // 右值的报错
  //
  // Q:赋值运算符函数的返回类型可以是void？
  // A:考虑连等情况
  //
#if 0
  Point &operator=(const Point &rhs) {
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix; // 赋值
    _iy = rhs._iy;

    return *this;
  }
#endif

#if 0
  void operator=(const Point &rhs) {
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix; // 赋值
    _iy = rhs._iy;

    // return *this;
  }

#endif

#if 0
  Point &operator=(Point &rhs) {
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix; // 赋值
    _iy = rhs._iy;

    return *this;
  }

#endif

#if 1
  // 如果去掉参数中的 &，会变为如下
  Point &operator=(const Point rhs) {
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix; // 赋值
    _iy = rhs._iy;

    return *this;
  }

#endif

  // this指针实质：指向对象本身
  // 隐藏在(非静态成员)函数的第一个参数的位置
  void print(/*   Point * const this    */) {
    /* this->_ix = 100;//ok */
    /* this = nullptr;//error */
    cout << "(" << this->_ix << ", " << this->_iy << ")" << endl;
  }

  // 析构函数的作用：完成数据成员的清理工作
  // 析构函数没有返回类型，没有参数，所以具有唯一性，不能重载
  // 对象在离开其作用域被销毁的时候，会自动调用析构函数
  //
  // 默认情况下，编译器会自动生成析构函数
  ~Point() {
    cout << "~Point()" << endl;
    // print();
  }

private:
  int _ix;
  int _iy;
};

Point func() {
  Point p1;
  return p1;
}

void test5() {
  Point p2;
  // 无 const 的赋值构造函数无法绑定到临时对象这种右值
  p2 = func();
}

void test() {
  Point pt1(1, 2);
  Point pt2(3, 4);

  cout << "pt1 = ";
  pt1.print(); // pt1.print(&pt1);
  cout << "pt2 = ";
  pt2.print();
}

void test2() {
  int a = 10;
  int b = 20;
  int c = 30;
  c = b = a;

  Point pt1(1, 2);
  Point pt2(3, 4);
  Point pt3(5, 6);
  cout << "pt1 = ";
  pt1.print();
  cout << "pt2 = ";
  pt2.print();
  cout << "pt3 = ";
  pt3.print();

  cout << endl;
  // Point operator=(const Point &rhs)
  // const Point rhs = pt1
  pt3 = pt2 = pt1;
  /* pt2 = func3(); */
  /* pt2.operator=(pt1);//赋值运算符函数 */
  cout << "pt1 = ";
  pt1.print();
  cout << "pt2 = ";
  pt2.print();
}

void test3() {
  Point pt1(1, 2);
  /* Point pt4 = pt1;//拷贝构造函数 */
  Point pt4(pt1); // 拷贝构造函数

  Point pt2(3, 4);

  pt2 = pt1; // 赋值运算符函数
}
int main(int argc, char **argv) {
  test2();

  return 0;
}
