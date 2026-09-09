/*
3.
构建一个类Person，包含字符串成员name（姓名），整型数据成员age（年龄），成员函数
display()用来输出name和age。
   构造函数包含两个参数，用来对name和age初始化。构建一个类Employee由Person派生，包含department（部门），实型
   数据成员salary（工资）,成员函数display（）用来输出职工姓名、年龄、部门、工资，其他成员根据需要自己设定。
   主函数中定义3个Employee类对象，内容自己设定，将其姓名、年龄、部门、工资输出，并计算他们的平均工资。

*/
#include <iostream>
#include <map>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::string;

class Person {
public:
  Person(const string &name, const int &age) : _name(name), _age(age) {}
  ~Person() { cout << "~Person()" << endl; }

  void display() {
    cout << "The name of the Person is " << _name << endl
         << "The age of the Person is " << _age << endl;
  }
  const string &getName() { return _name; }

private:
  string _name;
  int _age;
};

class Employee : public Person {
public:
  Employee(const string name, const int age, const string &department,
           const double &salary)
      : Person(name, age), _department(department), _salary(salary) {
    cout << "This is Employee(const string&, const double &). " << endl;
  }
  ~Employee() { std::cout << "~Employee()." << std::endl; }

  void print() {
    display();
    cout << "Department is " << _department << endl
         << "Salary is " << _salary << endl;
  }

  const double &getSalary() { return _salary; }

private:
  string _department;
  double _salary;
};

int main(int argc, char *argv[]) {
  Employee e1("张东", 20, "设计部", 4300);
  Employee e2("陈敏", 24, "人事部", 3300);
  Employee e3("于晨", 39, "工程部", 4900);
  e1.print();
  e2.print();
  e3.print();
  map<string, double> m;
  m[e1.getName()] = e1.getSalary();
  m[e2.getName()] = e2.getSalary();
  m[e3.getName()] = e3.getSalary();
  double sum = 0;
  for (auto &e : m) {
    sum += e.second;
  }
  double anverageSalary = sum / m.size();
  std::cout << "the anverage salasy is " << anverageSalary << std::endl;
  return 0;
}
