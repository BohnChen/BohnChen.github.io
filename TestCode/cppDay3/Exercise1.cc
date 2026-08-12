/**
S
定义一个学生类，其中有3个数据成员：学号、姓名、年龄，以及若干成员函数。同时编写main函数使用这个类，实现对学生数据的赋值和输出

*/
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class Student {
  int _stuNum;
  string _name;
  int _year;

public:
  Student(int stuNum, string name, int year)
      : _stuNum(stuNum), _name(name), _year(year) {}
  ~Student() = default;

  void getStuInfo() {
    cout << "The student info is:" << endl
         << "stuNum: " << _stuNum << endl
         << "name: " << _name << endl
         << "year: " << _year << endl
         << endl;
  }
};

int main(int argc, char *argv[]) {
  Student stu_1(1, string("nice"), 18);
  Student *stu_2 = new Student(2, string("day"), 21);
  stu_1.getStuInfo();
  stu_2->getStuInfo();

  delete stu_2;
  return 0;
}
