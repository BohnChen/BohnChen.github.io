#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class Point2D {
public:
  Point2D(int x, int y) : _ix(x), _iy(y) {}

  int _publicTest;

protected:
  int _ix;

private:
  int _iy;
};

class Point3D_1 : public Point2D {
public:
  Point3D_1(int x, int y, int z) : Point2D(x, y), _iz(z) {}
  ~Point3D_1() {}
  // cout << _iy << endl; // ERROR _iy仍为 private
  void func() {
    cout << _ix
         << endl; // _ix 仍为 protected, 派生类内可使用，派生类对象不可使用
    // cout << _iy << endl; //  _iy 仍为 private; ERROR
  }

private:
  int _iz;
};
class Point3D_2 : protected Point2D {
public:
  Point3D_2(int x, int y, int z) : Point2D(x, y), _iz(z) {}
  ~Point3D_2() {}
  void func() {
    cout << _ix
         << endl; // _ix 仍为 protected, 派生类内可使用，派生类对象不可使用
    // cout << _iy << endl; //  _iy 仍为 private; ERROR
  }

private:
  int _iz;
};

class Point3D_3 : private Point2D {
public:
  Point3D_3(int x, int y, int z) : Point2D(x, y), _iz(z) {}
  ~Point3D_3() {}
  void func() {
    cout
        << _ix
        << endl; // _ix 在此处降级为 private, 派生类内可使用，派生类对象不可使用
    // cout << _iy << endl; //  _iy 仍为 private; ERROR
  }

private:
  int _iz;
};

void test() {
  Point3D_1 p31(1, 2, 3);
  // p31._ix; // ERROR
  // p31._iy; // ERROR
  p31._publicTest;
  Point3D_2 p32(3, 4, 5);
  // p32._publicTest; // protected inherit , Point3D_2类不可用
  Point3D_3 p33(7, 8, 9);
  //  ERROR, 全错，因为私有继承后，不能访问任何成员对象
  // p33._publicTest;
  // p33._ix;
  // p33._iy;
  // p33._iz;
}
