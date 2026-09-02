#include <iostream>

using std::cout;
using std::endl;

class Point {
public:
  Point(int ix = 0, int iy = 0) : _ix(ix), _iy(iy) /* , _ref(_ix) */ {
    cout << "Point(int = 0, int = 0)" << endl;
    /* _ix = ix;//error,赋值 */
    /* _iy = iy; */
  }

  void print() { cout << "(" << _ix << ", " << _iy << ")" << endl; }

  ~Point() { cout << "~Point()" << endl; }

private:
  int _ix;
  int _iy;
  // int &_ref;
};

// 成员函数不占用类的大小，成员函数存在于程序代码区，被该类的
// 左右对象共享
//
int main(int argc, char **argv) {
  cout << "sizeof(Point) = " << sizeof(Point) << endl;

  Point pt(1, 2); // 栈对象
  return 0;
}
