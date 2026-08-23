#ifndef MYLOGGER_HH
#define MYLOGGER_HH

#include <log4cpp/Category.hh>

// 带文件/函数/行号的日志宏：把位置信息拼进格式串，再转发给 Mylogger 方法
// 注意：位置参数(__FILE__/__LINE__/__FUNCTION__)必须排在用户参数之前，
// 才能与格式串开头的 %s %d %s 一一对应。用 (fmt, ...) 形式，##__VA_ARGS__ 允许无额外参数。
#define LOG_INFO(fmt, ...)                                                     \
  Mylogger::getInstance()->info("[%s:%d] %s: " fmt, __FILE__, __LINE__,        \
                                __FUNCTION__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)                                                     \
  Mylogger::getInstance()->warn("[%s:%d] %s: " fmt, __FILE__, __LINE__,        \
                                __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
  Mylogger::getInstance()->error("[%s:%d] %s: " fmt, __FILE__, __LINE__,       \
                                 __FUNCTION__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                                                    \
  Mylogger::getInstance()->debug("[%s:%d] %s: " fmt, __FILE__, __LINE__,       \
                                 __FUNCTION__, ##__VA_ARGS__)

class Mylogger {
public:
  static Mylogger *getInstance(); // 单例入口

  void info(const char *fmt, ...);
  void warn(const char *fmt, ...);
  void error(const char *fmt, ...);
  void debug(const char *fmt, ...);

private:
  Mylogger(); // 私有：构造里调 init()，只初始化一次
  ~Mylogger();

  void init(); // 一次性初始化：appender/layout/优先级

  log4cpp::Category &_cat; // 只取一次 category
};

#endif
