#include <iostream>

// 用上一个例子的 Logger 对比：
//  1. 抽象依赖
class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void log(const std::string &msg) = 0;
};

class ConsoleLogger : public ILogger {
public:
  void log(const std::string &msg) override { /* 打印到控制台 */ }
};

class FileLogger : public ILogger {
public:
  void log(const std::string &msg) override { /* 写入文件 */ }
};

// 2. 使用方：不自己 new，构造函数接收依赖
class UserService {
public:
  explicit UserService(ILogger &logger) : logger_(logger) {}

  void createUser() {
    logger_.log("creating user...");
    // ...
  }

private:
  ILogger &logger_; // 只依赖抽象，不知道具体实现
};

// 3. 组合根：main 里统一组装
int main() {
  FileLogger logger;           // 创建具体依赖
  UserService service(logger); // 注入
  service.createUser();

  // 想换成控制台版？只改这一行：
  // ConsoleLogger logger;
  return 0;
}
