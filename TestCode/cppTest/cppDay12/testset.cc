#include <iostream>
#include <map>
#include <set>

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::set;
using std::string;

int main(int argc, char *argv[]) {
  set<int> ia;

  // initializer list constructor
  set<int> ib{1, 2, 3, 4};
  for (auto &str : ib)
    cout << str << ", ";
  cout << endl;

  // iterator constructor
  set<int> ic(ib.find(2), ib.end());
  for (auto &str : ic)
    cout << str << ", ";
  cout << endl;

  // copy constructor
  set<int> id(ic);
  for (auto &str : id)
    cout << str << ", ";
  cout << endl;
  auto it = id.begin();

  map<int, string> hi;
  auto it1 = hi.begin();

  return 0;
}
