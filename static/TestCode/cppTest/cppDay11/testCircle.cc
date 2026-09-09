
/*
1. 编写一个圆类Circle，该类拥有：

        ① 1个成员变量，存放圆的半径；
        ② 两个构造方法
          Circle( )              // 将半径设为0
          Circle(double  r )     //创建Circle对象时将半径初始化为r
        ③ 三个成员方法
          double getArea( )      //获取圆的面积
          double getPerimeter( ) //获取圆的周长
          void  show( )          //将圆的半径、周长、面积输出到屏幕

*/

#include <iostream>

using std::cin;
using std::cout;
using std::endl;

const double PI = 3.1415926;

class Circle {
public:
  Circle() : _r(0) {}
  Circle(const double &r) : _r(r) {}
  ~Circle() { cout << "~Circle()" << endl; }

  double getArea() { return PI * _r * _r; }

  double getPerimeter() { return 2 * PI * _r; }

  void show() {
    cout << "the radius of the circle is " << _r << endl
         << "the Area of the Circle is " << getArea() << endl
         << "the Perimeter of the Circle is " << getPerimeter() << endl;
  }

private:
  double _r;
};

/*
2. 编写一个圆柱体类Cylinder，它继承于上面的Circle类，还拥有：
   ① 1个成员变量，圆柱体的高；
   ② 构造方法
   Cylinder (double r, double  h) //创建Circle对象时将半径初始化为r
   ③ 成员方法
   double getVolume( )   //获取圆柱体的体积
   void  showVolume( )   //将圆柱体的体积输出到屏幕
   编写应用程序，创建类的对象，分别设置圆的半径、圆柱体的高，计算并分别显示圆半径、圆面积、圆周长，圆柱体的体积。



*/

class Cylinder : public Circle {
private:
  double _dheight;

public:
  Cylinder(double r, double h) : Circle(r), _dheight(h) {
    std::cout << "this is Cylinder()" << std::endl;
  }

  double getVolume() { return getArea() * _dheight; }

  void showVolume() {
    std::cout << "the Volume of the Cylinder is " << getVolume() << std::endl;
  }
};

int main(int argc, char *argv[]) {
  Cylinder c1(3, 5);
  c1.showVolume();

  return 0;
}
