#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cerr;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

void test() {
  ifstream ifs("./static/TestCode/cppDay6/testvector.cc");
  if (!ifs.good()) {
    cerr << ">> ifstream open file error!\n";
    return;
  }
  string line;
  vector<string> vec;
  while (getline(ifs, line)) {
    // cout << line << endl;
    vec.push_back(line);
  }
  ifs.close();
  ofstream ofs("./static/TestCode/cppDay6/a.txt");
  if (!ofs.good()) {
    cerr << ">> ofstream open file error!\n";
    return;
  }
  for (auto &elem : vec) {
    ofs << elem << '\n';
  }
  ofs.close();
}

int main(int argc, char *argv[]) {
  test();
  return 0;
}
