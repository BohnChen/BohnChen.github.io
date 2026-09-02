#include <string>

class Logger {
  Logger() = default;
  ~Logger() = default;

public:
  static Logger &getInstance() {
    static Logger instance; // 首次调用后创建唯一实例
    return instance;
  }

  void log(const std::string &msg) { /** Test Code **/ }
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
};

int main(int argc, char *argv[]) {
  Logger::getInstance().log("Hello, this is singleton. ");

  return 0;
}
