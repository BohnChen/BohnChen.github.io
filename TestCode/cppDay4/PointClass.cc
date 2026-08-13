#include <iostream>

using std::cout;
using std::endl;

class Point {
public:
  /* Point(int ix = 0, int iy = 0) */
  Point(int ix, int iy) : _ix(ix), _iy(iy) {
    cout << "Point(int = 0, int = 0)" << endl;
  }

  void print() { cout << "(" << _ix << ", " << _iy << ")" << endl; }

  ~Point() { cout << "~Point()" << endl; }

private:
  int _ix;
  int _iy;
};

class Line {
public:
  Line(int x1, int y1, int x2, int y2)
      : _pt1(x1, y1) // 类对象成员需要显示进行初始化，否则就是默认值
        ,
        _pt2(x2, y2) {
    cout << "Line(int, int, int,int)" << endl;
  }

  void printLine() {
    _pt1.print();
    _pt2.print();
  }

  ~Line() { cout << "~Line()" << endl; }

private:
  Point _pt1; // 类对象成员(子对象)
  Point _pt2;
};
int main(int argc, char **argv) {
  Line line(1, 2, 3, 4);
  line.printLine();
  return 0;
}
