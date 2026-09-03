#include "./tinyxml2.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

struct RssItem {
  string title;
  string link;
  string description;
  string content;
};

class RssReader {
public:
  RssReader(const string &filename);
  void parseRss(const string &filename); // 解析
  void dump(const string &filename);     // 输出
  void print();

private:
  void stripHtml(string &str);
  string _filename;
  vector<RssItem> _rss;
};
void RssReader::print() {
  for (auto &v : _rss) {
    std::cout << v.title << endl;
    std::cout << "\t" << v.link << endl;
    std::cout << "\t\t" << v.description << endl;
    std::cout << "\t\t\t" << v.content << endl;
  }
}
void RssReader::dump(const string &filename) {
  std::fstream ofs(filename);
  if (!ofs) {
    cout << "ofs error ." << endl;
    return;
  }
  int idx = 0;
  for (const auto &r : _rss) {
    ofs << "<doc>\n"
        << "\t<docid>" << ++idx << "</docid>\n"
        << "\t<title>" << r.title << "</title>\n"
        << "\t<link>" << r.link << "</link>\n"
        << "\t<description>" << r.description << "</description>\n"
        << "\t<content>" << r.content << "</content>\n"
        << "</doc>\n";
  }
  ofs.close();
}

void RssReader::stripHtml(string &str) {
  std::regex tag("<[^>]+>"); // 匹配 <p> </p> <a href="..."> 等
  str = std::regex_replace(str, tag, "");

  std::regex blank("\\s+"); // 顺手压缩连续空白/换行
  str = std::regex_replace(str, blank, " ");
}

RssReader::RssReader(const string &filename) : _filename(filename) {}

void RssReader::parseRss(const string &filename) {

  tinyxml2::XMLDocument doc;
  if (tinyxml2::XML_SUCCESS != doc.LoadFile(filename.c_str())) {
    std::cerr << "load file error. " << std::endl;
    return;
  }

  tinyxml2::XMLElement *root = doc.RootElement();
  if (!root) {
    cout << "no root." << endl;
    return;
  }

  tinyxml2::XMLElement *channel = root->FirstChildElement("channel");
  if (!channel) {
    cout << "no channel." << endl;
    return;
  }

  for (tinyxml2::XMLElement *item = channel->FirstChildElement("item");
       item != nullptr; item = item->NextSiblingElement("item")) {

    RssItem r;

    tinyxml2::XMLElement *title = item->FirstChildElement("title");
    if (!title) {
      cout << "no title" << endl;
      return;
    }
    r.title = title->GetText();

    tinyxml2::XMLElement *link = item->FirstChildElement("link");
    if (!link) {
      cout << "no link" << endl;
      return;
    }
    r.link = link->GetText();

    tinyxml2::XMLElement *content = item->FirstChildElement("content:encoded");
    if (!content) {
      cout << "no content" << endl;
      return;
    }
    r.content = content->GetText();

    tinyxml2::XMLElement *description = item->FirstChildElement("description");
    if (!description) {
      cout << "no description" << endl;
      return;
    }
    r.description = description->GetText();

    stripHtml(r.description);
    stripHtml(r.content);

    // 这里是一个移动语义，暂时还没学
    // 不用也不错，但是每次循环会有一次深拷贝
    // 效率比较低
    _rss.push_back(std::move(r));
  }
}

int main(int argc, char *argv[]) {

  RssReader rssreader("./coolsehll.xml");
  rssreader.parseRss("./coolsehll.xml");
  // rssreader.print();
  rssreader.dump("./new.txt");
  return 0;
}
