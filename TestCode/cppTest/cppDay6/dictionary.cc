#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::ifstream;
using std::ofstream;
using std::streampos;
using std::string;
using std::vector;

struct Record {
  string _word;
  int _frequency;
};

class Dictionary {
public:
  //......
  void ReadAndStore(const std::string &filename);
  void CleanFile(const std::string &filename, const std::string &fileNewname);

private:
  vector<Record> _dict;
};

void Dictionary::ReadAndStore(const std::string &filename) {
  ifstream ifs(filename);
  if (!ifs) {
    std::cerr << "read() : ifs is error." << std::endl;
    return;
  }

  ifs.seekg(0, std::ios_base::end);
  std::streamsize length = ifs.tellg();
  ifs.seekg(0, std::ios_base::beg);

  std::string content(length, '\0');
  ifs.read(&content[0], length); // 读 length，不多读

  std::unordered_map<string, int> freq;
  std::istringstream iss(content);
  string word;
  while (iss >> word) {
    ++freq[word];
  }

  std::vector<Record> result;
  result.reserve(freq.size());
  for (const auto &[w, n] : freq) {
    result.push_back({w, n});
  }

  // 按 ASCII 码排序
  std::sort(result.begin(), result.end(),
            [](const Record &a, const Record &b) { return a._word < b._word; });

  ofstream ofs("./static/TestCode/cppDay6/orderbyword.txt");
  if (!ofs) {
    std::cerr << "read() : ofs is error." << std::endl;
    return;
  }

  for (const auto &r : result) {
    ofs << r._word << "\t" << r._frequency << '\n';
  }

  // 按词频降序
  std::sort(result.begin(), result.end(), [](const Record &a, const Record &b) {
    return a._frequency > b._frequency;
  });
  ofstream ofs2("./static/TestCode/cppDay6/orderbyfrequency.txt");
  if (!ofs2) {
    std::cerr << "read() : ofs2 is error." << std::endl;
    return;
  }

  for (const auto &r : result) {
    ofs2 << r._word << "\t" << r._frequency << '\n';
  }

  ifs.close();
  ofs.close();
  ofs2.close();
}

void Dictionary::CleanFile(const std::string &filename,
                           const std::string &fileNewname) {
  ifstream ifs(filename, std::ios::binary);
  if (!ifs) {
    std::cerr << "ifs is error." << std::endl;
    return;
  }

  ifs.seekg(0, std::ios_base::end);
  std::streamsize length = ifs.tellg();
  ifs.seekg(0, std::ios_base::beg);

  std::string content(length, '\0');
  ifs.read(&content[0], length); // 读 length，不多读

  int slow = 0;
  for (int fast = 0; fast < (int)content.length(); ++fast) {
    char c = content[fast];
    if ((c <= 'z' && c >= 'a') || (c >= 'A' && c <= 'Z') || c == ' ' ||
        c == '\n') {
      content[slow++] = c;
    }
  }
  content.resize(slow); // 删掉尾部垃圾

  ofstream ofs(fileNewname, std::ios::binary);
  if (!ofs) {
    std::cerr << "ofs is error." << std::endl;
    return;
  }
  ofs.write(content.c_str(), content.size());
}

// deepseek 优化的高级版本，用了 erase + remove_if
/*
void Dictionary::cleanFile(const std::string &filename,
                           const std::string &newname) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs) {
    std::cerr << "open error\n";
    return;
  }

  // 1. 取文件大小
  ifs.seekg(0, std::ios::end);
  std::streamoff size = ifs.tellg(); // 类型明确
  ifs.seekg(0, std::ios::beg);

  // 2. 一次性读进 vector<char>
  std::vector<char> buf(static_cast<size_t>(size));
  if (size > 0)
    ifs.read(buf.data(), size);

  // 3. 剔除除英文字母外的所有字符
  buf.erase(std::remove_if(buf.begin(), buf.end(),
                           [](unsigned char c) {
                             return !std::isalpha(c) && c != ' ' && c != '\n';
                           }),
            buf.end());

  // 4. 写回新文件
  std::ofstream ofs(newname, std::ios::binary);
  ofs.write(buf.data(), static_cast<std::streamsize>(buf.size()));
}
*/

int main(int argc, char *argv[]) {
  Dictionary dic;
  const string filename = "./static/TestCode/cppDay6/The_Holy_Bible.txt";
  const string filenewname = "./static/TestCode/cppDay6/new.txt";
  // 我们先将文件做一次处理
  dic.CleanFile(filename, filenewname);

  dic.ReadAndStore(filenewname);

  return 0;
}
