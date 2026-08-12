#include <iostream>
#include <string>

using std::string;

int main(int argc, char *argv[]) {
  string str = "hello";
  std::cout << str << std::endl;
  str = str + "world";
  std::cout << str << std::endl;
  return 0;
}
